/*
** thingdef.cpp
**
**---------------------------------------------------------------------------
** Copyright 2011 Braden Obrzut
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
**
*/

#include "actor.h"
#include "a_inventory.h"
#include "doomerrors.h"
#include "id_ca.h"
#include "lnspec.h"
#include "m_random.h"
#include "r_sprites.h"
#include "scanner.h"
#include "w_wad.h"
#include "wl_def.h"
#include "wl_draw.h"
#include "thingdef/thingdef.h"
#include "thingdef/thingdef_type.h"
#include "thingdef/thingdef_expression.h"
#include "thinker.h"
#include "templates.h"
#include "g_mapinfo.h"

#include <climits>

// Code pointer stuff
void InitFunctionTable(ActionTable *table);
void ReleaseFunctionTable();
ActionInfo *LookupFunction(const FName &func, const ActionTable *table);

////////////////////////////////////////////////////////////////////////////////

#define DEFINE_FLAG(prefix, flag, type, variable) { NATIVE_CLASS(type), prefix##_##flag, #type, #flag, typeoffsetof(A##type,variable) }
const struct FlagDef
{
	public:
		const ClassDef * const &cls;
		const flagstype_t	value;
		const char* const	prefix;
		const char* const	name;
		const int			varOffset;
} flags[] =
{
	DEFINE_FLAG(FL, ALWAYSFAST, Actor, flags),
	DEFINE_FLAG(WF, ALWAYSGRIN, Weapon, weaponFlags),
	DEFINE_FLAG(IF, ALWAYSPICKUP, Inventory, itemFlags),
	DEFINE_FLAG(FL, AMBUSH, Actor, flags),
	DEFINE_FLAG(IF, AUTOACTIVATE, Inventory, itemFlags),
	DEFINE_FLAG(FL, BRIGHT, Actor, flags),
	DEFINE_FLAG(FL, CANUSEWALLS, Actor, flags),
	DEFINE_FLAG(FL, COUNTITEM, Actor, flags),
	DEFINE_FLAG(FL, COUNTKILL, Actor, flags),
	DEFINE_FLAG(FL, COUNTSECRET, Actor, flags),
	DEFINE_FLAG(WF, DONTBOB, Weapon, weaponFlags),
	DEFINE_FLAG(FL, DONTRIP, Actor, flags),
	DEFINE_FLAG(FL, DROPBASEDONTARGET, Actor, flags),
	DEFINE_FLAG(IF, INVBAR, Inventory, itemFlags),
	DEFINE_FLAG(FL, ISMONSTER, Actor, flags),
	DEFINE_FLAG(FL, MISSILE, Actor, flags),
	DEFINE_FLAG(WF, NOALERT, Weapon, weaponFlags),
	DEFINE_FLAG(WF, NOAUTOFIRE, Weapon, weaponFlags),
	DEFINE_FLAG(WF, NOGRIN, Weapon, weaponFlags),
	DEFINE_FLAG(FL, OLDRANDOMCHASE, Actor, flags),
	DEFINE_FLAG(FL, PICKUP, Actor, flags),
	DEFINE_FLAG(FL, PLOTONAUTOMAP, Actor, flags),
	DEFINE_FLAG(FL, RANDOMIZE, Actor, flags),
	DEFINE_FLAG(FL, RIPPER, Actor, flags),
	DEFINE_FLAG(FL, REQUIREKEYS, Actor, flags),
	DEFINE_FLAG(FL, SHOOTABLE, Actor, flags),
	DEFINE_FLAG(FL, SOLID, Actor, flags)
};
extern const PropDef properties[];

////////////////////////////////////////////////////////////////////////////////

StateLabel::StateLabel(const FString &str, const ClassDef *parent, bool noRelative)
{
	Scanner sc(str.GetChars(), str.Len());
	Parse(sc, parent, noRelative);
}

StateLabel::StateLabel(Scanner &sc, const ClassDef *parent, bool noRelative)
{
	Parse(sc, parent, noRelative);
}

const Frame *StateLabel::Resolve() const
{
	return cls->FindStateInList(label) + offset;
}

const Frame *StateLabel::Resolve(AActor *owner, const Frame *caller, const Frame *def) const
{
	if(isRelative)
		return caller + offset;
	else if(isDefault)
		return def;

	const Frame *frame = owner->GetClass()->FindStateInList(label);
	if(frame)
		return frame + offset;
	return NULL;
}

void StateLabel::Parse(Scanner &sc, const ClassDef *parent, bool noRelative)
{
	cls = parent;

	if(!noRelative && sc.CheckToken(TK_IntConst))
	{
		isRelative = true;
		offset = sc->number;
		return;
	}

	isRelative = false;
	isDefault = sc.CheckToken('*');
	if(isDefault)
		return;

	sc.MustGetToken(TK_Identifier);
	label = sc->str;
	if(sc.CheckToken(TK_ScopeResolution))
	{
		if(label.CompareNoCase("Super") == 0)
		{
			// This should never happen in normal use, but just in case.
			if(parent->parent == NULL)
				sc.ScriptMessage(Scanner::ERROR, "This actor does not have a super class.");
			cls = parent->parent;
		}
		else
		{
			do
			{
				cls = cls->parent;
				if(cls == NULL)
					sc.ScriptMessage(Scanner::ERROR, "%s is not a super class.", label.GetChars());
			}
			while(stricmp(cls->GetName().GetChars(), label.GetChars()) != 0);
		}

		sc.MustGetToken(TK_Identifier);
		label = sc->str;
	}

	while(sc.CheckToken('.'))
	{
		sc.MustGetToken(TK_Identifier);
		label = label + "." + sc->str;
	}

	if(sc.CheckToken('+'))
	{
		sc.MustGetToken(TK_IntConst);
		offset = sc->number;
	}
	else
		offset = 0;
}

////////////////////////////////////////////////////////////////////////////////

struct StateDefinition
{
	public:
		enum NextType
		{
			GOTO,
			LOOP,
			WAIT,
			STOP,

			NORMAL
		};

		FString		label;
		char		sprite[5];
		FString		frames;
		int			duration;
		unsigned	randDuration;
		bool		fullbright;
		NextType	nextType;
		FString		nextArg;
		StateLabel	jumpLabel;
		Frame::ActionCall	functions[2];
};

static TArray<const SymbolInfo *> *symbolPool = NULL;

SymbolInfo::SymbolInfo(const ClassDef *cls, const FName &var, const int offset) :
	cls(cls), var(var), offset(offset)
{
	if(symbolPool == NULL)
		symbolPool = new TArray<const SymbolInfo *>();
	symbolPool->Push(this);
}

int SymbolCompare(const void *s1, const void *s2)
{
	const Symbol * const sym1 = *((const Symbol **)s1);
	const Symbol * const sym2 = *((const Symbol **)s2);
	if(sym1->GetName() < sym2->GetName())
		return -1;
	else if(sym1->GetName() > sym2->GetName())
		return 1;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void ExprSin(AActor *self, ExpressionNode::Value &out, ExpressionNode* const *args, FRandom *rng)
{
	out = double(finesine[(args[0]->Evaluate(self).GetInt()%360)*FINEANGLES/360])/FRACUNIT;
}

void ExprCos(AActor *self, ExpressionNode::Value &out, ExpressionNode* const *args, FRandom *rng)
{
	out = double(finecosine[(args[0]->Evaluate(self).GetInt()%360)*FINEANGLES/360])/FRACUNIT;
}

void ExprRandom(AActor *self, ExpressionNode::Value &out, ExpressionNode* const *args, FRandom *rng)
{
	int64_t min = args[0]->Evaluate(self).GetInt();
	int64_t max = args[1]->Evaluate(self).GetInt();
	if(min > max)
		out = max+(*rng)((int)(min-max+1));
	else
		out = min+(*rng)((int)(max-min+1));
}

void ExprFRandom(AActor *self, ExpressionNode::Value &out, ExpressionNode* const *args, FRandom *rng)
{
	static const unsigned int randomPrecision = 0x80000000;

	double min = args[0]->Evaluate(self).GetDouble();
	double max = args[1]->Evaluate(self).GetDouble();
	out = min+(double((*rng)(randomPrecision))/randomPrecision)*(max-min);
}

static const struct ExpressionFunction
{
	const char* const				name;
	int								ret;
	unsigned short					args;
	bool							takesRNG;
	FunctionSymbol::ExprFunction	function;
} functions[] =
{
	{ "cos",		TypeHierarchy::FLOAT,	1,	false,	ExprCos },
	{ "frandom",	TypeHierarchy::FLOAT,	2,	true,	ExprFRandom },
	{ "random",		TypeHierarchy::INT,		2,	true,	ExprRandom },
	{ "sin",		TypeHierarchy::FLOAT,	1,	false,	ExprSin },

	{ NULL, 0, false, NULL }
};

////////////////////////////////////////////////////////////////////////////////

class MetaTable::Data
{
	public:
		Data(MetaTable::Type type, uint32_t id) : id(id), type(type), inherited(false), next(NULL) {}
		~Data()
		{
			SetType(MetaTable::INTEGER);
		}

		void	SetType(MetaTable::Type type)
		{
			// As soon as we try to change the value consider the meta data new
			inherited = false;
			if(this->type == type)
				return;

			if(this->type == MetaTable::STRING)
			{
				delete[] value.string;
				value.string = NULL;
			}

			this->type = type;
		}

		const Data &operator= (const Data &other)
		{
			id = other.id;
			SetType(other.type);
			inherited = true;

			switch(type)
			{
				case MetaTable::INTEGER:
					value.integer = other.value.integer;
					break;
				case MetaTable::STRING:
					value.string = new char[strlen(other.value.string)+1];
					strcpy(value.string, other.value.string);
					break;
				case MetaTable::FIXED:
					value.fixedPoint = other.value.fixedPoint;
					break;
			}

			return other;
		}

		uint32_t		id;
		MetaTable::Type	type;
		bool			inherited;
		union
		{
			int			integer;
			fixed		fixedPoint;
			char*		string;
		} value;
		Data			*next;
};

MetaTable::MetaTable() : head(NULL)
{
}

MetaTable::~MetaTable()
{
	FreeTable();
}

void MetaTable::CopyMeta(const MetaTable &other)
{
	Data *data = other.head;
	while(data)
	{
		Data *copyData = FindMetaData(data->id);
		*copyData = *data;

		data = data->next;
	}
}

MetaTable::Data *MetaTable::FindMeta(uint32_t id) const
{
	Data *data = head;

	while(data != NULL)
	{
		if(data->id == id)
			break;

		data = data->next;
	}

	return data;
}

MetaTable::Data *MetaTable::FindMetaData(uint32_t id)
{
	Data *data = FindMeta(id);
	if(data == NULL)
	{
		data = new MetaTable::Data(MetaTable::INTEGER, id);
		data->next = head;
		head = data;
	}

	return data;
}

void MetaTable::FreeTable()
{
	Data *data = head;
	while(data != NULL)
	{
		Data *prevData = data;
		data = data->next;
		delete prevData;
	}
}

int MetaTable::GetMetaInt(uint32_t id, int def) const
{
	Data *data = FindMeta(id);
	if(!data)
		return def;
	return data->value.integer;
}

fixed MetaTable::GetMetaFixed(uint32_t id, fixed def) const
{
	Data *data = FindMeta(id);
	if(!data)
		return def;
	return data->value.fixedPoint;
}

const char* MetaTable::GetMetaString(uint32_t id) const
{
	Data *data = FindMeta(id);
	if(!data)
		return NULL;
	return data->value.string;
}

bool MetaTable::IsInherited(uint32_t id)
{
	Data *data = FindMetaData(id);
	return data->inherited;	
}

void MetaTable::SetMetaInt(uint32_t id, int value)
{
	Data *data = FindMetaData(id);
	data->SetType(MetaTable::INTEGER);
	data->value.integer = value;
}

void MetaTable::SetMetaFixed(uint32_t id, fixed value)
{
	Data *data = FindMetaData(id);
	data->SetType(MetaTable::FIXED);
	data->value.fixedPoint = value;
}

void MetaTable::SetMetaString(uint32_t id, const char* value)
{
	Data *data = FindMetaData(id);
	if(data->type == MetaTable::STRING && data->value.string != NULL)
		delete[] data->value.string;
	else
		data->SetType(MetaTable::STRING);

	data->value.string = new char[strlen(value)+1];
	strcpy(data->value.string, value);
}

////////////////////////////////////////////////////////////////////////////////

static TMap<int, ClassDef *> EditorNumberTable, ConversationIDTable;
SymbolTable ClassDef::globalSymbols;
bool ClassDef::bShutdown = false;

ClassDef::ClassDef() : tentative(false)
{
	defaultInstance = NULL;
	FlatPointers = Pointers = NULL;
	replacement = replacee = NULL;
}

ClassDef::~ClassDef()
{
	if(defaultInstance)
	{
		M_Free(defaultInstance);
	}
	for(unsigned int i = 0;i < symbols.Size();++i)
		delete symbols[i];
}

TMap<FName, ClassDef *> &ClassDef::ClassTable()
{
	static TMap<FName, ClassDef *> classTable;
	return classTable;
}

const size_t ClassDef::POINTER_END = ~(size_t)0;
// [BL] Pulled from ZDoom more or less.
/*
** dobjtype.cpp
** Implements the type information class
**
**---------------------------------------------------------------------------
** Copyright 1998-2008 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/
// Create the FlatPointers array, if it doesn't exist already.
// It comprises all the Pointers from superclasses plus this class's own Pointers.
// If this class does not define any new Pointers, then FlatPointers will be set
// to the same array as the super class's.
void ClassDef::BuildFlatPointers()
{
	if (FlatPointers != NULL)
	{ // Already built: Do nothing.
		return;
	}
	else if (parent == NULL)
	{ // No parent: FlatPointers is the same as Pointers.
		if (Pointers == NULL)
		{ // No pointers: Make FlatPointers a harmless non-NULL.
			FlatPointers = &POINTER_END;
		}
		else
		{
			FlatPointers = Pointers;
		}
	}
	else
	{
		const_cast<ClassDef *>(parent)->BuildFlatPointers ();
		if (Pointers == NULL)
		{ // No new pointers: Just use the same FlatPointers as the parent.
			FlatPointers = parent->FlatPointers;
		}
		else
		{ // New pointers: Create a new FlatPointers array and add them.
			int numPointers, numSuperPointers;

			// Count pointers defined by this class.
			for (numPointers = 0; Pointers[numPointers] != POINTER_END; numPointers++)
			{ }
			// Count pointers defined by superclasses.
			for (numSuperPointers = 0; parent->FlatPointers[numSuperPointers] != POINTER_END; numSuperPointers++)
			{ }

			// Concatenate them into a new array
			size_t *flat = new size_t[numPointers + numSuperPointers + 1];
			if (numSuperPointers > 0)
			{
				memcpy (flat, parent->FlatPointers, sizeof(size_t)*numSuperPointers);
			}
			memcpy (flat + numSuperPointers, Pointers, sizeof(size_t)*(numPointers+1));
			FlatPointers = flat;
		}
	}
}

AActor *ClassDef::CreateInstance() const
{
	if(IsDescendantOf(NATIVE_CLASS(Actor)) && !((AActor*)defaultInstance)->SpawnState)
	{
		((AActor*)defaultInstance)->MeleeState = FindState(NAME_Melee);
		((AActor*)defaultInstance)->MissileState = FindState(NAME_Missile);
		((AActor*)defaultInstance)->PainState = FindState(NAME_Pain);
		((AActor*)defaultInstance)->PathState = FindState(NAME_Path);
		((AActor*)defaultInstance)->SpawnState = FindState(NAME_Spawn);
		((AActor*)defaultInstance)->SeeState = FindState(NAME_See);
	}

	AActor *newactor = (AActor *) M_Malloc(size);
	memcpy((void*)newactor, (void*)defaultInstance, size);
	ConstructNative(this, newactor);
	newactor->Init();
	return newactor;
}

const ClassDef *ClassDef::FindClass(unsigned int ednum)
{
	ClassDef **ret = EditorNumberTable.CheckKey(ednum);
	if(ret == NULL)
		return NULL;
	return *ret;
}

const ClassDef *ClassDef::FindClass(const FName &className)
{
	ClassDef **ret = ClassTable().CheckKey(className);
	if(ret == NULL)
		return NULL;
	return *ret;
}

const ClassDef *ClassDef::FindConversationClass(unsigned int convid)
{
	ClassDef **ret = ConversationIDTable.CheckKey(convid);
	if(ret == NULL)
		return NULL;
	return *ret;
}

const ClassDef *ClassDef::FindClassTentative(const FName &className, const ClassDef *parent)
{
	const ClassDef *search = FindClass(className);
	if(search)
	{
		if(!search->parent->IsDescendantOf(parent))
			I_Error("%s does not inherit %s!", className.GetChars(), parent->GetName().GetChars());
		return search;
	}

	ClassDef *newClass = new ClassDef();
	ClassTable()[className] = newClass;

	newClass->tentative = true;
	newClass->name = className;
	newClass->parent = parent;
	return newClass;
}

const ActionInfo *ClassDef::FindFunction(const FName &function, int &specialNum) const
{
	Specials::LineSpecials special = Specials::LookupFunctionNum(function);
	if(special != Specials::NUM_POSSIBLE_SPECIALS)
	{
		specialNum = special;
		return FindFunction("A_CallSpecial", specialNum);
	}

	if(actions.Size() != 0)
	{
		ActionInfo *func = LookupFunction(function, &actions);
		if(func)
			return func;
	}
	if(parent)
		return parent->FindFunction(function, specialNum);
	return NULL;
}

const Frame *ClassDef::FindState(const FName &stateName) const
{
	const Frame *ret = FindStateInList(stateName);
	return ret;
}

const Frame *ClassDef::FindStateInList(const FName &stateName) const
{
	const unsigned int *ret = stateList.CheckKey(stateName);
	if(ret == NULL)
		return (!parent ? NULL : parent->FindStateInList(stateName));

	// Change the frameLists
	return ResolveStateIndex(*ret);
}

Symbol *ClassDef::FindSymbol(const FName &symbol) const
{
	unsigned int min = 0;
	unsigned int max = symbols.Size()-1;
	unsigned int mid = max/2;
	if(symbols.Size() > 0)
	{
		do
		{
			if(symbols[mid]->GetName() == symbol)
				return symbols[mid];

			if(symbols[mid]->GetName() > symbol)
				max = mid-1;
			else if(symbols[mid]->GetName() < symbol)
				min = mid+1;
			mid = (min+max)/2;
		}
		while(max >= min && max < symbols.Size());
	}

	if(parent)
		return parent->FindSymbol(symbol);
	else if(globalSymbols.Size() > 0)
	{
		// Search globals.
		min = 0;
		max = globalSymbols.Size()-1;
		mid = max/2;
		do
		{
			if(globalSymbols[mid]->GetName() == symbol)
				return globalSymbols[mid];

			if(globalSymbols[mid]->GetName() > symbol)
				max = mid-1;
			else if(globalSymbols[mid]->GetName() < symbol)
				min = mid+1;
			mid = (min+max)/2;
		}
		while(max >= min && max < globalSymbols.Size());
	}
	return NULL;
}

const ClassDef *ClassDef::GetReplacement(bool respectMapinfo) const
{
	return replacement ? replacement->GetReplacement(false) : this;
}

struct Goto
{
	public:
		Frame		*frame;
		FString		remapLabel; // Label: goto Label2
		StateLabel	jumpLabel;
};
void ClassDef::InstallStates(const TArray<StateDefinition> &stateDefs)
{
	// We need to resolve gotos after we install the states.
	TArray<Goto> gotos;

	// Count the number of states we need so that we can allocate the memory
	// in one go and keep our pointers valid.
	unsigned int numStates = 0;
	for(unsigned int iter = 0;iter < stateDefs.Size();++iter)
	{
		if(!stateDefs[iter].label.IsEmpty() && stateDefs[iter].sprite[0] == 0)
			continue;
		numStates += (unsigned int)stateDefs[iter].frames.Len();
	}
	frameList.Resize(numStates);

	FString thisLabel;
	Frame *prevFrame = NULL;
	Frame *loopPoint = NULL;
	Frame *thisFrame = &frameList[0];
	for(unsigned int iter = 0;iter < stateDefs.Size();++iter)
	{
		const StateDefinition &thisStateDef = stateDefs[iter];

		// Special case, `Label: stop`, remove state.  Hmm... I wonder if ZDoom handles fall throughs on this.
		if(!thisStateDef.label.IsEmpty() && thisStateDef.sprite[0] == 0)
		{
			switch(thisStateDef.nextType)
			{
				case StateDefinition::STOP:
					stateList[thisStateDef.label] = INT_MAX;
					break;
				case StateDefinition::NORMAL:
					stateList[thisStateDef.label] = (unsigned int)(thisFrame - &frameList[0]);
					continue;
				case StateDefinition::GOTO:
				{
					Goto thisGoto;
					thisGoto.frame = NULL;
					thisGoto.remapLabel = thisStateDef.label;
					thisGoto.jumpLabel = thisStateDef.jumpLabel;
					gotos.Push(thisGoto);
					continue;
				}
				default:
					Quit("Tried to use a loop on a frameless state.\n");
					break;
			}
			continue;
		}

		for(unsigned int i = 0;i < thisStateDef.frames.Len();++i)
		{
			if(i == 0 && !thisStateDef.label.IsEmpty())
			{
				stateList[thisStateDef.label] = (unsigned int)(thisFrame - &frameList[0]);
				loopPoint = thisFrame;
			}
			memcpy(thisFrame->sprite, thisStateDef.sprite, 4);
			thisFrame->frame = thisStateDef.frames[i]-'A';
			thisFrame->duration = thisStateDef.duration;
			thisFrame->randDuration = thisStateDef.randDuration;
			thisFrame->fullbright = thisStateDef.fullbright;
			thisFrame->action = thisStateDef.functions[0];
			thisFrame->thinker = thisStateDef.functions[1];
			thisFrame->next = NULL;
			thisFrame->index = (unsigned int)(thisFrame - &frameList[0]);
			thisFrame->spriteInf = 0;
			// Only free the action arguments if we are the last frame using them.
			thisFrame->freeActionArgs = i == thisStateDef.frames.Len()-1;
			if(i == thisStateDef.frames.Len()-1) // Handle nextType
			{
				if(thisStateDef.nextType == StateDefinition::WAIT)
					thisFrame->next = thisFrame;
				else if(thisStateDef.nextType == StateDefinition::LOOP)
					thisFrame->next = loopPoint;
				// Add to goto list
				else if(thisStateDef.nextType == StateDefinition::GOTO)
				{
					Goto thisGoto;
					thisGoto.frame = thisFrame;
					thisGoto.jumpLabel = thisStateDef.jumpLabel;
					gotos.Push(thisGoto);
				}
			}
			if(prevFrame != NULL)
				prevFrame->next = thisFrame;

			if(thisStateDef.nextType == StateDefinition::NORMAL || i != thisStateDef.frames.Len()-1)
				prevFrame = thisFrame;
			else
				prevFrame = NULL;
			//printf("Adding frame: %s %c %d\n", thisStateDef.sprite, thisFrame->frame, thisFrame->duration);
			++thisFrame;
		}
	}

	// Safe guard to make sure state counting stays in sync
	assert(thisFrame == &frameList[frameList.Size()]);

	// Resolve Gotos
	for(unsigned int iter = 0;iter < gotos.Size();++iter)
	{
		const Frame *result = gotos[iter].jumpLabel.Resolve();
		if(gotos[iter].frame)
			gotos[iter].frame->next = result;
		else
		{
			unsigned int frameIndex = result->index;
			const ClassDef *owner = this;
			while(!owner->IsStateOwner(result))
			{
				frameIndex += owner->frameList.Size();
				owner = owner->parent;
			}
			stateList[gotos[iter].remapLabel] = frameIndex;
		}
	}
}

bool ClassDef::IsDescendantOf(const ClassDef *parent) const
{
	const ClassDef *currentParent = this;
	while(currentParent != NULL)
	{
		if(currentParent == parent)
			return true;
		currentParent = currentParent->parent;
	}
	return false;
}

void ClassDef::LoadActors()
{
	printf("ClassDef: Loading actor definitions.\n");
	atterm(&ClassDef::UnloadActors);

	// First iterate through the native classes and fix their parent pointers
	// In order to keep things simple what I did was in DeclareNativeClass I
	// force a const ClassDef ** into the parent, so we just need to cast back
	// and get the value of the pointer.
	{
		TMap<FName, ClassDef *>::Iterator iter(ClassTable());
		TMap<FName, ClassDef *>::Pair *pair;
		while(iter.NextPair(pair))
		{
			ClassDef * const cls = pair->Value;
			if(cls->parent)
				cls->parent = *(const ClassDef **)cls->parent;
		}
	}

	InitFunctionTable(NULL);

	// Add function symbols
	const ExpressionFunction *func = functions;
	do
	{
		globalSymbols.Push(new FunctionSymbol(func->name, TypeHierarchy::staticTypes.GetType(TypeHierarchy::PrimitiveTypes(func->ret)), func->args, func->function, func->takesRNG));
	}
	while((++func)->name != NULL);
	qsort(&globalSymbols[0], globalSymbols.Size(), sizeof(globalSymbols[0]), SymbolCompare);

	int lastLump = 0;
	int lump = 0;
	while((lump = Wads.FindLump("DECORATE", &lastLump)) != -1)
	{
		ParseDecorateLump(lump);
	}

	ReleaseFunctionTable();
	delete symbolPool;
#if 0
	// Debug code - Dump actor tree visually.
	DumpClasses();
#endif

	R_InitSprites();

	{
		unsigned int index = 0;

		TMap<FName, ClassDef *>::Iterator iter(ClassTable());
		TMap<FName, ClassDef *>::Pair *pair;
		while(iter.NextPair(pair))
		{
			ClassDef * const cls = pair->Value;

			if(cls->tentative)
			{
				FString error;
				error.Format("The actor '%s' is referenced but never defined.", cls->GetName().GetChars());
				throw CFatalError(error);
			}

			cls->ClassIndex = index++;
			for(unsigned int i = 0;i < cls->frameList.Size();++i)
				cls->frameList[i].spriteInf = R_GetSprite(cls->frameList[i].sprite);
		}
	}
}

void ClassDef::ParseActor(Scanner &sc)
{
	// Read the header
	sc.MustGetToken(TK_Identifier);
	ClassDef **classRef = ClassTable().CheckKey(sc->str);
	ClassDef *newClass;
	bool previouslyDefined = classRef != NULL;
	if(!previouslyDefined)
	{
		newClass = new ClassDef();
		ClassTable()[sc->str] = newClass;
	}
	else
		newClass = *classRef;
	bool native = false;
	newClass->name = sc->str;
	if(sc.CheckToken(':'))
	{
		sc.MustGetToken(TK_Identifier);
		const ClassDef *parent = FindClass(sc->str);
		if(parent == NULL || parent->tentative)
			sc.ScriptMessage(Scanner::ERROR, "Could not find parent actor '%s'", sc->str.GetChars());
		if(newClass->tentative && !parent->IsDescendantOf(newClass->parent))
			sc.ScriptMessage(Scanner::ERROR, "Parent for actor expected to be '%s'", newClass->parent->GetName().GetChars());
		newClass->parent = parent;
	}
	else
	{
		// If no class was specified to inherit from, inherit from AActor, but not for AActor.
		if(newClass != NATIVE_CLASS(Actor))
			newClass->parent = NATIVE_CLASS(Actor);
	}

	// Handle class replacements
	if(sc.CheckToken(TK_Identifier))
	{
		if(sc->str.CompareNoCase("replaces") == 0)
		{
			sc.MustGetToken(TK_Identifier);

			if(sc->str.CompareNoCase(newClass->name) == 0)
				sc.ScriptMessage(Scanner::ERROR, "Actor '%s' attempting to replace itself!", sc->str.GetChars());

			ClassDef *replacee = const_cast<ClassDef *>(FindClassTentative(sc->str, NATIVE_CLASS(Actor)));
			replacee->replacement = newClass;
			newClass->replacee = replacee;
		}
		else
			sc.Rewind();
	}

	if(sc.CheckToken(TK_IntConst))
	{
		if(EditorNumberTable.CheckKey(sc->number) != NULL)
			sc.ScriptMessage(Scanner::WARNING, "Overwriting editor number %d previously assigned to '%s', use replaces instead.", sc->number, EditorNumberTable[sc->number]->GetName().GetChars());
		EditorNumberTable[sc->number] = newClass;
	}
	if(sc.CheckToken(TK_Identifier))
	{
		if(sc->str.CompareNoCase("native") == 0)
			native = true;
		else
			sc.ScriptMessage(Scanner::ERROR, "Unknown keyword '%s'.", sc->str.GetChars());
	}
	if(previouslyDefined && !native && !newClass->tentative)
		sc.ScriptMessage(Scanner::ERROR, "Actor '%s' already defined.", newClass->name.GetChars());
	else
		newClass->tentative = false;

	if(!native) // Initialize the default instance to the nearest native class.
	{
		newClass->ConstructNative = newClass->parent->ConstructNative;
		newClass->size = newClass->parent->size;

		newClass->defaultInstance = (DObject *) M_Malloc(newClass->parent->size);
		memcpy((void*)newClass->defaultInstance, (void*)newClass->parent->defaultInstance, newClass->parent->size);
	}
	else
	{
		// This could happen if a non-native actor is declared native or
		// possibly in the case of a stuck dependency.
		if(!newClass->defaultInstance)
			sc.ScriptMessage(Scanner::ERROR, "Uninitialized default instance for '%s'.", newClass->GetName().GetChars());

		// Copy the parents defaults for native classes
		if(newClass->parent)
			memcpy((void*)newClass->defaultInstance, (void*)newClass->parent->defaultInstance, newClass->parent->size);
	}
	// Copy properties and flags.
	if(newClass->parent != NULL)
	{
		memcpy((void*)newClass->defaultInstance, (void*)newClass->parent->defaultInstance, newClass->parent->size);
		newClass->defaultInstance->Class = newClass;
		newClass->Meta = newClass->parent->Meta;
	}

	bool actionsSorted = true;
	sc.MustGetToken('{');
	while(!sc.CheckToken('}'))
	{
		if(sc.CheckToken('+') || sc.CheckToken('-'))
		{
			bool set = sc->token == '+';
			FString prefix;
			sc.MustGetToken(TK_Identifier);
			FString flagName = sc->str;
			if(sc.CheckToken('.'))
			{
				prefix = flagName;
				sc.MustGetToken(TK_Identifier);
				flagName = sc->str;
			}
			if(!SetFlag(newClass, (AActor*)newClass->defaultInstance, prefix, flagName, set))
				sc.ScriptMessage(Scanner::WARNING, "Unknown flag '%s' for actor '%s'.", flagName.GetChars(), newClass->name.GetChars());
		}
		else
		{
			sc.MustGetToken(TK_Identifier);
			if(sc->str.CompareNoCase("states") == 0)
			{
				if(!actionsSorted)
					InitFunctionTable(&newClass->actions);

				TArray<StateDefinition> stateDefs;

				sc.MustGetToken('{');
				//sc.MustGetToken(TK_Identifier); // We should already have grabbed the identifier in all other cases.
				bool needIdentifier = true;
				bool infiniteLoopProtection = false;
				while(sc->token != '}' && !sc.CheckToken('}'))
				{
					StateDefinition thisState;
					thisState.sprite[0] = thisState.sprite[4] = 0;
					thisState.duration = 0;
					thisState.randDuration = 0;
					thisState.nextType = StateDefinition::NORMAL;

					if(needIdentifier)
						sc.MustGetToken(TK_Identifier);
					else
						needIdentifier = true;
					FString stateString = sc->str;
					if(sc.CheckToken(':'))
					{
						infiniteLoopProtection = false;
						thisState.label = stateString;
						// New state
						if(sc.CheckToken('}'))
							sc.ScriptMessage(Scanner::ERROR, "State defined with no frames.");
						sc.MustGetToken(TK_Identifier);

						if(sc->str.CompareNoCase("stop") == 0)
						{
							thisState.nextType = StateDefinition::STOP;
							if(!sc.CheckToken('}'))
								sc.MustGetToken(TK_Identifier);
						}
						else if(sc->str.CompareNoCase("goto") == 0)
						{
							thisState.jumpLabel = StateLabel(sc, newClass, true);
							thisState.nextType = StateDefinition::GOTO;
							if(!sc.CheckToken('}'))
								sc.MustGetToken(TK_Identifier);
						}

						stateString = sc->str;
					}

					if(thisState.nextType == StateDefinition::NORMAL &&
						(sc.CheckToken(TK_Identifier) || sc.CheckToken(TK_StringConst)))
					{
						bool invalidSprite = (stateString.Len() != 4);
						strncpy(thisState.sprite, stateString, 4);

						infiniteLoopProtection = false;
						if(invalidSprite) // We now know this is a frame so check sprite length
							sc.ScriptMessage(Scanner::ERROR, "Sprite name must be exactly 4 characters long.");

						R_LoadSprite(thisState.sprite);
						thisState.frames = sc->str;
						if(sc.CheckToken('-'))
						{
							sc.MustGetToken(TK_FloatConst);
							thisState.duration = -1;
						}
						else
						{
							if(sc.CheckToken(TK_FloatConst))
							{
								// Eliminate confusion about fractional frame delays
								if(!CheckTicsValid(sc->decimal))
									sc.ScriptMessage(Scanner::ERROR, "Fractional frame durations must be exactly .5!");

								thisState.duration = static_cast<int> (sc->decimal*2);
							}
							else if(stricmp(thisState.sprite, "goto") == 0)
							{
								thisState.nextType = StateDefinition::GOTO;
								thisState.nextArg = thisState.frames;
								thisState.frames = FString();
							}
							else if(sc.CheckToken(TK_Identifier))
							{
								if(sc->str.CompareNoCase("random") != 0)
									sc.ScriptMessage(Scanner::ERROR, "Expected random frame duration.");

								sc.MustGetToken('(');
								sc.MustGetToken(TK_FloatConst);
								if(!CheckTicsValid(sc->decimal))
									sc.ScriptMessage(Scanner::ERROR, "Fractional frame durations must be exactly .5!");
								thisState.duration = static_cast<int> (sc->decimal*2);
								sc.MustGetToken(',');
								sc.MustGetToken(TK_FloatConst);
								if(!CheckTicsValid(sc->decimal))
									sc.ScriptMessage(Scanner::ERROR, "Fractional frame durations must be exactly .5!");
								thisState.randDuration = static_cast<int> (sc->decimal*2);
								sc.MustGetToken(')');
							}
							else
								sc.ScriptMessage(Scanner::ERROR, "Expected frame duration.");
						}
						thisState.functions[0].pointer = thisState.functions[1].pointer = NULL;
						thisState.fullbright = false;

						do
						{
							if(sc.CheckToken('}'))
								goto FinishState;
							else
								sc.MustGetToken(TK_Identifier);

							if(sc->str.CompareNoCase("bright") == 0)
								thisState.fullbright = true;
							else
								break;
						}
						while(true);

						if(thisState.nextType == StateDefinition::NORMAL)
						{
							for(int func = 0;func <= 2;func++)
							{
								if(sc.CheckToken(':'))
								{
									// We have a state label!
									needIdentifier = false;
									sc.Rewind();
									break;
								}
								if(sc->str.Len() == 4 || func == 2)
								{
									if(sc->str.CompareNoCase("goto") == 0)
									{
										thisState.jumpLabel = StateLabel(sc, newClass, true);
										thisState.nextType = StateDefinition::GOTO;
									}
									else if(sc->str.CompareNoCase("wait") == 0 || sc->str.CompareNoCase("fail") == 0)
									{
										thisState.nextType = StateDefinition::WAIT;
									}
									else if(sc->str.CompareNoCase("loop") == 0)
									{
										thisState.nextType = StateDefinition::LOOP;
									}
									else if(sc->str.CompareNoCase("stop") == 0)
									{
										thisState.nextType = StateDefinition::STOP;
									}
									else
										needIdentifier = false;
									break;
								}
								else
								{
									if(sc->str.CompareNoCase("NOP") != 0)
									{
										int specialNum = -1;
										const ActionInfo *funcInf = newClass->FindFunction(sc->str, specialNum);
										if(funcInf)
										{
											thisState.functions[func].pointer = *funcInf->func;

											CallArguments *&ca = thisState.functions[func].args;
											ca = new CallArguments();
											CallArguments::Value val;
											unsigned int argc = 0;

											// When using a line special we have to inject a parameter.
											if(specialNum >= 0)
											{
												val.useType = CallArguments::Value::VAL_INTEGER;
												val.isExpression = false;
												val.val.i = specialNum;
												ca->AddArgument(val);
												++argc;
											}
				
											if(sc.CheckToken('('))
											{
												if(funcInf->maxArgs == 0)
													sc.MustGetToken(')');
												else if(!(funcInf->minArgs == 0 && sc.CheckToken(')')))
												{
													do
													{
														val.isExpression = false;

														const Type *argType = funcInf->types[argc];
														if(argType == TypeHierarchy::staticTypes.GetType(TypeHierarchy::INT) ||
															argType == TypeHierarchy::staticTypes.GetType(TypeHierarchy::FLOAT) ||
															argType == TypeHierarchy::staticTypes.GetType(TypeHierarchy::BOOL))
														{
															val.isExpression = true;
															if(argType == TypeHierarchy::staticTypes.GetType(TypeHierarchy::INT))
																val.useType = CallArguments::Value::VAL_INTEGER;
															else
																val.useType = CallArguments::Value::VAL_DOUBLE;
															val.expr = ExpressionNode::ParseExpression(newClass, TypeHierarchy::staticTypes, sc);
														}
														else if(argType == TypeHierarchy::staticTypes.GetType(TypeHierarchy::STATE))
														{
															val.useType = CallArguments::Value::VAL_STATE;
															if(sc.CheckToken(TK_IntConst))
															{
																if(thisState.frames.Len() > 1)
																	sc.ScriptMessage(Scanner::ERROR, "State offsets not allowed on multistate definitions.");
																FString label;
																label.Format("%d", sc->number);
																val.label = StateLabel(label, newClass);
															}
															else
															{
																sc.MustGetToken(TK_StringConst);
																val.label = StateLabel(sc->str, newClass);
															}
														}
														else
														{
															sc.MustGetToken(TK_StringConst);
															val.useType = CallArguments::Value::VAL_STRING;
															val.str = sc->str;
														}
														ca->AddArgument(val);
														++argc;

														// Check if we can or should take another argument
														if(!funcInf->varArgs && argc >= funcInf->maxArgs)
															break;
													}
													while(sc.CheckToken(','));
													sc.MustGetToken(')');
												}
											}
											if(argc < funcInf->minArgs)
												sc.ScriptMessage(Scanner::ERROR, "Too few arguments.");
											else
											{
												// Push unused defaults.
												while(argc < funcInf->maxArgs)
													ca->AddArgument(funcInf->defaults[(argc++)-funcInf->minArgs]);
											}
										}
										else
											sc.ScriptMessage(Scanner::WARNING, "Could not find function %s.", sc->str.GetChars());
									}
								}

								if(!sc.CheckToken(TK_Identifier))
									break;
								else if(sc.CheckToken(':'))
								{
									needIdentifier = false;
									sc.Rewind();
									break;
								}
							}
						}
					}
					else
					{
						thisState.sprite[0] = 0;
						needIdentifier = false;
						if(infiniteLoopProtection)
							sc.ScriptMessage(Scanner::ERROR, "Malformed script.");
						infiniteLoopProtection = true;
					}
				FinishState:
					stateDefs.Push(thisState);
				}

				newClass->InstallStates(stateDefs);
			}
			else if(sc->str.CompareNoCase("action") == 0)
			{
				actionsSorted = false;
				sc.MustGetToken(TK_Identifier);
				if(sc->str.CompareNoCase("native") != 0)
					sc.ScriptMessage(Scanner::ERROR, "Custom actions not supported.");
				sc.MustGetToken(TK_Identifier);
				ActionInfo *funcInf = LookupFunction(sc->str, NULL);
				if(!funcInf)
					sc.ScriptMessage(Scanner::ERROR, "The specified function %s could not be located.", sc->str.GetChars());
				newClass->actions.Push(funcInf);
				sc.MustGetToken('(');
				if(!sc.CheckToken(')'))
				{
					bool optRequired = false;
					do
					{
						// If we have processed at least one argument, then we can take varArgs.
						if(funcInf->minArgs > 0 && sc.CheckToken(TK_Ellipsis))
						{
							funcInf->varArgs = true;
							break;
						}

						sc.MustGetToken(TK_Identifier);
						const Type *type = TypeHierarchy::staticTypes.GetType(sc->str);
						if(type == NULL)
							sc.ScriptMessage(Scanner::ERROR, "Unknown type %s.\n", sc->str.GetChars());
						funcInf->types.Push(type);

						if(sc->str.CompareNoCase("class") == 0)
						{
							sc.MustGetToken('<');
							sc.MustGetToken(TK_Identifier);
							sc.MustGetToken('>');
						}
						sc.MustGetToken(TK_Identifier);
						if(optRequired || sc.CheckToken('='))
						{
							if(optRequired)
								sc.MustGetToken('=');
							else
								optRequired = true;

							CallArguments::Value defVal;
							defVal.isExpression = false;

							if(type == TypeHierarchy::staticTypes.GetType(TypeHierarchy::INT) ||
								type == TypeHierarchy::staticTypes.GetType(TypeHierarchy::FLOAT)
							)
							{
								ExpressionNode *node = ExpressionNode::ParseExpression(newClass, TypeHierarchy::staticTypes, sc);
								const ExpressionNode::Value &val = node->Evaluate(NULL);
								if(type == TypeHierarchy::staticTypes.GetType(TypeHierarchy::INT))
								{
									defVal.useType = CallArguments::Value::VAL_INTEGER;
									defVal.val.i = val.GetInt();
								}
								else
								{
									defVal.useType = CallArguments::Value::VAL_DOUBLE;
									defVal.val.d = val.GetDouble();
								}
								delete node;
							}
							else if(type == TypeHierarchy::staticTypes.GetType(TypeHierarchy::BOOL))
							{
								sc.MustGetToken(TK_BoolConst);
								defVal.useType = CallArguments::Value::VAL_INTEGER;
								defVal.val.i = sc->boolean;
							}
							else if(type == TypeHierarchy::staticTypes.GetType(TypeHierarchy::STATE))
							{
								defVal.useType = CallArguments::Value::VAL_STATE;
								if(sc.CheckToken(TK_IntConst))
									sc.ScriptMessage(Scanner::ERROR, "State offsets not allowed for defaults.");
								else
								{
									sc.MustGetToken(TK_StringConst);
									defVal.label = StateLabel(sc->str, newClass);
								}
							}
							else
							{
								sc.MustGetToken(TK_StringConst);
								defVal.useType = CallArguments::Value::VAL_STRING;
								defVal.str = sc->str;
							}
							funcInf->defaults.Push(defVal);
						}
						else
							++funcInf->minArgs;
						++funcInf->maxArgs;
					}
					while(sc.CheckToken(','));
					sc.MustGetToken(')');
				}
				sc.MustGetToken(';');
			}
			else if(sc->str.CompareNoCase("native") == 0)
			{
				sc.MustGetToken(TK_Identifier);
				const Type *type = TypeHierarchy::staticTypes.GetType(sc->str);
				if(type == NULL)
					sc.ScriptMessage(Scanner::ERROR, "Unknown type %s.\n", sc->str.GetChars());
				sc.MustGetToken(TK_Identifier);
				FName varName(sc->str);
				const SymbolInfo *symInf = NULL;
				for(unsigned int i = 0;i < symbolPool->Size();++i)
				{
					// I think the symbol pool will be small enough to do a
					// linear search on.
					if((*symbolPool)[i]->cls == newClass && (*symbolPool)[i]->var == varName)
					{
						symInf = (*symbolPool)[i];
						break;
					}
				}
				if(symInf == NULL)
					sc.ScriptMessage(Scanner::ERROR, "Could not identify symbol %s::%s.\n", newClass->name.GetChars(), varName.GetChars());
				sc.MustGetToken(';');

				newClass->symbols.Push(new VariableSymbol(varName, type, symInf->offset));
			}
			else
			{
				FString className("actor");
				FString propertyName = sc->str;
				if(sc.CheckToken('.'))
				{
					className = propertyName;
					sc.MustGetToken(TK_Identifier);
					propertyName = sc->str;
				}

				if(!SetProperty(newClass, className, propertyName, sc))
				{
					do
					{
						sc.GetNextToken();
					}
					while(sc.CheckToken(','));
					sc.ScriptMessage(Scanner::WARNING, "Unknown property '%s' for actor '%s'.", propertyName.GetChars(), newClass->name.GetChars());
				}
			}
		}
	}

	// Now sort the symbol table.
	qsort(&newClass->symbols[0], newClass->symbols.Size(), sizeof(newClass->symbols[0]), SymbolCompare);
	if(!actionsSorted)
		InitFunctionTable(&newClass->actions);

	// Register conversation id into table if assigned
	if(int convid = newClass->Meta.GetMetaInt(AMETA_ConversationID))
		ConversationIDTable[convid] = newClass;
}

void ClassDef::ParseDecorateLump(int lumpNum)
{
	FWadLump lump = Wads.OpenLumpNum(lumpNum);
	char* data = new char[Wads.LumpLength(lumpNum)];
	lump.Read(data, Wads.LumpLength(lumpNum));
	Scanner sc(data, Wads.LumpLength(lumpNum));
	sc.SetScriptIdentifier(Wads.GetLumpFullName(lumpNum));

	while(sc.TokensLeft())
	{
		if(sc.CheckToken('#'))
		{
			sc.MustGetToken(TK_Identifier);
			if(sc->str.CompareNoCase("include") != 0)
				sc.ScriptMessage(Scanner::ERROR, "Expected 'include' got '%s' instead.", sc->str.GetChars());
			sc.MustGetToken(TK_StringConst);

			int lmp = Wads.CheckNumForFullName(sc->str, true);
			if(lmp == -1)
				sc.ScriptMessage(Scanner::ERROR, "Could not find lump \"%s\".", sc->str.GetChars());
			ParseDecorateLump(lmp);
			continue;
		}

		sc.MustGetToken(TK_Identifier);
		if(sc->str.CompareNoCase("actor") == 0)
		{
			ParseActor(sc);
		}
		else if(sc->str.CompareNoCase("const") == 0)
		{
			sc.MustGetToken(TK_Identifier);
			const Type *type = TypeHierarchy::staticTypes.GetType(sc->str);
			if(type == NULL)
				sc.ScriptMessage(Scanner::ERROR, "Unknown type %s.\n", sc->str.GetChars());
			sc.MustGetToken(TK_Identifier);
			FName constName(sc->str);
			sc.MustGetToken('=');
			ExpressionNode *expr = ExpressionNode::ParseExpression(NATIVE_CLASS(Actor), TypeHierarchy::staticTypes, sc);
			ConstantSymbol *newSym = new ConstantSymbol(constName, type, expr->Evaluate(NULL));
			delete expr;
			sc.MustGetToken(';');

			// We must insert the constant into the table at the proper place
			// now since the next const may try to reference it.
			if(globalSymbols.Size() > 0)
			{
				unsigned int min = 0;
				unsigned int max = globalSymbols.Size()-1;
				unsigned int mid = max/2;
				if(max > 0)
				{
					do
					{
						if(globalSymbols[mid]->GetName() > constName)
							max = mid-1;
						else if(globalSymbols[mid]->GetName() < constName)
							min = mid+1;
						else
							break;
						mid = (min+max)/2;
					}
					while(max >= min && max < globalSymbols.Size());
				}
				if(globalSymbols[mid]->GetName() <= constName)
					++mid;
				globalSymbols.Insert(mid, newSym);
			}
			else
				globalSymbols.Push(newSym);
		}
		else
			sc.ScriptMessage(Scanner::ERROR, "Unknown thing section '%s'.", sc->str.GetChars());
	}
	delete[] data;
}

const Frame *ClassDef::ResolveStateIndex(unsigned int index) const
{
	if(index == INT_MAX) // Deleted state (Label: stop)
		return NULL;
	if(index > frameList.Size() && parent)
		return parent->ResolveStateIndex(index - frameList.Size());
	return &frameList[index];
}

bool ClassDef::SetFlag(const ClassDef *newClass, AActor *instance, const FString &prefix, const FString &flagName, bool set)
{
	int min = 0;
	int max = sizeof(flags)/sizeof(FlagDef) - 1;
	while(min <= max)
	{
		int mid = (min+max)/2;
		int ret = flagName.CompareNoCase(flags[mid].name);
		if(ret == 0 && !prefix.IsEmpty())
			ret = prefix.CompareNoCase(flags[mid].prefix);

		if(ret == 0)
		{
			if(!newClass->IsDescendantOf(flags[mid].cls))
				return false;

			if(set)
				*reinterpret_cast<flagstype_t *>((int8_t*)instance + flags[mid].varOffset) |= flags[mid].value;
			else
				*reinterpret_cast<flagstype_t *>((int8_t*)instance + flags[mid].varOffset) &= ~flags[mid].value;
			return true;
		}
		else if(ret < 0)
			max = mid-1;
		else
			min = mid+1;
	}
	return false;
}

bool ClassDef::SetProperty(ClassDef *newClass, const char* className, const char* propName, Scanner &sc)
{
	static unsigned int NUM_PROPERTIES = 0;
	if(NUM_PROPERTIES == 0)
	{
		// Calculate NUM_PROPERTIES if needed.
		while(properties[NUM_PROPERTIES++].name != NULL)
			;
	}

	int min = 0;
	int max = NUM_PROPERTIES - 1;
	while(min <= max)
	{
		int mid = (min+max)/2;
		int ret = stricmp(properties[mid].name, propName);
		if(ret == 0)
		{
			if(!newClass->IsDescendantOf(properties[mid].className) ||
				stricmp(properties[mid].prefix, className) != 0)
				sc.ScriptMessage(Scanner::ERROR, "Property %s.%s not available in this scope.\n", properties[mid].className->name.GetChars(), propName);

			PropertyParam* params = new PropertyParam[strlen(properties[mid].params)];
			// Key:
			//   K - Keyword (Identifier)
			//   I - Integer
			//   F - Float
			//   S - String
			bool optional = false;
			bool done = false;
			const char* p = properties[mid].params;
			unsigned int paramc = 0;
			if(*p != 0)
			{
				do
				{
					if(*p != 0)
					{
						while(*p == '_') // Optional
						{
							optional = true;
							p++;
						}

						bool negate = false;
						params[paramc].i = 0; // Try to default to 0

						switch(*p)
						{
							case 'K':
								if(!optional)
									sc.MustGetToken(TK_Identifier);
								else if(!sc.CheckToken(TK_Identifier))
								{
									done = true;
									break;
								}
								params[paramc].s = new char[sc->str.Len()+1];
								strcpy(params[paramc].s, sc->str);
								break;
							default:
							case 'I':
								if(sc.CheckToken('('))
								{
									params[paramc].isExpression = true;
									params[paramc].expr = ExpressionNode::ParseExpression(newClass, TypeHierarchy::staticTypes, sc, NULL);
									sc.MustGetToken(')');
									break;
								}
								else
									params[paramc].isExpression = false;

								if(sc.CheckToken('-'))
									negate = true;

								if(!optional) // Float also includes integers
									sc.MustGetToken(TK_FloatConst);
								else if(!sc.CheckToken(TK_FloatConst))
								{
									done = true;
									break;
								}
								params[paramc].i = (negate ? -1 : 1) * static_cast<int64_t> (sc->decimal);
								break;
							case 'F':
								if(sc.CheckToken('-'))
									negate = true;

								if(!optional)
									sc.MustGetToken(TK_FloatConst);
								else if(!sc.CheckToken(TK_FloatConst))
								{
									done = true;
									break;
								}
								params[paramc].f = (negate ? -1 : 1) * sc->decimal;
								break;
							case 'S':
								if(!optional)
									sc.MustGetToken(TK_StringConst);
								else if(!sc.CheckToken(TK_StringConst))
								{
									done = true;
									break;
								}
								params[paramc].s = new char[sc->str.Len()+1];
								strcpy(params[paramc].s, sc->str);
								break;
						}
						paramc++;
						p++;
					}
					else
						sc.GetNextToken();
				}
				while(sc.CheckToken(','));
			}
			if(!optional && *p != 0 && *p != '_')
				sc.ScriptMessage(Scanner::ERROR, "Not enough parameters.");

			properties[mid].handler(newClass, (AActor*)newClass->defaultInstance, paramc, params);

			// Clean up
			p = properties[mid].params;
			unsigned int i = 0;
			do
			{
				if(*p == 'S' || *p == 'K')
					delete[] params[i].s;
				i++;
			}
			while(*(++p) != 0 && i < paramc);
			delete[] params;
			return true;
		}
		else if(ret > 0)
			max = mid-1;
		else
			min = mid+1;
	}
	return false;
}

void ClassDef::UnloadActors()
{
	TMap<FName, ClassDef *>::Pair *pair;

	// Clean up the frames in case any expressions use the symbols
	for(TMap<FName, ClassDef *>::Iterator iter(ClassTable());iter.NextPair(pair);)
			pair->Value->frameList.Clear();

	// Also contains code from ZDoom

	bShutdown = true;

	TArray<size_t *> uniqueFPs;
	for(TMap<FName, ClassDef *>::Iterator iter(ClassTable());iter.NextPair(pair);)
	{
		ClassDef *type = pair->Value;
		if (type->FlatPointers != &POINTER_END && type->FlatPointers != type->Pointers)
		{
			// FlatPointers are shared by many classes, so we must check for
			// duplicates and only delete those that are unique.
			unsigned int j;
			for (j = 0; j < uniqueFPs.Size(); ++j)
			{
				if (type->FlatPointers == uniqueFPs[j])
				{
					break;
				}
			}
			if (j == uniqueFPs.Size())
			{
				uniqueFPs.Push(const_cast<size_t *>(type->FlatPointers));
			}
		}
		type->FlatPointers = NULL;

		delete type;
	}
	for (unsigned int i = 0; i < uniqueFPs.Size(); ++i)
	{
		delete[] uniqueFPs[i];
	}

	// Also clear globals
	// but first clear the damage expression table since it relies on some of the symbols.
	AActor::damageExpressions.Clear();
	for(unsigned int i = 0;i < globalSymbols.Size();++i)
		delete globalSymbols[i];
}

////////////////////////////////////////////////////////////////////////////////

void ClassDef::DumpClasses()
{
	struct ClassTree
	{
		public:
			ClassTree(const ClassDef *classType) : child(NULL), next(NULL), thisClass(classType)
			{
				ClassTree **nextChild = &child;
				TMap<FName, ClassDef *>::Pair *pair;
				for(TMap<FName, ClassDef *>::Iterator iter(ClassTable());iter.NextPair(pair);)
				{
					if(pair->Value->parent == classType)
					{
						*nextChild = new ClassTree(pair->Value);
						nextChild = &(*nextChild)->next;
					}
				}
			}

			~ClassTree()
			{
				if(child != NULL)
					delete child;
				if(next != NULL)
					delete next;
			}

			void Dump(int spacing)
			{
				for(int i = spacing;i > 0;--i)
					printf("  ");
				printf("%s\n", thisClass->name.GetChars());
				if(child != NULL)
					child->Dump(spacing+1);
				if(next != NULL)
					next->Dump(spacing);
			}

			ClassTree		*child;
			ClassTree		*next;
			const ClassDef	*thisClass;
	};

	ClassTree root(FindClass("Actor"));
	root.Dump(0);
}

////////////////////////////////////////////////////////////////////////////////

CallArguments::~CallArguments()
{
	for(unsigned int i = 0;i < args.Size();++i)
	{
		if(args[i].isExpression)
			delete args[i].expr;
	}
}

void CallArguments::AddArgument(const CallArguments::Value &val)
{
	args.Push(val);
}

void CallArguments::Evaluate(AActor *self)
{
	for(unsigned int i = 0;i < args.Size();++i)
	{
		if(args[i].isExpression)
		{
			const ExpressionNode::Value &val = args[i].expr->Evaluate(self);
			if(args[i].useType == Value::VAL_INTEGER)
				args[i].val.i = val.GetInt();
			else
				args[i].val.d = val.GetDouble();
		}
	}
}

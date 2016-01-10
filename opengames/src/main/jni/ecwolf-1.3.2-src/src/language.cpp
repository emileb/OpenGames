/*
** language.cpp
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

#include "language.h"
#include "w_wad.h"
#include "scanner.h"

Language language;

void Language::SetupStrings(const char* language)
{
	int lastLump = 0;
	int lump = 0;
	while((lump = Wads.FindLump("LANGUAGE", &lastLump)) != -1)
	{
		// Read the lump defaulting to english
		ReadLump(lump, language == NULL ? "enu" : language);
	}
}

void Language::ReadLump(int lump, const char* language)
{
	FMemLump wadLump = Wads.ReadLump(lump);
	Scanner sc((const char*)(wadLump.GetMem()), wadLump.GetSize());
	sc.SetScriptIdentifier(Wads.GetLumpFullName(lump));

	int token = TK_NoToken;
	bool skip = false;
	bool noReplace = false;
	while(sc.GetNextToken())
	{
		token = sc->token;
		if(token == '[')
		{
			// match with language
			sc.MustGetToken(TK_Identifier);
			if(sc->str.Compare(language) != 0)
				skip = true;
			else
			{
				skip = false;
				noReplace = false;
			}

			if(sc.CheckToken(TK_Identifier))
			{
				if(sc->str.Compare("default") == 0)
				{
					// if not the correct language, go in no replace mode.
					if(skip)
					{
						skip = false;
						noReplace = true;
					}
				}
				else
				{
					printf("Unexpected identifier '%s'\n", sc->str.GetChars());
					exit(0);
				}
			}

			sc.MustGetToken(']');
		}
		else if(token == TK_Identifier)
		{
			FName index = sc->str;
			sc.MustGetToken('=');
			sc.MustGetToken(TK_StringConst);
			if(!skip)
			{
				if(!noReplace || (noReplace && strings.CheckKey(index) == NULL))
					strings[index] = sc->str;
			}
			sc.MustGetToken(';');
		}
		else
		{
			sc.ScriptMessage(Scanner::ERROR, "Unexpected token.\n");
			exit(0);
		}
	}
}

const char* Language::operator[] (const char* index) const
{
	const FString *it = strings.CheckKey(index);
	if(it != NULL)
		return it->GetChars();
	return index;
}

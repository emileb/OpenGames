// WL_AGENT.C

#include <cmath>
#include <climits>

#include "wl_def.h"
#include "id_ca.h"
#include "id_sd.h"
#include "id_vl.h"
#include "id_vh.h"
#include "id_us.h"
#include "actor.h"
#include "thingdef/thingdef.h"
#include "lnspec.h"
#include "wl_agent.h"
#include "a_inventory.h"
#include "a_keys.h"
#include "m_random.h"
#include "g_mapinfo.h"
#include "thinker.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_loadsave.h"
#include "wl_state.h"
#include "wl_play.h"
#include "templates.h"

#include "w_wad.h"
#include "scanner.h"

/*
=============================================================================

								LOCAL CONSTANTS

=============================================================================
*/

#define MAXMOUSETURN    10


#define MOVESCALE       150l
#define ANGLESCALE      20

/*
=============================================================================

								GLOBAL VARIABLES

=============================================================================
*/



//
// player state info
//
int32_t			thrustspeed;

AActor			*LastAttacker;

player_t		players[1];

void ClipMove (AActor *ob, int32_t xmove, int32_t ymove);

/*
=============================================================================

								GLOBAL VARIABLES

=============================================================================
*/

DBaseStatusBar *StatusBar;

DBaseStatusBar *CreateStatusBar_Wolf3D();

void DestroyStatusBar() { delete StatusBar; }
void CreateStatusBar()
{
	StatusBar = CreateStatusBar_Wolf3D();
	atterm(DestroyStatusBar);
}

/*
=============================================================================

								CONTROL STUFF

=============================================================================
*/

/*
======================
=
= CheckWeaponChange
=
= Keys 1-4 change weapons
=
======================
*/

void CheckWeaponChange (void)
{
	if(players[0].flags & player_t::PF_DISABLESWITCH)
		return;

	AWeapon *newWeapon = NULL;

	if(buttonstate[bt_nextweapon] && !buttonheld[bt_nextweapon])
	{
		newWeapon = players[0].weapons.PickNextWeapon(&players[0]);
		buttonheld[bt_nextweapon] = true;
	}
	else if(buttonstate[bt_prevweapon] && !buttonheld[bt_prevweapon])
	{
		newWeapon = players[0].weapons.PickPrevWeapon(&players[0]);
		buttonheld[bt_prevweapon] = true;
	}
	else
	{
		for(int i = 0;i <= 9;++i)
		{
			if(buttonstate[bt_slot0 + i] && !buttonheld[bt_slot0 + i])
			{
				newWeapon = players[0].weapons.Slots[i].PickWeapon(&players[0]);
				buttonheld[bt_slot0 + i] = true;
				break;
			}
		}
	}

	if(newWeapon && newWeapon != players[0].ReadyWeapon)
		players[0].PendingWeapon = newWeapon;
}


/*
=======================
=
= ControlMovement
=
= Takes controlx,controly, and buttonstate[bt_strafe]
=
= Changes the players[0].mo's angle and position
=
= There is an angle hack because when going 70 fps, the roundoff becomes
= significant
=
=======================
*/

void ControlMovement (AActor *ob)
{
	if(playstate == ex_died)
		return;

	int32_t oldx,oldy;
	angle_t angle;
	int strafe = controlstrafe;

	thrustspeed = 0;

	oldx = players[0].mo->x;
	oldy = players[0].mo->y;

	//
	// side to side move
	//
	if (buttonstate[bt_strafe])
	{
		//
		// strafing
		//
		//
		strafe += controlx;
	}
	else
	{
		if(players[0].ReadyWeapon && players[0].ReadyWeapon->fovscale > 0)
			controlx = xs_ToInt(controlx*players[0].ReadyWeapon->fovscale);

		//
		// not strafing
		//
		ob->angle -= controlx*(ANGLE_1/ANGLESCALE);
	}

	if(strafe)
	{
		// Cap the speed
		if (strafe > 100)
			strafe = 100;
		else if (strafe < -100)
			strafe = -100;

		strafe = FixedMul(players[0].mo->speed<<7, FixedMul(strafe, players[0].mo->sidemove[abs(strafe) >= RUNMOVE]));

		if (strafe > 0)
		{
			angle = ob->angle - ANGLE_90;
			Thrust (angle,strafe*MOVESCALE);      // move to left
		}
		else if (strafe < 0)
		{
			angle = ob->angle + ANGLE_90;
			Thrust (angle,-strafe*MOVESCALE);     // move to right
		}
	}

	//
	// forward/backwards move
	//
	if (controly < 0)
	{
		if(controly < -100)
			controly = -100;

		controly = FixedMul(players[0].mo->speed<<7, FixedMul(controly, players[0].mo->forwardmove[controly <= -RUNMOVE]));

		Thrust (ob->angle,-controly*MOVESCALE); // move forwards
	}
	else if (controly > 0)
	{
		if(controly > 100)
			controly = 100;

		controly = FixedMul(players[0].mo->speed<<7, FixedMul(controly, players[0].mo->forwardmove[controly >= RUNMOVE]));

		angle = ob->angle + ANGLE_180;
		Thrust (angle,controly*MOVESCALE*2/3);          // move backwards
	}

	if (gamestate.victoryflag)              // watching the BJ actor
		return;
}

/*
===============
=
= GiveExtraMan
=
===============
*/

void GiveExtraMan (int amount)
{
	players[0].lives += amount;
	if (players[0].lives < 0)
		players[0].lives = 0;
	else if(players[0].lives > 9)
		players[0].lives = 9;
	SD_PlaySound ("misc/1up");
}

/*
===============
=
= GivePoints
=
===============
*/

void GivePoints (int32_t points)
{
	players[0].score += points;
	while (players[0].score >= players[0].nextextra)
	{
		players[0].nextextra += EXTRAPOINTS;
		GiveExtraMan (1);
	}
}

/*
===============
=
= TakeDamage
=
===============
*/

void TakeDamage (int points,AActor *attacker)
{
	LastAttacker = attacker;

	if (gamestate.victoryflag)
		return;
	points = (points*gamestate.difficulty->DamageFactor)>>FRACBITS;

	if (!godmode)
		players[0].mo->health = players[0].health -= points;

	if (players[0].health<=0)
	{
		players[0].mo->Die();
		players[0].health = 0;
		playstate = ex_died;
		players[0].killerobj = attacker;
	}

	if (godmode != 2)
		StartDamageFlash (points);

	if (points > 0)
		SD_PlaySound("player/pain");

	StatusBar->UpdateFace(points);
	StatusBar->DrawStatusBar();
}

/*
=============================================================================

								MOVEMENT

=============================================================================
*/

/*
===================
=
= TryMove
=
= returns true if move ok
= debug: use pointers to optimize
===================
*/

static bool TryMove (AActor *ob)
{
	if (noclip)
	{
		return (ob->x-ob->radius >= 0 && ob->y-ob->radius >= 0
			&& ob->x+ob->radius < (((int32_t)(map->GetHeader().width))<<TILESHIFT)
			&& ob->y+ob->radius < (((int32_t)(map->GetHeader().height))<<TILESHIFT) );
	}

	int xl,yl,xh,yh,x,y;

	xl = (ob->x-ob->radius) >>TILESHIFT;
	yl = (ob->y-ob->radius) >>TILESHIFT;

	xh = (ob->x+ob->radius) >>TILESHIFT;
	yh = (ob->y+ob->radius) >>TILESHIFT;

	//
	// check for solid walls
	//
	for (y=yl;y<=yh;y++)
	{
		for (x=xl;x<=xh;x++)
		{
			const bool checkLines[4] =
			{
				(ob->x+ob->radius) > ((x+1)<<TILESHIFT),
				(ob->y-ob->radius) < (y<<TILESHIFT),
				(ob->x-ob->radius) < (x<<TILESHIFT),
				(ob->y+ob->radius) > ((y+1)<<TILESHIFT)
			};
			MapSpot spot = map->GetSpot(x, y, 0);
			if(spot->tile)
			{
				// Check pushwall backs
				if(spot->pushAmount != 0)
				{
					switch(spot->pushDirection)
					{
						case MapTile::North:
							if(ob->y-ob->radius <= static_cast<fixed>((y<<TILESHIFT)+((63-spot->pushAmount)<<10)))
								return false;
							break;
						case MapTile::West:
							if(ob->x-ob->radius <= static_cast<fixed>((x<<TILESHIFT)+((63-spot->pushAmount)<<10)))
								return false;
							break;
						case MapTile::East:
							if(ob->x+ob->radius >= static_cast<fixed>((x<<TILESHIFT)+(spot->pushAmount<<10)))
								return false;
							break;
						case MapTile::South:
							if(ob->y+ob->radius >= static_cast<fixed>((y<<TILESHIFT)+(spot->pushAmount<<10)))
								return false;
							break;
					}
				}
				else
				{
					for(unsigned short i = 0;i < 4;++i)
					{
						if(spot->sideSolid[i] && spot->slideAmount[i] != 0xffff && checkLines[i])
							return false;
					}
				}
			}
		}
	}

	//
	// check for actors
	//
	for(AActor::Iterator iter = AActor::GetIterator().Next();iter;)
	{
		// We need to iterate a little awkwardly since the object may disappear
		// on us rendering the next pointer invalid.
		AActor *check = iter;
		iter.Next();

		if(check == ob)
			continue;

		fixed r = check->radius + ob->radius;
		if(check->flags & FL_SOLID)
		{
			if(abs(ob->x - check->x) > r ||
				abs(ob->y - check->y) > r)
				continue;
			return false;
		}
		else
		{
			if(abs(ob->x - check->x) <= r &&
				abs(ob->y - check->y) <= r)
				check->Touch(ob);
		}
	}

	return true;
}

static void ExecuteWalkTriggers(MapSpot spot, MapTrigger::Side dir)
{
	if(!spot)
		return;

	for(unsigned int i = spot->triggers.Size();i-- > 0;)
	{
		MapTrigger &trigger = spot->triggers[i];
		if(trigger.playerCross && trigger.activate[dir])
			map->ActivateTrigger(trigger, dir, players[0].mo);
	}
}

static void CheckWalkTriggers(AActor *ob, int32_t xmove, int32_t ymove)
{
	MapSpot spot;

	if(ob->fracx <= abs(xmove) || ob->fracx >= 0xFFFF-abs(xmove))
	{
		spot = map->GetSpot((ob->x-xmove)>>FRACBITS, ob->y>>FRACBITS, 0);
		if(xmove > 0)
			ExecuteWalkTriggers(spot->GetAdjacent(MapTile::East), MapTrigger::West);
		else if(xmove < 0)
			ExecuteWalkTriggers(spot->GetAdjacent(MapTile::West), MapTrigger::East);
	}

	if(ob->fracy <= abs(ymove) || ob->fracy >= 0xFFFF-abs(ymove))
	{
		spot = map->GetSpot(ob->x>>FRACBITS, (ob->y-ymove)>>FRACBITS, 0);
		if(ymove > 0)
			ExecuteWalkTriggers(spot->GetAdjacent(MapTile::South), MapTrigger::North);
		else if(ymove < 0)
			ExecuteWalkTriggers(spot->GetAdjacent(MapTile::North), MapTrigger::South);
	}
}


/*
===================
=
= ClipMove
=
===================
*/

void ClipMove (AActor *ob, int32_t xmove, int32_t ymove)
{
	fixed basex = ob->x;
	fixed basey = ob->y;

	ob->x = basex+xmove;
	ob->y = basey+ymove;

	if (TryMove (ob))
	{
		CheckWalkTriggers(ob, xmove, ymove);
		return;
	}

	if (!SD_SoundPlaying())
		SD_PlaySound ("world/hitwall");

	ob->x = basex+xmove;
	ob->y = basey;
	if (TryMove (ob))
	{
		CheckWalkTriggers(ob, xmove, 0);
		return;
	}

	ob->x = basex;
	ob->y = basey+ymove;
	if (TryMove (ob))
	{
		CheckWalkTriggers(ob, 0, ymove);
		return;
	}

	ob->x = basex;
	ob->y = basey;
}

//==========================================================================

/*
===================
=
= Thrust
=
===================
*/

void Thrust (angle_t angle, int32_t speed)
{
	static const int MAXTHRUST = 0x5800l * 2;
	int32_t xmove,ymove;

	//
	// ZERO FUNNY COUNTER IF MOVED!
	//
	if (speed)
		funnyticount = 0;

	thrustspeed += speed;
	//
	// moving bounds speed
	//
	if (speed >= MAXTHRUST)
		speed = MAXTHRUST-1;

	xmove = FixedMul(speed,finecosine[angle>>ANGLETOFINESHIFT]);
	ymove = -FixedMul(speed,finesine[angle>>ANGLETOFINESHIFT]);

	ClipMove(players[0].mo,xmove,ymove);

	players[0].mo->EnterZone(map->GetSpot(players[0].mo->tilex, players[0].mo->tiley, 0)->zone);
}


/*
=============================================================================

								ACTIONS

=============================================================================
*/

//===========================================================================

/*
===============
=
= Cmd_Use
=
===============
*/

void Cmd_Use (void)
{
	int     checkx,checky;
	MapTrigger::Side direction;

	//
	// find which cardinal direction the player is facing
	//
	if (players[0].mo->angle < ANGLE_45 || players[0].mo->angle > 7*ANGLE_45)
	{
		checkx = players[0].mo->tilex + 1;
		checky = players[0].mo->tiley;
		direction = MapTrigger::West;
	}
	else if (players[0].mo->angle < 3*ANGLE_45)
	{
		checkx = players[0].mo->tilex;
		checky = players[0].mo->tiley-1;
		direction = MapTrigger::South;
	}
	else if (players[0].mo->angle < 5*ANGLE_45)
	{
		checkx = players[0].mo->tilex - 1;
		checky = players[0].mo->tiley;
		direction = MapTrigger::East;
	}
	else
	{
		checkx = players[0].mo->tilex;
		checky = players[0].mo->tiley + 1;
		direction = MapTrigger::North;
	}

	bool doNothing = true;
	bool isRepeatable = false;
	BYTE lastTrigger = 0;
	MapSpot spot = map->GetSpot(checkx, checky, 0);
	for(unsigned int i = 0;i < spot->triggers.Size();++i)
	{
		MapTrigger &trig = spot->triggers[i];
		if(trig.activate[direction] && trig.playerUse)
		{
			if(map->ActivateTrigger(trig, direction, players[0].mo))
			{
				isRepeatable |= trig.repeatable;
				lastTrigger = trig.action;
				doNothing = false;
			}
		}
	}

	if(doNothing)
		SD_PlaySound ("misc/do_nothing");
	else
		P_ChangeSwitchTexture(spot, static_cast<MapTile::Side>(direction), isRepeatable, lastTrigger);
}

/*
=============================================================================

								PLAYER CONTROL

=============================================================================
*/

player_t::player_t() : FOV(90), DesiredFOV(90), bob(0), attackheld(false)
{
}

// P_BobWeapon From ZDoom
//============================================================================
//
// P_BobWeapon
//
// [RH] Moved this out of A_WeaponReady so that the weapon can bob every
// tic and not just when A_WeaponReady is called. Not all weapons execute
// A_WeaponReady every tic, and it looks bad if they don't bob smoothly.
//
// [XA] Added new bob styles and exposed bob properties. Thanks, Ryan Cordell!
//
//============================================================================

void player_t::BobWeapon (fixed *x, fixed *y)
{
	AWeapon *weapon;

	weapon = ReadyWeapon;

	if (weapon == NULL || weapon->weaponFlags & WF_DONTBOB)
	{
		*x = *y = 0;
		return;
	}

	// [XA] Get the current weapon's bob properties.
	int bobstyle = weapon->BobStyle;
	int bobspeed = (weapon->BobSpeed * 128) >> 16;
	fixed rangex = weapon->BobRangeX;
	fixed rangey = weapon->BobRangeY;

	// Bob the weapon based on movement speed.
	int angle = (bobspeed*35/TICRATE*gamestate.TimeCount)&FINEMASK;
	fixed curbob = (flags & PF_WEAPONBOBBING) ? bob : 0;

	if (curbob != 0)
	{
		fixed_t bobx = FixedMul(curbob, rangex);
		fixed_t boby = FixedMul(curbob, rangey);
		switch (bobstyle)
		{
		case AWeapon::BobNormal:
			*x = FixedMul(bobx, finecosine[angle]);
			*y = FixedMul(boby, finesine[angle & (FINEANGLES/2-1)]);
			break;
			
		case AWeapon::BobInverse:
			*x = FixedMul(bobx, finecosine[angle]);
			*y = boby - FixedMul(boby, finesine[angle & (FINEANGLES/2-1)]);
			break;
			
		case AWeapon::BobAlpha:
			*x = FixedMul(bobx, finesine[angle]);
			*y = FixedMul(boby, finesine[angle & (FINEANGLES/2-1)]);
			break;
			
		case AWeapon::BobInverseAlpha:
			*x = FixedMul(bobx, finesine[angle]);
			*y = boby - FixedMul(boby, finesine[angle & (FINEANGLES/2-1)]);
			break;
			
		case AWeapon::BobSmooth:
			*x = FixedMul(bobx, finecosine[angle]);
			*y = (boby - FixedMul(boby, finecosine[angle*2 & (FINEANGLES-1)])) / 2;
			break;

		case AWeapon::BobInverseSmooth:
			*x = FixedMul(bobx, finecosine[angle]);
			*y = (FixedMul(boby, finecosine[angle*2 & (FINEANGLES-1)]) + boby) / 2;
			break;

		case AWeapon::BobThrust:
			{
				*x = 0;

				// Down thrust is faster than up
				// Blake Stone uses a linearly increasing velocity,
				// we use a sin table since it's available and requires no extra storage
				const int thrustPosition = (((angle<<3)*3)&(FRACUNIT-1)) * 3;
				if(thrustPosition < FRACUNIT*2)
					*y = -FixedMul(boby, thrustPosition - finesine[(thrustPosition/2)>>5] - FRACUNIT/2);
				else
					*y = FixedMul(boby, finesine[(thrustPosition - FRACUNIT*2)>>5] - FRACUNIT/2);
			}
			break;
		}
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

const fixed RAISERANGE = 96*FRACUNIT;
const fixed RAISESPEED = FRACUNIT*6;

void player_t::BringUpWeapon()
{
	if(PendingWeapon == WP_NOCHANGE)
	{
		SetPSprite(ReadyWeapon ? ReadyWeapon->GetReadyState() : NULL, player_t::ps_weapon);
		return;
	}

	psprite[player_t::ps_weapon].sy = RAISERANGE;
	psprite[player_t::ps_weapon].sx = 0;

	ReadyWeapon = PendingWeapon;
	PendingWeapon = WP_NOCHANGE;
	SetPSprite(ReadyWeapon ? ReadyWeapon->GetUpState() : NULL, player_t::ps_weapon);
}
ACTION_FUNCTION(A_Lower)
{
	player_t *player = self->player;

	player->psprite[player_t::ps_weapon].sy += RAISESPEED;
	if(player->psprite[player_t::ps_weapon].sy < RAISERANGE)
		return false;
	player->psprite[player_t::ps_weapon].sy = RAISERANGE;

	if(player->PendingWeapon == WP_NOCHANGE)
		player->PendingWeapon = NULL;

	player->SetPSprite(NULL, player_t::ps_flash);
	// If we're dead, don't bother trying to raise a weapon.
	// In fact, we want to keep the current weapon "up" so that the status bar
	// displays the correct information.
	if(player->state != player_t::PST_DEAD)
		player->BringUpWeapon();
	else
		player->SetPSprite(NULL, player_t::ps_weapon);
	return true;
}
ACTION_FUNCTION(A_Raise)
{
	player_t *player = self->player;

	if(player->PendingWeapon != WP_NOCHANGE)
	{
		player->SetPSprite(player->ReadyWeapon->GetDownState(), player_t::ps_weapon);
		return false;
	}

	player->psprite[player_t::ps_weapon].sy -= RAISESPEED;
	if(player->psprite[player_t::ps_weapon].sy > 0)
		return false;
	player->psprite[player_t::ps_weapon].sy = 0;

	if(player->ReadyWeapon)
		player->SetPSprite(player->ReadyWeapon->GetReadyState(), player_t::ps_weapon);
	else
		player->psprite[player_t::ps_weapon].frame = NULL;
	return true;
}

// Finds the target closest to the player within shooting range.
AActor *player_t::FindTarget()
{
	//
	// find potential targets
	//

	int32_t viewdist = 0x7fffffffl;
	AActor *closest = NULL, *oldclosest = NULL;

	while (1)
	{
		oldclosest = closest;

		for(AActor::Iterator check = AActor::GetIterator();check.Next();)
		{
			if(check == mo)
				continue;

			if ((check->flags & FL_SHOOTABLE) && (check->flags & FL_VISABLE)
				&& abs(check->viewx-centerx) < shootdelta)
			{
				if (check->transx < viewdist)
				{
					viewdist = check->transx;
					closest = check;
				}
			}
		}

		if (closest == oldclosest)
			return NULL; // no more targets, all missed

		//
		// trace a line from player to enemey
		//
		if (CheckLine(closest))
			break;
	}

	return closest;
}

size_t player_t::PropagateMark()
{
	GC::Mark(mo);
	GC::Mark(camera);
	GC::Mark(ReadyWeapon);
	if(PendingWeapon != WP_NOCHANGE)
		GC::Mark(PendingWeapon);
	return sizeof(*this);
}

void player_t::Reborn()
{
	ReadyWeapon = NULL;
	PendingWeapon = WP_NOCHANGE;
	flags = 0;
	FOV = DesiredFOV = 90.0f;

	if(state == PST_ENTER)
	{
		lives = 3;
		score = oldscore = 0;
		nextextra = EXTRAPOINTS;
	}

	mo->GiveStartingInventory();
	health = mo->maxhealth;

	// Recalculate the projection here so that player classes with differing radii are supported.
	CalcProjection(mo->radius);
}

void player_t::Serialize(FArchive &arc)
{
	BYTE state = this->state;
	arc << state;
	this->state = static_cast<State>(state);

	arc << mo
		<< camera
		<< killerobj
		<< oldscore
		<< score
		<< nextextra
		<< lives
		<< health
		<< ReadyWeapon
		<< PendingWeapon
		<< flags
		<< extralight;

	for(unsigned int i = 0;i < NUM_PSPRITES;++i)
	{
		arc << psprite[i].frame
			<< psprite[i].ticcount
			<< psprite[i].sx
			<< psprite[i].sy;
	}

	if(GameSave::SaveProdVersion >= 0x001002FF && GameSave::SaveVersion > 1374729160)
		arc << FOV << DesiredFOV;

	if(arc.IsLoading())
	{
		mo->SetupWeaponSlots();
		CalcProjection(mo->radius);
	}
}

void player_t::SetPSprite(const Frame *frame, player_t::PSprite layer)
{
	flags &= ~(player_t::PF_READYFLAGS);
	psprite[layer].frame = frame;

	while(psprite[layer].frame)
	{
		psprite[layer].ticcount = psprite[layer].frame->GetTics();
		psprite[layer].frame->action(mo, ReadyWeapon, psprite[layer].frame);

		if(psprite[layer].frame && psprite[layer].ticcount == 0)
			psprite[layer].frame = psprite[layer].frame->next;
		else
			break;
	}
}

FArchive &operator<< (FArchive &arc, player_t *&player)
{
	return arc.SerializePointer(players, (BYTE**)&player, sizeof(players[0]));
}

/*
===============
=
= SpawnPlayer
=
===============
*/

void SpawnPlayer (int tilex, int tiley, int dir)
{
	players[0].mo = (APlayerPawn *) AActor::Spawn(gamestate.playerClass, ((int32_t)tilex<<TILESHIFT)+TILEGLOBAL/2, ((int32_t)tiley<<TILESHIFT)+TILEGLOBAL/2, 0, 0);
	players[0].mo->angle = dir*ANGLE_1;
	players[0].mo->player = &players[0];
	Thrust (0,0); // set some variables
	players[0].mo->SetPriority(ThinkerList::PLAYER);

	if(players[0].state == player_t::PST_ENTER || players[0].state == player_t::PST_REBORN)
		players[0].Reborn();

	players[0].camera = players[0].mo;
	players[0].state = player_t::PST_LIVE;
	players[0].extralight = 0;

	// Re-raise the weapon like Doom if we don't have the flag set in mapinfo.
	if(!levelInfo->SpawnWithWeaponRaised && players[0].PendingWeapon == WP_NOCHANGE)
		players[0].PendingWeapon = players[0].ReadyWeapon;
	players[0].BringUpWeapon();
}


//===========================================================================

/*
===============
=
= T_KnifeAttack
=
= Update player hands, and try to do damage when the proper frame is reached
=
===============
*/

static FRandom pr_cwpunch("CustomWpPunch");
ACTION_FUNCTION(A_CustomPunch)
{
	enum
	{
		CPF_USEAMMO = 1,
		CPF_ALWAYSPLAYSOUND = 2
	};

	ACTION_PARAM_INT(damage, 0);
	ACTION_PARAM_BOOL(norandom, 1);
	ACTION_PARAM_INT(flags, 2);
	ACTION_PARAM_STRING(pufftype, 3);
	ACTION_PARAM_DOUBLE(range, 4);
	ACTION_PARAM_FIXED(lifesteal, 5);

	player_t *player = self->player;

	if(flags & CPF_ALWAYSPLAYSOUND)
		SD_PlaySound(player->ReadyWeapon->attacksound, SD_WEAPONS);
	if(range == 0)
		range = 64;

	if(!(players[0].ReadyWeapon->weaponFlags & WF_NOALERT))
		madenoise = true;

	// actually fire
	int dist = 0x7fffffff;
	AActor *closest = NULL;
	for(AActor::Iterator check = AActor::GetIterator();check.Next();)
	{
		if(check == self)
			continue;

		if ( (check->flags & FL_SHOOTABLE) && (check->flags & FL_VISABLE)
			&& abs(check->viewx-centerx) < shootdelta)
		{
			if (check->transx < dist)
			{
				dist = check->transx;
				closest = check;
			}
		}
	}

	if (!closest || dist-(FRACUNIT/2) > (range/64)*FRACUNIT)
	{
		// missed
		return false;
	}

	if(!norandom)
		damage *= pr_cwpunch()%8 + 1;

	// hit something
	if(!(flags & CPF_ALWAYSPLAYSOUND))
		SD_PlaySound(player->ReadyWeapon->attacksound, SD_WEAPONS);
	DamageActor(closest, damage);

	// Ammo is only used when hit
	if(flags & CPF_USEAMMO)
	{
		if(!player->ReadyWeapon->DepleteAmmo())
			return true;
	}

	if(lifesteal > 0 && player->health < self->health)
	{
		damage *= lifesteal;
		player->health += damage;
		if(player->health > self->health)
			player->health = self->health;
	}
	return true;
}

static FRandom pr_cwbullet("CustomWpBullet");
ACTION_FUNCTION(A_GunAttack)
{
	enum
	{
		GAF_NORANDOM = 1,
		GAF_NOAMMO = 2,
		GAF_MACDAMAGE = 4
	};

	player_t *player = self->player;
	int      dx,dy,dist;

	ACTION_PARAM_INT(flags, 0);
	ACTION_PARAM_STRING(sound, 1);
	ACTION_PARAM_FIXED(snipe, 2);
	ACTION_PARAM_INT(maxdamage, 3);
	ACTION_PARAM_INT(blocksize, 4);
	ACTION_PARAM_INT(pointblank, 5);
	ACTION_PARAM_INT(longrange, 6);
	ACTION_PARAM_INT(maxrange, 7);

	if(!(flags & GAF_NOAMMO))
	{
		if(!player->ReadyWeapon->DepleteAmmo())
			return false;
	}

	if(sound.Len() == 1 && sound[0] == '*')
		SD_PlaySound(player->ReadyWeapon->attacksound, SD_WEAPONS);
	else
		SD_PlaySound(sound, SD_WEAPONS);

	if(!(players[0].ReadyWeapon->weaponFlags & WF_NOALERT))
		madenoise = true;

	AActor *closest = players[0].FindTarget();
	if(!closest)
		return false;

	//
	// hit something
	//
	dx = abs(closest->x - players[0].mo->x);
	dy = abs(closest->y - players[0].mo->y);
	dist = dx>dy ? dx:dy;

	dist = FixedMul(dist, snipe);
	dist /= blocksize<<9;

	int damage = flags & GAF_NORANDOM ? maxdamage : (1 + (pr_cwbullet()%maxdamage));
	if (dist >= pointblank)
		damage = (flags & GAF_MACDAMAGE) ? damage >> 1 : damage * 2 / 3;
	if (dist >= longrange)
	{
		if ( (pr_cwbullet() % maxrange) < dist)           // missed
			return false;
	}
	DamageActor (closest,damage);
	return true;
}

ACTION_FUNCTION(A_FireCustomMissile)
{
	ACTION_PARAM_STRING(missiletype, 0);
	ACTION_PARAM_DOUBLE(angleOffset, 1);
	ACTION_PARAM_BOOL(useammo, 2);
	ACTION_PARAM_INT(spawnoffset, 3);
	ACTION_PARAM_INT(spawnheight, 4);
	ACTION_PARAM_BOOL(aim, 5);

	if(useammo && !players[0].ReadyWeapon->DepleteAmmo())
		return false;

	if(!(players[0].ReadyWeapon->weaponFlags & WF_NOALERT))
		madenoise = true;

	fixed newx = self->x + spawnoffset*finesine[self->angle>>ANGLETOFINESHIFT]/64;
	fixed newy = self->y + spawnoffset*finecosine[self->angle>>ANGLETOFINESHIFT]/64;

	angle_t iangle = self->angle + (angle_t) ((angleOffset*ANGLE_45)/45);

	const ClassDef *cls = ClassDef::FindClass(missiletype);
	if(!cls)
		return false;
	AActor *newobj = AActor::Spawn(cls, newx, newy, 0, SPAWN_AllowReplacement);
	newobj->flags |= FL_PLAYERMISSILE;
	newobj->angle = iangle;

	newobj->velx = FixedMul(newobj->speed,finecosine[iangle>>ANGLETOFINESHIFT]);
	newobj->vely = -FixedMul(newobj->speed,finesine[iangle>>ANGLETOFINESHIFT]);
	return true;
}

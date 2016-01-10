// WL_STATE.C

#include "wl_def.h"
#include "id_ca.h"
#include "id_sd.h"
#include "id_us.h"
#include "g_mapinfo.h"
#include "m_random.h"
#include "actor.h"
#include "thingdef/thingdef.h"
#include "wl_agent.h"
#include "wl_game.h"
#include "wl_play.h"
#include "templates.h"

/*
=============================================================================

							LOCAL CONSTANTS

=============================================================================
*/


/*
=============================================================================

							GLOBAL VARIABLES

=============================================================================
*/


static const dirtype opposite[9] =
	{west,southwest,south,southeast,east,northeast,north,northwest,nodir};

static const dirtype diagonal[9][9] =
{
	/* east */  {nodir,nodir,northeast,nodir,nodir,nodir,southeast,nodir,nodir},
				{nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
	/* north */ {northeast,nodir,nodir,nodir,northwest,nodir,nodir,nodir,nodir},
				{nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
	/* west */  {nodir,nodir,northwest,nodir,nodir,nodir,southwest,nodir,nodir},
				{nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
	/* south */ {southeast,nodir,nodir,nodir,southwest,nodir,nodir,nodir,nodir},
				{nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
				{nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir}
};

bool TryWalk (AActor *ob);
bool MoveObj (AActor *ob, int32_t move);

void    FirstSighting (AActor *ob);

/*
=============================================================================

								LOCAL VARIABLES

=============================================================================
*/


/*
=============================================================================

						ENEMY TILE WORLD MOVEMENT CODE

=============================================================================
*/


/*
==================================
=
= TryWalk
=
= Attempts to move ob in its current (ob->dir) direction.
=
= If blocked by either a wall or an actor returns FALSE
=
= If move is either clear or blocked only by a door, returns TRUE and sets
=
= ob->tilex         = new destination
= ob->tiley
= ob->distance      = TILEGLOBAl, or -doornumber if a door is blocking the way
=
= If a door is in the way, an OpenDoor call is made to start it opening.
= The actor code should wait until the door has been fully opened
=
==================================
*/

// Returns 1 - Wait for Door, 0 - Blocked, -1 - Continue checks
static inline short CheckSide(AActor *ob, unsigned int x, unsigned int y, MapTrigger::Side dir, bool canuse)
{
	MapSpot spot = map->GetSpot(x, y, 0);
	if(spot->tile)
	{
		if(canuse)
		{
			bool used = false;
			for(unsigned int i = 0;i < spot->triggers.Size();++i)
			{
				if(spot->triggers[i].monsterUse && spot->triggers[i].activate[dir])
				{
					if(map->ActivateTrigger(spot->triggers[i], dir, ob))
						used = true;
				}
			}
			if(used)
			{
				ob->distance = -1;
				return 1;
			}
		}
		if(spot->slideAmount[dir] != 0xffff)
			return 0;
	}
	for(AActor::Iterator iter = AActor::GetIterator();iter.Next();)
	{
		// We want to check where the actor is heading instead of the exact
		// tile it exists in since this is essentially how Wolf3D handled things
		// We must first determine if the monster has moved into the destination
		// tile or not.  (Half way to destination.)

		const dirtype offsetDir = iter->distance >= TILEGLOBAL/2 ? iter->dir : nodir;

		// Players need not be checked
		if(iter != ob && !iter->player && (iter->flags & FL_SOLID) &&
			static_cast<unsigned int>(iter->tilex+dirdeltax[offsetDir]) == x &&
			static_cast<unsigned int>(iter->tiley+dirdeltay[offsetDir]) == y)
			return 0;
	}
	return -1;
}
#define CHECKSIDE(x,y,dir) \
{ \
	short _cs; \
	if((_cs = CheckSide(ob, x, y, dir, !!(ob->flags & FL_CANUSEWALLS))) >= 0) \
		return _cs != 0; \
}
#define CHECKDIAG(x,y,dir) \
{ \
	short _cs; \
	if((_cs = CheckSide(ob, x, y, dir, false)) >= 0) \
		return _cs != 0; \
}



bool TryWalk (AActor *ob)
{
	word zonex = ob->tilex;
	word zoney = ob->tiley;

	switch (ob->dir)
	{
		case north:
			CHECKSIDE(ob->tilex,ob->tiley-1,MapTrigger::South);
			zoney--;
			break;

		case northeast:
			CHECKDIAG(ob->tilex+1,ob->tiley-1,MapTrigger::South);
			CHECKDIAG(ob->tilex+1,ob->tiley,MapTrigger::West);
			CHECKDIAG(ob->tilex,ob->tiley-1,MapTrigger::South);
			zonex++;
			zoney--;
			break;

		case east:
			CHECKSIDE(ob->tilex+1,ob->tiley,MapTrigger::West);
			zonex++;
			break;

		case southeast:
			CHECKDIAG(ob->tilex+1,ob->tiley+1,MapTrigger::North);
			CHECKDIAG(ob->tilex+1,ob->tiley,MapTrigger::West);
			CHECKDIAG(ob->tilex,ob->tiley+1,MapTrigger::North);
			zonex++;
			zoney++;
			break;

		case south:
			CHECKSIDE(ob->tilex,ob->tiley+1,MapTrigger::North);
			zoney++;
			break;

		case southwest:
			CHECKDIAG(ob->tilex-1,ob->tiley+1,MapTrigger::North);
			CHECKDIAG(ob->tilex-1,ob->tiley,MapTrigger::East);
			CHECKDIAG(ob->tilex,ob->tiley+1,MapTrigger::North);
			zonex--;
			zoney++;
			break;

		case west:
			CHECKSIDE(ob->tilex-1,ob->tiley,MapTrigger::East);
			zonex--;
			break;

		case northwest:
			CHECKDIAG(ob->tilex-1,ob->tiley-1,MapTrigger::South);
			CHECKDIAG(ob->tilex-1,ob->tiley,MapTrigger::East);
			CHECKDIAG(ob->tilex,ob->tiley-1,MapTrigger::South);
			zonex--;
			zoney--;
			break;

		case nodir:
			return false;

		default:
			Printf ("Walk: Bad dir");
			assert(ob->dir <= nodir);
	}

	ob->EnterZone(map->GetSpot(zonex, zoney, 0)->zone);

	ob->distance = TILEGLOBAL;
	return true;
}


/*
==================================
=
= SelectDodgeDir
=
= Attempts to choose and initiate a movement for ob that sends it towards
= the players[0].mo while dodging
=
= If there is no possible move (ob is totally surrounded)
=
= ob->dir           =       nodir
=
= Otherwise
=
= ob->dir           = new direction to follow
= ob->distance      = TILEGLOBAL or -doornumber
= ob->tilex         = new destination
= ob->tiley
=
==================================
*/

static FRandom pr_newchasedir("NewChaseDir");
void SelectDodgeDir (AActor *ob)
{
	int         deltax,deltay,i;
	unsigned    absdx,absdy;
	dirtype     dirtry[5];
	dirtype     turnaround,tdir;

	if (ob->flags & FL_FIRSTATTACK)
	{
		//
		// turning around is only ok the very first time after noticing the
		// player
		//
		turnaround = nodir;
		ob->flags &= ~FL_FIRSTATTACK;
	}
	else
		turnaround=opposite[ob->dir];

	deltax = players[0].mo->tilex - ob->tilex;
	deltay = players[0].mo->tiley - ob->tiley;

	//
	// arange 5 direction choices in order of preference
	// the four cardinal directions plus the diagonal straight towards
	// the player
	//

	if (deltax>0)
	{
		dirtry[1]= east;
		dirtry[3]= west;
	}
	else
	{
		dirtry[1]= west;
		dirtry[3]= east;
	}

	if (deltay>0)
	{
		dirtry[2]= south;
		dirtry[4]= north;
	}
	else
	{
		dirtry[2]= north;
		dirtry[4]= south;
	}

	//
	// randomize a bit for dodging
	//
	absdx = abs(deltax);
	absdy = abs(deltay);

	if (absdx > absdy)
	{
		tdir = dirtry[1];
		dirtry[1] = dirtry[2];
		dirtry[2] = tdir;
		tdir = dirtry[3];
		dirtry[3] = dirtry[4];
		dirtry[4] = tdir;
	}

	if (pr_newchasedir() < 128)
	{
		tdir = dirtry[1];
		dirtry[1] = dirtry[2];
		dirtry[2] = tdir;
		tdir = dirtry[3];
		dirtry[3] = dirtry[4];
		dirtry[4] = tdir;
	}

	dirtry[0] = diagonal [ dirtry[1] ] [ dirtry[2] ];

	//
	// try the directions util one works
	//
	for (i=0;i<5;i++)
	{
		if ( dirtry[i] == nodir || dirtry[i] == turnaround)
			continue;

		ob->dir = dirtry[i];
		if (TryWalk(ob))
			return;
	}

	//
	// turn around only as a last resort
	//
	if (turnaround != nodir)
	{
		ob->dir = turnaround;

		if (TryWalk(ob))
			return;
	}

	ob->dir = nodir;
}


/*
============================
=
= SelectChaseDir
=
= As SelectDodgeDir, but doesn't try to dodge
=
============================
*/

void SelectChaseDir (AActor *ob)
{
	int     deltax,deltay;
	dirtype d[3];
	dirtype tdir, olddir, turnaround;


	olddir=ob->dir;
	turnaround=opposite[olddir];

	deltax=players[0].mo->tilex - ob->tilex;
	deltay=players[0].mo->tiley - ob->tiley;

	d[1]=nodir;
	d[2]=nodir;

	if (deltax>0)
		d[1]= east;
	else if (deltax<0)
		d[1]= west;
	if (deltay>0)
		d[2]=south;
	else if (deltay<0)
		d[2]=north;

	if (abs(deltay)>abs(deltax))
	{
		tdir=d[1];
		d[1]=d[2];
		d[2]=tdir;
	}

	if (d[1]==turnaround)
		d[1]=nodir;
	if (d[2]==turnaround)
		d[2]=nodir;


	if (d[1]!=nodir)
	{
		ob->dir=d[1];
		if (TryWalk(ob))
			return;     /*either moved forward or attacked*/
	}

	if (d[2]!=nodir)
	{
		ob->dir=d[2];
		if (TryWalk(ob))
			return;
	}

	/* there is no direct path to the player, so pick another direction */

	if (olddir!=nodir)
	{
		ob->dir=olddir;
		if (TryWalk(ob))
			return;
	}

	if (pr_newchasedir()>128)      /*randomly determine direction of search*/
	{
		for (tdir=north; tdir<=west; tdir=(dirtype)(tdir+1))
		{
			if (tdir!=turnaround)
			{
				ob->dir=tdir;
				if ( TryWalk(ob) )
					return;
			}
		}
	}
	else
	{
		for (tdir=west; tdir>=north; tdir=(dirtype)(tdir-1))
		{
			if (tdir!=turnaround)
			{
				ob->dir=tdir;
				if ( TryWalk(ob) )
					return;
			}
		}
	}

	if (turnaround !=  nodir)
	{
		ob->dir=turnaround;
		if (ob->dir != nodir)
		{
			if ( TryWalk(ob) )
				return;
		}
	}

	ob->dir = nodir;                // can't move
}


/*
============================
=
= SelectRunDir
=
= Run Away from player
=
============================
*/

void SelectRunDir (AActor *ob)
{
	int deltax,deltay;
	dirtype d[3];
	dirtype tdir;


	deltax=players[0].mo->tilex - ob->tilex;
	deltay=players[0].mo->tiley - ob->tiley;

	if (deltax<0)
		d[1]= east;
	else
		d[1]= west;
	if (deltay<0)
		d[2]=south;
	else
		d[2]=north;

	if (abs(deltay)>abs(deltax))
	{
		tdir=d[1];
		d[1]=d[2];
		d[2]=tdir;
	}

	ob->dir=d[1];
	if (TryWalk(ob))
		return;     /*either moved forward or attacked*/

	ob->dir=d[2];
	if (TryWalk(ob))
		return;

	/* there is no direct path to the player, so pick another direction */

	if (pr_newchasedir()>128)      /*randomly determine direction of search*/
	{
		for (tdir=north; tdir<=west; tdir=(dirtype)(tdir+1))
		{
			ob->dir=tdir;
			if ( TryWalk(ob) )
				return;
		}
	}
	else
	{
		for (tdir=west; tdir>=north; tdir=(dirtype)(tdir-1))
		{
			ob->dir=tdir;
			if ( TryWalk(ob) )
				return;
		}
	}

	ob->dir = nodir;                // can't move
}


/*
=================
=
= MoveObj
=
= Moves ob be move global units in ob->dir direction
= Actors are not allowed to move inside the player
= Does NOT check to see if the move is tile map valid
=
= ob->x                 = adjusted for new position
= ob->y
=
=================
*/

bool MoveObj (AActor *ob, int32_t move)
{
	switch (ob->dir)
	{
		case north:
			ob->y -= move;
			break;
		case northeast:
			ob->x += move;
			ob->y -= move;
			break;
		case east:
			ob->x += move;
			break;
		case southeast:
			ob->x += move;
			ob->y += move;
			break;
		case south:
			ob->y += move;
			break;
		case southwest:
			ob->x -= move;
			ob->y += move;
			break;
		case west:
			ob->x -= move;
			break;
		case northwest:
			ob->x -= move;
			ob->y -= move;
			break;

		case nodir:
			return true;

		default:
			Printf ("MoveObj: bad dir!");
			assert(ob->dir <= nodir);
	}

	//
	// check to make sure it's not on top of player
	//
	if (map->CheckLink(ob->GetZone(), players[0].mo->GetZone(), true))
	{
		fixed r = ob->radius + players[0].mo->radius;
		if (abs(ob->x - players[0].mo->x) > r || abs(ob->y - players[0].mo->y) > r)
			goto moveok;

		if (ob->GetClass()->Meta.GetMetaInt(AMETA_Damage) >= 0)
			TakeDamage (ob->GetDamage(), ob);

		//
		// back up
		//
		switch (ob->dir)
		{
			case north:
				ob->y += move;
				break;
			case northeast:
				ob->x -= move;
				ob->y += move;
				break;
			case east:
				ob->x -= move;
				break;
			case southeast:
				ob->x -= move;
				ob->y -= move;
				break;
			case south:
				ob->y -= move;
				break;
			case southwest:
				ob->x += move;
				ob->y -= move;
				break;
			case west:
				ob->x += move;
				break;
			case northwest:
				ob->x += move;
				ob->y += move;
				break;

			case nodir:
				return false;
		}
		return false;
	}
moveok:
	ob->distance -=move;

	// Check for touching objects
	for(AActor::Iterator iter = AActor::GetIterator();iter.Next();)
	{
		AActor *check = iter;
		if(check == ob || (check->flags & FL_SOLID))
			continue;

		fixed r = check->radius + ob->radius;
		if(abs(ob->x - check->x) <= r &&
			abs(ob->y - check->y) <= r)
			check->Touch(ob);
	}

	return true;
}

/*
=============================================================================

								STUFF

=============================================================================
*/


/*
===================
=
= DamageActor
=
= Called when the player succesfully hits an enemy.
=
= Does damage points to enemy ob, either putting it into a stun frame or
= killing it.
=
===================
*/

static FRandom pr_damagemobj("ActorTakeDamage");
void DamageActor (AActor *ob, unsigned damage)
{
	madenoise = true;

	//
	// do double damage if shooting a non attack mode actor
	//
	if ( !(ob->flags & FL_ATTACKMODE) )
		damage <<= 1;

	ob->health -= FixedMul(damage, gamestate.difficulty->PlayerDamageFactor);

	if (ob->health<=0)
	{
		ob->killerx = players[0].mo->x;
		ob->killery = players[0].mo->y;
		ob->Die();
	}
	else
	{
		if (! (ob->flags & FL_ATTACKMODE) )
			FirstSighting (ob);             // put into combat mode

		if(ob->PainState && pr_damagemobj() < ob->painchance)
			ob->SetState(ob->PainState);
	}
}

/*
=============================================================================

								CHECKSIGHT

=============================================================================
*/

bool CheckSlidePass(unsigned int style, unsigned int intercept, unsigned int amount)
{
	if(!amount)
		return false;

	switch(style)
	{
		default:
			return intercept < amount;
		case SLIDE_Split:
			return (unsigned int)abs((int)(FRACUNIT - intercept*2)) < amount;
		case SLIDE_Invert:
			return intercept>(FRACUNIT-amount);
	}
}


/*
=====================
=
= CheckLine
=
= Returns true if a straight line between the player and ob is unobstructed
=
=====================
*/

bool CheckLine (AActor *ob)
{
	int         x1,y1,xt1,yt1,x2,y2,xt2,yt2;
	int         x,y;
	int         xdist,ydist,xstep,ystep;
	int         partial,delta;
	int32_t     ltemp;
	int         xfrac,yfrac,deltafrac;
	unsigned    intercept;
	MapTile::Side	direction;

	x1 = ob->x >> UNSIGNEDSHIFT;            // 1/256 tile precision
	y1 = ob->y >> UNSIGNEDSHIFT;
	xt1 = x1 >> 8;
	yt1 = y1 >> 8;

	x2 = players[0].mo->x >> UNSIGNEDSHIFT;
	y2 = players[0].mo->y >> UNSIGNEDSHIFT;
	xt2 = players[0].mo->tilex;
	yt2 = players[0].mo->tiley;

	xdist = abs(xt2-xt1);

	if (xdist > 0)
	{
		if (xt2 > xt1)
		{
			partial = 256-(x1&0xff);
			xstep = 1;
			direction = MapTile::East;
		}
		else
		{
			partial = x1&0xff;
			xstep = -1;
			direction = MapTile::West;
		}

		deltafrac = abs(x2-x1);
		delta = y2-y1;
		ltemp = ((int32_t)delta<<8)/deltafrac;
		if (ltemp > 0x7fffl)
			ystep = 0x7fff;
		else if (ltemp < -0x7fffl)
			ystep = -0x7fff;
		else
			ystep = ltemp;
		yfrac = y1 + (((int32_t)ystep*partial) >>8);

		x = xt1+xstep;
		xt2 += xstep;
		do
		{
			y = yfrac>>8;
			yfrac += ystep;

			MapSpot spot = map->GetSpot(x, y, 0);
			x += xstep;

			if (!spot->tile)
				continue;
			if (spot->tile && spot->slideAmount[direction] == 0)
				return false;

			//
			// see if the door is open enough
			//
			intercept = yfrac-ystep/2;

			if (!CheckSlidePass(spot->slideStyle, intercept, spot->slideAmount[direction]))
				return false;

		} while (x != xt2);
	}

	ydist = abs(yt2-yt1);

	if (ydist > 0)
	{
		if (yt2 > yt1)
		{
			partial = 256-(y1&0xff);
			ystep = 1;
			direction = MapTile::South;
		}
		else
		{
			partial = y1&0xff;
			ystep = -1;
			direction = MapTile::North;
		}

		deltafrac = abs(y2-y1);
		delta = x2-x1;
		ltemp = ((int32_t)delta<<8)/deltafrac;
		if (ltemp > 0x7fffl)
			xstep = 0x7fff;
		else if (ltemp < -0x7fffl)
			xstep = -0x7fff;
		else
			xstep = ltemp;
		xfrac = x1 + (((int32_t)xstep*partial) >>8);

		y = yt1 + ystep;
		yt2 += ystep;
		do
		{
			x = xfrac>>8;
			xfrac += xstep;

			MapSpot spot = map->GetSpot(x, y, 0);
			y += ystep;

			if (!spot->tile)
				continue;
			if (spot->tile && spot->slideAmount[direction] == 0)
				return false;

			//
			// see if the door is open enough
			//
			intercept = xfrac-xstep/2;

			if (intercept>spot->slideAmount[direction])
				return false;
		} while (y != yt2);
	}

	return true;
}


/*
================
=
= CheckSight
=
= Checks a straight line between player and current object
=
= If the sight is ok, check alertness and angle to see if they notice
=
= returns true if the player has been spoted
=
================
*/

#define MINSIGHT (0x18000l*64)

static bool CheckSight (AActor *ob, double minseedist, double maxseedist, double maxheardist, double fov)
{
	bool heardnoise = madenoise;

	// Check if we can hear the player's noise
	if (heardnoise && !map->CheckLink(ob->GetZone(), players[0].mo->GetZone(), true))
		heardnoise = false;

	//
	// if the players[0].mo is real close, sight is automatic
	//
	int32_t deltax = players[0].mo->x - ob->x;
	int32_t deltay = players[0].mo->y - ob->y;
	uint32_t distance = MAX(abs(deltax), abs(deltay))*64;

	if (!(ob->flags & FL_AMBUSH) && heardnoise &&
		(maxheardist < 0.00001 ||
		distance < maxheardist))
		return true;

	if (minseedist > 0.00001 &&
		distance < minseedist)
		return false;
	if (maxseedist > 0.00001 &&
		distance > maxseedist)
		return false;

	if (distance < MINSIGHT)
		return true;

	if(fov < 359.75)
	{
		//
		// see if they are looking in the right direction
		//
		fov /= 2;
		float angle = (float) atan2 ((float) deltay, (float) deltax);
		if (angle<0)
			angle = (float) (M_PI*2+angle);
		angle_t iangle = 0-(angle_t)(angle*ANGLE_180/M_PI);
		angle_t lowerAngle = MIN(iangle, ob->angle);
		angle_t upperAngle = MAX(iangle, ob->angle);
		if(MIN(upperAngle - lowerAngle, lowerAngle - upperAngle) > angle_t(fov*ANGLE_1))
			return false;
	}

	//
	// trace a line to check for blocking tiles (corners)
	//
	return CheckLine (ob);
}


/*
===============
=
= FirstSighting
=
= Puts an actor into attack mode and possibly reverses the direction
= if the player is behind it
=
===============
*/

void FirstSighting (AActor *ob)
{
	PlaySoundLocActor(ob->seesound, ob);
	ob->speed = ob->runspeed;

	if (ob->distance < 0)
		ob->distance = 0;       // ignore the door opening command

	ob->flags &= ~FL_PATHING;
	ob->flags |= FL_ATTACKMODE|FL_FIRSTATTACK;

	if(ob->SeeState)
		ob->SetState(ob->SeeState);
}



/*
===============
=
= SightPlayer
=
= Called by actors that ARE NOT chasing the players[0].  If the player
= is detected (by sight, noise, or proximity), the actor is put into
= it's combat frame and true is returned.
=
= Incorporates a random reaction delay
=
===============
*/

static FRandom pr_sight("SightPlayer");
bool SightPlayer (AActor *ob, double minseedist, double maxseedist, double maxheardist, double fov)
{
	if (notargetmode)
		return false;

	if (ob->flags & FL_ATTACKMODE)
	{
		ob->sighttime = ob->GetDefault()->sighttime;
		ob->flags &= ~FL_ATTACKMODE;
	}

	if (ob->sighttime != ob->GetDefault()->sighttime)
	{
		//
		// count down reaction time
		//
		if (ob->sightrandom)
		{
			--ob->sightrandom;
			return false;
		}

		if (ob->sighttime > 0)
		{
			--ob->sighttime;
			return false;
		}
	}
	else
	{
		if (!CheckSight (ob, minseedist, maxseedist, maxheardist, fov))
			return false;
		ob->flags &= ~FL_AMBUSH;

		--ob->sighttime; // We need to somehow mark we started.
		ob->sightrandom = 1; // Account for tic.
		if(ob->GetDefault()->sightrandom)
			ob->sightrandom += pr_sight()/ob->GetDefault()->sightrandom;
		return false;
	}

	FirstSighting (ob);

	return true;
}

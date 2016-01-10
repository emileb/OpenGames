
#include "g_local.h"
#include "m_player.h"

#include "cr_main.h"

#if defined(WIN32) || defined(_WIN32)
#define PATHSEPERATOR_CHAR     '\\'
#define PATHSEPERATOR_STR      "\\"
#else                       
#define PATHSEPERATOR_CHAR     '/'
#define PATHSEPERATOR_STR      "/"
#endif

#if defined(WIN32) || defined(_WIN32)
#include <direct.h>
#else                       
#include <sys/stat.h>
#endif

#define sqr(x) ((x)*(x))

#undef DotProduct
#undef VectorSubtract
#undef VectorAdd
#undef VectorCopy
#undef VectorClear
#undef VectorNegate

__inline float DotProduct( vec3_t x, vec3_t y) { return (x[0]*y[0]+x[1]*y[1]+x[2]*y[2]); }
__inline void VectorSubtract( vec3_t a, vec3_t b, vec3_t c) { c[0]=a[0]-b[0]; c[1]=a[1]-b[1]; c[2]=a[2]-b[2]; }
__inline void VectorAdd( vec3_t a, vec3_t b, vec3_t c) { c[0]=a[0]+b[0]; c[1]=a[1]+b[1]; c[2]=a[2]+b[2]; }
__inline void VectorCopy( vec3_t a, vec3_t b) { b[0]=a[0]; b[1]=a[1]; b[2]=a[2]; }
__inline void VectorClear( vec3_t a) { a[0]=a[1]=a[2]=0; }
__inline void VectorNegate(vec3_t a, vec3_t b) { b[0]=-a[0]; b[1]=-a[1]; b[2]=-a[2]; }

__inline qboolean bVectorZero( vec3_t a ) { return fabs(a[0])<0.05f && fabs(a[1])<0.05f && fabs(a[2])<0.05f; }

void YawVector( float yaw, vec3_t dir )
{
    float angle;
    angle = yaw*( M_PI*2 / 360 );
    dir[0] = cos(angle);
    dir[1] = sin(angle);
    dir[2] = 0;
}

__inline float dist( vec3_t x, vec3_t y )  { return sqrt(sqr(x[0]-y[0])+sqr(x[1]-y[1])+sqr(x[2]-y[2])); }
__inline float dist2( vec3_t x, vec3_t y ) { return sqr(x[0]-y[0])+sqr(x[1]-y[1])+sqr(x[2]-y[2]); }

__inline float dist2d( vec3_t x, vec3_t y )  { return sqrt(sqr(x[0]-y[0])+sqr(x[1]-y[1])); }
__inline float dist2d2( vec3_t x, vec3_t y ) { return sqr(x[0]-y[0])+sqr(x[1]-y[1]); }

//== from p_client.c
void SelectSpawnPoint (edict_t *ent, vec3_t origin, vec3_t angles);
void InitClientPersistant (gclient_t *client);
void TossClientWeapon (edict_t *self);
void ClientObituary (edict_t *self, edict_t *inflictor, edict_t *attacker);
void CopyToBodyQue (edict_t *ent);

//== from g_target.c
void target_laser_start (edict_t *self);

//== from g_items.c
#define HEALTH_IGNORE_MAX       1
qboolean Pickup_Health (edict_t *ent, edict_t *other);
qboolean Pickup_Ammo (edict_t *ent, edict_t *other);
qboolean Pickup_Armor (edict_t *ent, edict_t *other);
qboolean Pickup_Weapon (edict_t *ent, edict_t *other);
qboolean Pickup_Adrenaline (edict_t *ent, edict_t *other);
qboolean Pickup_Powerup(edict_t *ent, edict_t *other);
qboolean Pickup_PowerArmor(edict_t *ent, edict_t *other);
qboolean Pickup_Bandolier(edict_t *ent, edict_t *other);
qboolean Pickup_Pack(edict_t *ent, edict_t *other);
void Use_Quad( edict_t *ent, gitem_t *item );
void Use_Invulnerability( edict_t *ent, gitem_t *item );
void Use_Breather( edict_t *ent, gitem_t *item );
void Weapon_Blaster (edict_t *ent);

//== from g_monster.c
void M_WorldEffects (edict_t *self);

//== touch funcs
void Touch_Plat_Center (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf);
void teleporter_touch (edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
void Use_Plat (edict_t *ent, edict_t *other, edict_t *activator);
//void trigger_elevator_use (edict_t *self, edict_t *other, edict_t *activator);
void door_use (edict_t *self, edict_t *other, edict_t *activator);

void rocket_touch  (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf);
void blaster_touch (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf);
void Grenade_Touch (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf);

//== local functions
void cr_pain ( edict_t *self, edict_t *other, float kick, int damage );
void cr_die ( edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void cr_respawn( edict_t *self );

void cr_sight(edict_t *self, edict_t *other);
void cr_think (edict_t *self);
void cr_think_pickup ( edict_t *self );
void cr_think_temp_target ( edict_t *self );
void cr_think_attack (edict_t *self);
void cr_think_chase (edict_t *self);
void cr_think_chase_route (edict_t *self);
void cr_think_taunt (edict_t *self);
void cr_think_flip (edict_t *self);
void cr_think_salute (edict_t *self);
void cr_think_run_for_life (edict_t *self);
void cr_think_follow_route( edict_t *self );
void cr_think_team_help( edict_t *self );
void cr_think_team_group( edict_t *self );
void cr_think_team_guard( edict_t *self );
void cr_think_team_patrol( edict_t *self );
void cr_think_wait( edict_t *self );

void cr_run_frames( edict_t *self, int start, int end );
void cr_animate_frames( edict_t *self, int start, int end );

qboolean cr_fire_weapon( edict_t *self, edict_t *target );
qboolean cr_moveto( edict_t *self );

qboolean cr_find_route( edict_t *self, vec3_t target, qboolean b_important );

void cr_init_node_net();
void cr_init_node_keeper();


//== sounds
static int sound_footstep[4];

//== constants
const float BOT_MIN_SPEED = 120.f;
const float BOT_MAX_SPEED = 300.f;

const float BOT_MIN_ROT_SPEED = 10.f;
const float BOT_MAX_ROT_SPEED = 90.f;

const int   STEPSIZE         = 22;

const int   TIME_WEAPON_CHANGE = 1.f;

const int   CROUCH_DELTA = 32;

const float WATER_SPEED_COEF  = 0.8f;
const float CROUCH_SPEED_COEF = 0.6f;

const float FIGHT_MSG_DELAY = 10;
const float JUMP_DELAY = 0.2f;

const float MESSAGE_DELAY = 10.f;

const float ROCKET_MIN_AVOID_DIST =  30.f;
const float ROCKET_AVOID_DIST     = 180.f;
const float ROCKET_MAX_AVOID_DIST = 250.f;

//=== weapons
#define SPEED_MODIFIER 1.2f

const int BLASTER_SPEED       = 1000;
const int BLASTER_DAMAGE      = 15;
const float TIME_BLASTER_SHOT = 0.5f*SPEED_MODIFIER;
const float TIME_BLASTER_AFTERSHOT = 0.2f;

const int SHOTGUN_SPEED       = -1;
const int SHOTGUN_DAMAGE      = 4;
const float TIME_SHOTGUN_SHOT = 1.2f*SPEED_MODIFIER;
const float TIME_SHOTGUN_AFTERSHOT = 0.2f;

const int SUPERSHOTGUN_SPEED       = -1;
const int SUPERSHOTGUN_DAMAGE      = 6;
const float TIME_SUPERSHOTGUN_SHOT = 1.2f*SPEED_MODIFIER;
const float TIME_SUPERSHOTGUN_AFTERSHOT = 0.2f;

const int MACHINEGUN_SPEED  = -1;
const int MACHINEGUN_DAMAGE = 8;
const float TIME_MACHINEGUN_SHOT = 0.1f;//*SPEED_MODIFIER;
const float TIME_MACHINEGUN_AFTERSHOT = 0.2f;

const int CHAINGUN_SPEED  = -1;
const int CHAINGUN_DAMAGE = 6;
const float TIME_CHAINGUN_SHOT = 0.04f;//*SPEED_MODIFIER;
const float TIME_CHAINGUN_AFTERSHOT = 1.f;

const int ROCKET_SPEED  = 650;
const int ROCKET_DAMAGE = 100;
const float TIME_ROCKET_SHOT = 0.85f*SPEED_MODIFIER;
const float TIME_ROCKET_AFTERSHOT = 0.f;

const int GRENADE_SPEED  = 600;
const int GRENADE_DAMAGE = 120;
const float TIME_GRENADE_SHOT = 1.2f*SPEED_MODIFIER;
const float TIME_GRENADE_AFTERSHOT = 0.f;

const int HYPERBLASTER_SPEED  = 1000;
const int HYPERBLASTER_DAMAGE = 15;
const float TIME_HYPERBLASTER_SHOT = 0.1f;//*SPEED_MODIFIER;
const float TIME_HYPERBLASTER_AFTERSHOT = 0.5f;

const int BFG_SPEED  = 400;
const int BFG_DAMAGE = 200;
const float TIME_BFG_SHOT = 1.2f;//*SPEED_MODIFIER;
const float TIME_BFG_AFTERSHOT = 1.1f;
const float TIME_BFG_SPINUP = 1.f;

const int RAILGUN_SPEED  = -1;
const int RAILGUN_DAMAGE = 100;
const float TIME_RAILGUN_SHOT = 1.5f*SPEED_MODIFIER;
const float TIME_RAILGUN_AFTERSHOT = 0.2f;

//== team constants
const float TEAM_HELP_TIME = 20;

//== AI constants
const float CHASE_TIME = 20;  // 20 secs.


const float ATTACK_RANGE_MIN = 200;
const float ATTACK_RANGE_MAX = 350;
const float ENGAGE_RANGE_MIN = 300;
const float ENGAGE_RANGE_MAX = 500;

const float PICKUP_RANGE_MIN = 120;
const float PICKUP_RANGE_MAX = 300;

const float MELEE_COMBAT_DIST = 40;
const float TOUCH_DIST = 10;

const float UNREACHABLE_TIMEOUT       = 30;
const float UNREACHABLE_ENEMY_TIMEOUT = 10;

const float NODE_MAX_DIST = 280;
const float NODE_MIN_DIST = 90;

const float MOVE_JUMP_DIST = 30;
const float JUMP_DIST      = 150;
const float JUMP_HEIGHT    = 50;
const float BOT_JUMP_SPEED = 300.f;

const float ROCKETJUMP_MAXDIST = 320;
const float ROCKETJUMP_DIST    = 200;
const float ROCKETJUMP_MINDIST = 100;
const float ROCKETJUMP_HEIGHT  = 220;

const float TIME_STUCK       = 0.6f;
const float TIME_NEXT_ENEMY_MIN = 0.3f;
const float TIME_NEXT_ENEMY_MAX = 1.5f;
const float TIME_NEXT_PICKUP = 0.8f;
const float TIME_LONE_NODE   = 30.f; 
const float TIME_ROAM_DIR_CHANGE = 3.f; 

const float BOT_ACCURACY_VERT  = 0.3f;
const float BOT_ACCURACY_HORZ  = 0.6f;

const float TIME_CROUCH      = 0.5f;
const float TIME_CHECK_STUCK = 0.6f;

//== global net of nodes
static path_node_t* cr_node_head   = NULL;
static edict_t*     cr_node_keeper = NULL;
static int          node_count     = 0;

const int MAX_NODE_COUNT = 2000;

//== number of bots respawned
static int global_bot_number=1;

static bot_info_pers_t* global_bots=NULL;
static int global_bots_count=0;

//== map cycle related
#define MAX_MAP 256
typedef char pathname[32];
static int map_cycle_count=0, next_map=0;
static pathname map_list[MAX_MAP];
void cr_load_maplist();

//== debug vars
#ifdef _DEBUG
static edict_t* global_path_route=NULL;
#endif

#define is_closer(pos1,pos2,xx) ((sqr((pos1)[0]-(pos2)[0])+sqr((pos1)[1]-(pos2)[1])+sqr((pos1)[2]-(pos2)[2]))<((xx)*(xx)))
#define is_closer_b(pos1,pos2,xx) ( fabs((pos1)[0]-(pos2)[0])<(xx) && fabs((pos1)[1]-(pos2)[1])<(xx) && fabs((pos1)[2]-(pos2)[2])<(xx) )

#define MAX_STACK_NODE 1024

#define MAX_MSG_LEN  256
#define MAX_MSG      4096

static char* msg_pain[MAX_MSG];
static int msg_pain_count=0, msg_pain_last=0;
static char* msg_kill[MAX_MSG];
static int msg_kill_count=0, msg_kill_last=0;
static char* msg_death[MAX_MSG];
static int msg_death_count=0, msg_death_last=0;
static char* msg_fight[MAX_MSG];
static int msg_fight_count=0, msg_fight_last=0;


static int INDEX_SLUGS;
static int INDEX_RAILGUN;
static int INDEX_CELLS;
static int INDEX_HYPERBLASTER;
static int INDEX_ROCKETS;
static int INDEX_ROCKET_LAUNCHER;
static int INDEX_GRENADES;
static int INDEX_GRENADE_LAUNCHER;
static int INDEX_BULLETS;
static int INDEX_CHAINGUN;
static int INDEX_MACHINEGUN;
static int INDEX_SHELLS;
static int INDEX_SUPER_SHOTGUN;
static int INDEX_SHOTGUN;
static int INDEX_BLASTER;
static int INDEX_POWER_SCREEN;
static int INDEX_POWER_SHIELD;
static int INDEX_QUAD;
static int INDEX_INVULN;
static int INDEX_BREATHER;
static int INDEX_BFG;
static int INDEX_TECH1, INDEX_TECH2, INDEX_TECH3, INDEX_TECH4;
static int INDEX_FLAG1, INDEX_FLAG2;

void cr_init_variables()
{
    INDEX_SLUGS = ITEM_INDEX(FindItem("slugs"));
    INDEX_RAILGUN = ITEM_INDEX(FindItem("railgun"));
    INDEX_CELLS = ITEM_INDEX(FindItem("cells"));  
    INDEX_BFG = ITEM_INDEX(FindItem("BFG10K"));
    INDEX_HYPERBLASTER = ITEM_INDEX(FindItem("hyperblaster"));
    INDEX_ROCKETS = ITEM_INDEX(FindItem("rockets")); 
    INDEX_ROCKET_LAUNCHER = ITEM_INDEX(FindItem("rocket launcher"));
    INDEX_GRENADES = ITEM_INDEX(FindItem("grenades"));  
    INDEX_GRENADE_LAUNCHER = ITEM_INDEX(FindItem("grenade launcher"));
    INDEX_BULLETS = ITEM_INDEX(FindItem("bullets")); 
    INDEX_CHAINGUN = ITEM_INDEX(FindItem("chaingun"));
    INDEX_MACHINEGUN = ITEM_INDEX(FindItem("machinegun"));
    INDEX_SHELLS = ITEM_INDEX(FindItem("shells"));
    INDEX_SUPER_SHOTGUN = ITEM_INDEX(FindItem("super shotgun"));
    INDEX_SHOTGUN = ITEM_INDEX(FindItem("shotgun"));
    INDEX_BLASTER = ITEM_INDEX(FindItem("blaster"));
    INDEX_POWER_SCREEN = ITEM_INDEX(FindItem("Power Screen"));
    INDEX_POWER_SHIELD = ITEM_INDEX(FindItem("Power Shield"));
    INDEX_INVULN = ITEM_INDEX(FindItem("Invulnerability"));
    INDEX_QUAD = ITEM_INDEX(FindItem("Quad Damage"));
    INDEX_BREATHER = ITEM_INDEX(FindItem("Rebreather"));
    INDEX_TECH1 = ITEM_INDEX(FindItemByClassname("item_tech1"));
    INDEX_TECH2 = ITEM_INDEX(FindItemByClassname("item_tech2"));
    INDEX_TECH3 = ITEM_INDEX(FindItemByClassname("item_tech3"));
    INDEX_TECH4 = ITEM_INDEX(FindItemByClassname("item_tech4"));
    INDEX_FLAG1 = ITEM_INDEX(FindItemByClassname("item_flag_team1"));
    INDEX_FLAG2 = ITEM_INDEX(FindItemByClassname("item_flag_team2"));
}

qboolean pos_visible ( vec3_t spot1, vec3_t spot2 )
{
    trace_t trace;
    trace = gi.trace ( spot1, vec3_origin, vec3_origin, spot2, NULL, MASK_OPAQUE );
    return (trace.fraction == 1.f);
}

qboolean pos_reachable ( vec3_t spot1, vec3_t spot2 )
{
    trace_t trace;
    trace = gi.trace ( spot1, vec3_origin, vec3_origin, spot2, NULL, 
                       (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_PLAYERCLIP) );
    return (trace.fraction==1.f);
}

qboolean can_reach ( edict_t *self, edict_t *other )
{
    vec3_t  spot1;
    vec3_t  spot2;

    VectorCopy (self->s.origin, spot1);
    spot1[2] += self->viewheight;
    VectorCopy (other->s.origin, spot2);
    spot2[2] += other->viewheight;

    return pos_reachable (spot1,spot2);
}

qboolean cr_infront ( edict_t *self, edict_t *other )
{
    vec3_t  vec;
    float   dot;
    vec3_t  forward;
    
    AngleVectors (self->s.angles, forward, NULL, NULL);
    VectorSubtract (other->s.origin, self->s.origin, vec);
    VectorNormalize (vec);
    dot = DotProduct (vec, forward);

    return dot > (13-self->bot_pers->skill)*0.06f;
}

qboolean is_touched( edict_t* self, vec3_t pos )
{
    if ( pos[0] < (self->s.origin[0] + self->mins[0]) ) return false;
    if ( pos[0] > (self->s.origin[0] + self->maxs[0]) ) return false;
    if ( pos[1] < (self->s.origin[1] + self->mins[1]) ) return false;
    if ( pos[1] > (self->s.origin[1] + self->maxs[1]) ) return false;
    if ( pos[2] < (self->s.origin[2] + self->mins[2]) ) return false;
    if ( pos[2] > (self->s.origin[2] + self->maxs[2]) ) return false;
    return true;
}

qboolean is_touched2d( edict_t* self, vec3_t pos )
{
    if ( pos[0] < (self->s.origin[0] + self->mins[0]) ) return false;
    if ( pos[0] > (self->s.origin[0] + self->maxs[0]) ) return false;
    if ( pos[1] < (self->s.origin[1] + self->mins[1]) ) return false;
    if ( pos[1] > (self->s.origin[1] + self->maxs[1]) ) return false;
    return true;
}


#ifdef _DEBUG

void line_think ( edict_t* self )
{
    VectorCopy ( self->pos2, self->s.old_origin);

    self->nextthink = level.time + FRAMETIME;
    self->think = G_FreeEdict;
}

#define COLOR_BEAM_GREEN  4
#define COLOR_BEAM_BLUE   8

void cr_draw_line ( vec3_t pos1, vec3_t pos2, int col, edict_t** chain )
{
    edict_t *beam;
    beam = G_Spawn();

    beam->spawnflags = 1 | col;
    beam->dmg = 0.001;
    beam->classname = "laser line";

    target_laser_start (beam);

    VectorCopy( pos1, beam->s.origin );
    VectorCopy( pos2, beam->pos2 );

    beam->think = line_think;
    beam->nextthink = level.time + FRAMETIME;

    gi.linkentity (beam);

    if (chain) {
      beam->flags |= FL_TEAMSLAVE;
      beam->teamchain = *chain;
      *chain = beam;
      }
}

void cr_marker( vec3_t pos )
{
    edict_t* t; 
    t = G_Spawn();
    VectorSet( t->mins, -10, -10, -10);
    VectorSet( t->maxs, 10, 10, 10);
    VectorClear( t->velocity );
    VectorCopy( pos, t->s.origin );
    t->classname = "debug marker";
    t->model = "models/items/adrenal/tris.md2";
    t->s.modelindex = gi.modelindex(t->model);

    t->nextthink = level.time + 30;
    t->think = G_FreeEdict;

    gi.linkentity(t);

  //  gi.bprintf ( PRINT_MEDIUM, "pos %.1f %.1f %.1f\n", pos[0], pos[1], pos[2] );
}

#endif

void cr_set_move_target( edict_t* self, vec3_t target )
{
    VectorCopy( target, self->bot_info->move_target );
    self->bot_info->move_target[2] += STEPSIZE;
}

void cr_add_direct_route( path_node_t* node1, path_node_t* node2, qboolean bComputeDistance )
{
    qboolean bAdd;
    int      i, j;
    float    d;
  // establish links

    if (!node1 || !node2 || node1==node2) return;

#ifdef _DEBUG
   // cr_draw_line( node1->position, node2->position, COLOR_BEAM_BLUE, NULL );
#endif

    bAdd=false;
    for ( i=0; i<MAX_NODE_LINKS; i++ ) {
       if ( node1->link_to[i]==node2 ) break;
       if (!node1->link_to[i]) { bAdd=true; break; }
       }
    if (!bAdd) return;

    bAdd=false;
    for ( j=0; j<MAX_NODE_LINKS; j++ ) {
       if ( node2->link_from[j]==node1 ||
           !node2->link_from[j] ) { bAdd=true; break; }
       }
    if (!bAdd) return;

    node1->link_to[i] = node2;
    node2->link_from[j] = node1;

    if (bComputeDistance) d=dist( node1->position, node2->position );
                     else d=1.f;
    node1->link_dist[i] = d;
}

void cr_add_direct_route_uni( path_node_t* node1, path_node_t* node2 )
{
    if (!node1 || !node2 || node1==node2) return;
    cr_add_direct_route( node1, node2, true );
    cr_add_direct_route( node2, node1, true );
}

void cr_remove_direct_route( path_node_t* node1, path_node_t* node2 )
{
    int i, r;

    if (!node1 || !node2 || node1==node2) return;

    r=-1;
    for ( i=0; i<MAX_NODE_LINKS; i++ ) {
       if (!node1->link_to[i]) break;
       if ( node1->link_to[i]==node2 ) r=i;
       }
    i--;

    if (i>=0 && r>=0) {
      node1->link_to[r] = node1->link_to[i];
      node1->link_to[i] = NULL;
      }

    r=-1;
    for ( i=0; i<MAX_NODE_LINKS; i++ ) {
       if (!node2->link_from[i]) break;
       if ( node2->link_from[i]==node1 ) r=i;
       }
    i--;

    if (i>=0 && r>=0) {
      node2->link_from[r] = node2->link_from[i];
      node2->link_from[i] = NULL;
      }
}

void cr_message_pain ( edict_t* self, edict_t* other ) 
{                                                                
    int n;                                                       
    if (!bot_chat->value) return;                                              
    if (!other || !other->inuse || !other->client || other->bot_info) return;
    if (msg_pain_count<2) return;                              
    if ( (level.time-self->bot_info->time_last_message)<MESSAGE_DELAY ) return;
    self->bot_info->time_last_message = level.time - random()*MESSAGE_DELAY/4;
    while (1) {                                                  
         n = rand() % msg_pain_count;                            
         if (n==msg_pain_last) continue;                         
         msg_pain_last=n;                                        
         gi.cprintf ( other, PRINT_CHAT, "%s: %s\n", self->client->pers.netname, msg_pain[n] ); 
         break;                                                  
         }                                                       
}

void cr_message_kill ( edict_t* self, edict_t* other ) 
{                                                                
    int n;          
    if (!bot_chat->value) return;                                              
    if (!other || !other->inuse || !other->client || other->bot_info) return;
    if (msg_kill_count<2) return;                              
    if ( (level.time-self->bot_info->time_last_message)<MESSAGE_DELAY ) return;
    self->bot_info->time_last_message = level.time - random()*MESSAGE_DELAY/4;
    while (1) {                                                  
         n = rand() % msg_kill_count;                            
         if (n==msg_kill_last) continue;                         
         msg_kill_last=n;                                        
         gi.cprintf ( other, PRINT_CHAT, "%s: %s\n", self->client->pers.netname, msg_kill[n] ); 
         break;                                                  
         }                                                       
}

void cr_message_death ( edict_t* self, edict_t* other ) 
{                                                                
    int n;                                                       
    if (!bot_chat->value) return;                                              
    if (!other || !other->inuse || !other->client || other->bot_info) return;       
    if (msg_death_count<2) return;                              
    if ( (level.time-self->bot_info->time_last_message)<MESSAGE_DELAY ) return;
    self->bot_info->time_last_message = level.time - random()*MESSAGE_DELAY/4;
    while (1) {                                                  
         n = rand() % msg_death_count;                            
         if (n==msg_death_last) continue;                         
         msg_death_last=n;                                        
         gi.cprintf ( other, PRINT_CHAT, "%s: %s\n", self->client->pers.netname, msg_death[n] ); 
         break;                                                  
         }                                                       
}

void cr_message_fight ( edict_t* self, edict_t* other ) 
{                                                                
    int n;                                                       
    if (!bot_chat->value) return;                                              
    if (!other || !other->inuse || !other->client || other->bot_info) return;
    if (msg_fight_count<2) return;                              
    if ( (level.time-self->bot_info->time_last_message)<MESSAGE_DELAY ) return;
    self->bot_info->time_last_message = level.time - random()*MESSAGE_DELAY/4;
    while (1) {                                                  
         n = rand() % msg_fight_count;                            
         if (n==msg_fight_last) continue;                         
         msg_fight_last=n;                                        
         gi.cprintf ( other, PRINT_CHAT, "%s: %s\n", self->client->pers.netname, msg_fight[n] ); 
         break;                                                  
         }                                                       
}

qboolean cr_get_avoid_vector( edict_t* self, vec3_t av )
{
    edict_t *hit;
    float    d, old_d;
    vec3_t   pos, rpos, avoid_vect, dir_v;
    int      i;
    qboolean bRes;

    VectorCopy( self->s.origin, pos );
    VectorClear( av );

    bRes = false;
    for ( hit=g_edicts+1; hit<&g_edicts[globals.num_edicts]; hit++) {
       if (!hit->inuse) continue;
       if ( hit->touch != rocket_touch  &&
            hit->touch != Grenade_Touch && 
            (self->bot_pers->skill<6 || hit->touch!=blaster_touch || ((rand()&3)==3)) ) continue;
       if ((hit->svflags & SVF_NOCLIENT) || (hit->solid==SOLID_NOT)) continue;

       VectorCopy( hit->s.origin, rpos );
       old_d = sqr(pos[0]-rpos[0])+sqr(pos[1]-rpos[1])+sqr(pos[2]-rpos[2]);
       if (old_d>(ROCKET_MAX_AVOID_DIST*ROCKET_MAX_AVOID_DIST)) continue;
       VectorMA( rpos, FRAMETIME/2, hit->velocity, rpos );
       for (i=0; i<7; i++) {
          avoid_vect[0] = pos[0]-rpos[0];
          avoid_vect[1] = pos[1]-rpos[1];
          avoid_vect[2] = pos[2]-rpos[2];
          d = sqr(avoid_vect[0])+sqr(avoid_vect[1])+sqr(avoid_vect[2]);
          if (d>(ROCKET_MAX_AVOID_DIST*ROCKET_MAX_AVOID_DIST)) break;
          if (d>old_d) break;
         // old_d = d;
          d = sqrt(d);
          if (d<ROCKET_MIN_AVOID_DIST) {
            // move bot perpendicular to rocket speed
            CrossProduct( avoid_vect, hit->velocity, dir_v );
            if (self->groundentity) {
              dir_v[0]=0;
              dir_v[1]=0;
              dir_v[2] = dir_v[2]>0 ? 1.f : -1.f;
              }
            CrossProduct( hit->velocity, dir_v, avoid_vect );
            VectorNormalize( avoid_vect );
            VectorMA( av, 2*ROCKET_AVOID_DIST/ROCKET_MIN_AVOID_DIST, avoid_vect, av );
            }
          else {
            VectorMA( av, ROCKET_AVOID_DIST/(d*d), avoid_vect, av );
            }
          bRes = true;
          VectorMA( rpos, FRAMETIME, hit->velocity, rpos );
          }
       }

    return bRes;
}

void cr_avoid_rocket( edict_t* self )
{
    vec3_t av;
    float  yaw;

    if ( self->bot_info->time_next_rocket_avoid>level.time ) return;
    self->bot_info->time_next_rocket_avoid = level.time + FRAMETIME*(10-self->bot_pers->skill);

    if (!cr_get_avoid_vector(self,av)) return;

   // gi.dprintf ( "avoid: %.1f %.1f %.1f\n", av[0], av[1], av[2] );

    yaw = self->ideal_yaw*M_PI*2/360;
    av[0] += cos(yaw);
    av[1] += sin(yaw);
    self->ideal_yaw = vectoyaw(av);
}

void cr_add_links_radius( path_node_t *new_node )
{
    path_node_t *node;
    float        max_dist;

    max_dist = NODE_MIN_DIST*1.4f;
    for ( node=cr_node_head; node!=NULL; node=node->next ) {
       if ( is_closer_b( new_node->position, node->position, max_dist) && 
            pos_reachable( new_node->position, node->position ) ) 
         cr_add_direct_route_uni( node, new_node );
       if (new_node->link_to[MAX_NODE_LINKS-1]) return;
       }

    max_dist = NODE_MAX_DIST;
    for ( node=cr_node_head; node!=NULL; node=node->next ) {
       if ( is_closer_b( new_node->position, node->position, max_dist) && 
            pos_reachable( new_node->position, node->position ) ) 
         cr_add_direct_route_uni( node, new_node );
       if (new_node->link_to[MAX_NODE_LINKS-1]) return;
       }
}

path_node_t* cr_find_touched_node ( edict_t *self, vec3_t pos )
{
    path_node_t *best, *node;

    VectorCopy ( self->s.origin, self->s.old_origin);
    VectorCopy ( pos, self->s.origin );

    best = NULL;
    for ( node=cr_node_head; node!=NULL; node=node->next ) {
       if ( is_touched(self,node->position) ) {
         best = node;
         break;
         }
       }

    VectorCopy ( self->s.old_origin, self->s.origin );
    return best;
}

path_node_t *cr_insert_node( vec3_t pos, path_node_t* prev_node, int flags )
{
    path_node_t *new_node;      

#ifdef _DEBUG
    cr_marker(pos);
#endif

    new_node = gi.TagMalloc ( sizeof(path_node_t), TAG_GAME );
    memset(new_node,0,sizeof(path_node_t));
    new_node->time = level.time;

    VectorCopy( pos, new_node->position );

    if (prev_node) cr_add_direct_route_uni( new_node, prev_node );

    // try to establish links to all visible nodes around
    cr_add_links_radius(new_node);

   // link new node in
    new_node->next = cr_node_head;
    cr_node_head = new_node;

    new_node->flags = flags;

    return new_node;
}

float distance( edict_t *e1, edict_t *e2 )
{
    vec3_t  vec;

    if (!e1 || !e2) return 1e32;

    VectorSubtract ( e1->s.origin, e2->s.origin, vec);
    return VectorLength (vec);
}

void cr_add_unreachable( edict_t *self, edict_t *other, float timeout )
{
    int         i, best_i;
    float       best_time;
    bot_info_t* bi = self->bot_info;

    if (!other) return;

    best_time = 1e32;
    best_i=0;
    for ( i=0; i<MAX_UNREACHABLES; i++ ) {
       if ( bi->unreachable[i]==other || !bi->unreachable[i] ) {
         best_i = i;
         break;
         }
       if (bi->time_unreachable[i]<best_time) {
         best_i = i;
         best_time = bi->time_unreachable[i];
         }
       }

    bi->unreachable[best_i] = other;
    bi->time_unreachable[best_i] = level.time + timeout*(0.9f+random()*0.2f);

   // gi.bprintf ( PRINT_MEDIUM, "marking %s as unreachable for %.1f sec. (%d)\n", other->classname, bi->time_unreachable[best_i]-level.time, best_i );
}

qboolean cr_check_unreachable( edict_t *self, edict_t *other )
{
    int i;
    bot_info_t* bi = self->bot_info;

    if (!other) return true;

    for ( i=0; i<MAX_UNREACHABLES; i++ ) {
       if (!bi->unreachable[i]) continue;
       if ( bi->time_unreachable[i]<level.time ) {
        // remove it
        // gi.bprintf ( PRINT_MEDIUM, "(1) removing %s from unreachables\n", bi->unreachable[i]->classname );
         bi->unreachable[i] = NULL;
         bi->time_unreachable[i] = 0;
         }
       else 
       if (bi->unreachable[i]==other) return true;
       }

    return false;
}

void cr_remove_unreachable( edict_t *self, edict_t *other )
{
    int i;
    bot_info_t* bi = self->bot_info;

    if (!other) return;

    for ( i=0; i<MAX_UNREACHABLES; i++ ) {
       if (!bi->unreachable[i]) continue;
       if ( bi->time_unreachable[i]<level.time ||
            bi->unreachable[i]==other ) {
        // remove it
         bi->unreachable[i] = NULL;
         bi->time_unreachable[i] = 0;
        // gi.bprintf ( PRINT_MEDIUM, "(2) removing %s from unreachables\n", other->classname );
         }
       }
}

void cr_forget_pickup_target( edict_t *self )
{
    self->bot_info->pickup_target = NULL;
    self->bot_info->pickup_target_score = 1e32;
}

void cr_forget_enemy( edict_t *self )
{
    self->oldenemy = self->enemy;
    self->enemy = NULL;
}

qboolean cr_low_health( edict_t *self )
{
    return self->health<(self->max_health/8);
}

qboolean cr_avoid_fight( edict_t *self )
{
    return (self->client->pers.inventory[INDEX_FLAG1]>0) ||
           (self->client->pers.inventory[INDEX_FLAG2]>0); 
}
                        
qboolean cr_find_enemy( edict_t *self ) 
{
    edict_t *client, *best_client;
    float    dist, best_dist;
    int      min_light_level;

    if (cr_avoid_fight(self)) return false;
    if (cr_low_health(self)) return false;

    min_light_level = (14 - self->bot_pers->skill)*.4f;

    if (self->enemy && self->bot_info->time_next_enemy>level.time) {
      self->oldenemy = self->enemy;
      VectorCopy( self->enemy->s.origin, self->monsterinfo.last_sighting );
      self->monsterinfo.trail_time = level.time;
      return true;
      }
    self->bot_info->time_next_enemy = level.time + 
       TIME_NEXT_ENEMY_MIN + (TIME_NEXT_ENEMY_MAX-TIME_NEXT_ENEMY_MIN)*0.1f*self->bot_pers->skill;

    if (self->client->pers.weapon == FindItem ("blaster")) {
      if (random()<0.8f) return false; 
      }

    best_client=NULL;
    best_dist=1e32;

    for ( client=g_edicts+1; client<=(g_edicts+game.maxclients); client++) {
       if (!client->inuse || client==self || (client->flags & FL_NOTARGET)) continue;

      // attack only bots from other teams, unless we are in team #0
       if ( self->client->pers.team_no>0 && 
            client->client->pers.team_no==self->client->pers.team_no ) continue;

      // is it dead already?
       if ( client->deadflag>DEAD_DYING || (client->svflags & SVF_NOCLIENT) ) continue;
       if (cr_check_unreachable(self,client)) continue;

      // can we see him?
       if (client!=self->oldenemy && !cr_infront (self,client)) continue;
       if (!can_reach(self,client)) continue;

       dist = distance(self,client);

      // is client in an spot too dark to be seen?
       if (!client->bot_info && client->light_level<=min_light_level) {
         if ( client->light_level < (dist-100)*0.002f*min_light_level ) continue;
         }

       if ( !best_client || dist<best_dist ) {
         best_client = client;
         best_dist = dist;
         if (best_dist<MELEE_COMBAT_DIST) break;
         }
       }

    if (!best_client) return false;

    if ( !self->enemy &&
         self->bot_info->time_next_shot < level.time ) {
      self->bot_info->time_next_shot = level.time + 0.05f*(11-self->bot_pers->skill)*(1.f+0.2f*random());
      }

    cr_forget_enemy(self);
    self->enemy = best_client;
    VectorCopy( self->enemy->s.origin, self->monsterinfo.last_sighting );
    self->monsterinfo.trail_time = level.time;

    cr_forget_pickup_target(self);

    return true;
}

void cr_create_bot_structure( edict_t *new_bot )
{
    new_bot->bot_pers = gi.TagMalloc ( sizeof(bot_info_pers_t), TAG_GAME );
    memset(new_bot->bot_pers,0,sizeof(bot_info_pers_t));
    new_bot->bot_info = gi.TagMalloc ( sizeof(bot_info_t), TAG_GAME );
    memset(new_bot->bot_info,0,sizeof(bot_info_t));
}

void cr_compute_skills( edict_t *self, int skill_level )
{
    self->bot_pers->speed = BOT_MIN_SPEED + (BOT_MAX_SPEED-BOT_MIN_SPEED)*0.1f*skill_level;
    self->bot_pers->rot_speed = BOT_MIN_ROT_SPEED + (BOT_MAX_ROT_SPEED-BOT_MIN_ROT_SPEED)*0.1f*skill_level;
    self->bot_pers->attack_range = ATTACK_RANGE_MIN + (ATTACK_RANGE_MAX-ATTACK_RANGE_MIN)*0.1f*skill_level; 
    self->bot_pers->engage_range = ENGAGE_RANGE_MIN + (ENGAGE_RANGE_MAX-ENGAGE_RANGE_MIN)*0.1f*skill_level; 

    self->bot_pers->adapt_count = 0;
}

void cr_register_new_bot( bot_info_pers_t* new_bot )
{
    int i;

    for ( i=0; i<global_bots_count; i++ ) {
       if (global_bots[i].b_inuse) continue;
       memcpy( global_bots+i, new_bot, sizeof(bot_info_pers_t) );
       global_bots[i].b_inuse = true;
       break;
       } 
}

void cr_unregister_bot( bot_info_pers_t* bot )
{
    int i;

    for ( i=0; i<global_bots_count; i++ ) {
       if (!global_bots[i].b_inuse ||
           bot->playernum!=global_bots[i].playernum) continue;
       memset(global_bots+i,0,sizeof(bot_info_pers_t));  
       game.clients[bot->playernum].inuse = false;
       global_bots[i].b_inuse = false;
       }
}

int cr_find_unused_client_slot()
{
   edict_t  *cl_ent;
   int       i;

   for ( i=0; i<game.maxclients; i++ ) {
       cl_ent = g_edicts + 1 + i;
       if ( !cl_ent->inuse && !cl_ent->client->inuse ) return i;
       }
   return -1;
}

void cr_init_per_info( edict_t* self )
{
    strcpy ( self->client->pers.netname, self->bot_pers->name );
    self->client->pers.team_no = self->bot_pers->team_no;

    sprintf ( self->client->pers.userinfo, "\\msg\\1\\rate\\25000\\name\\%s\\skin\\%s\\fov\\90\\hand\\0\\ip\\loopback", self->bot_pers->name, self->bot_pers->skin );
}

void cr_update_userinfo( bot_info_pers_t* bot_pers )
{
    gi.configstring( CS_PLAYERSKINS+bot_pers->playernum, 
                     va( "%s\\%s", bot_pers->name, bot_pers->skin ) );
}


void SP_crbot( char* name, int skill_level, char* skin, int team, char* model )
{
    edict_t *new_crbot;
    int      playernum;

    if (!deathmatch->value) {
      gi.dprintf ( "Start a deathmatch game first or add '+set deathmatch 1' to the command line\n" );
      return;
      }

    playernum = cr_find_unused_client_slot();
    if (playernum<0) {
      gi.dprintf ( "*** Not enough client slots to create a new bot.\n*** Increase 'maxclients' var and restart the map.\n*** You can do it by typing 'maxclients #' in the console window or\n*** by adding '+set maxclients #' to your command line.\n*** To restart the map type 'map map_name_here'\n" );
      return;
      }

    // spawn the bot on a spawn spot
    new_crbot = g_edicts + playernum + 1; //G_Spawn();
    G_InitEdict(new_crbot);
    cr_create_bot_structure(new_crbot);

 //=== init bot_pers
    new_crbot->bot_pers->b_adapting = (skill_level==0);
    new_crbot->bot_pers->playernum = playernum;

    if (skill_level<1)  skill_level=5; 
    if (skill_level>10) skill_level=10; 
    new_crbot->bot_pers->skill = skill_level;
    cr_compute_skills( new_crbot, skill_level );

    new_crbot->bot_pers->team_no = team;
    if (!skin || !*skin) skin = bot_skin->string;
    strncpy( new_crbot->bot_pers->skin, skin, 64 );
    if (!model || !*model) model = bot_model->string;
    sprintf( new_crbot->bot_pers->model, "players/%s/tris.md2", model );
    if (!strchr(new_crbot->bot_pers->skin,'/')) {
      char nm[128];
      sprintf( nm, "%s/%s", bot_model->string, new_crbot->bot_pers->skin );
      strncpy( new_crbot->bot_pers->skin, nm, 64 );
      }

    if (!name || !*name) sprintf( new_crbot->bot_pers->name, "crbot%d", global_bot_number++ );
                    else strcpy ( new_crbot->bot_pers->name, name );
  //===
    cr_update_userinfo(new_crbot->bot_pers);
    cr_register_new_bot(new_crbot->bot_pers);

   // new_crbot->client = &game.clients[new_crbot->bot_pers->playernum]; // not required any more
    new_crbot->client->inuse = true;
    InitClientResp(new_crbot->client);
    InitClientPersistant(new_crbot->client);

    cr_init_per_info(new_crbot);

    new_crbot->classname = "new_bot";
    new_crbot->think = cr_respawn;
    new_crbot->nextthink = level.time + random();
}

qboolean cr_force_attack_enemy( edict_t*self, edict_t *enemy )
{
    if (cr_find_route( self, enemy->s.origin, true )) {
      VectorCopy( enemy->s.origin, self->monsterinfo.last_sighting );
      self->monsterinfo.trail_time = level.time;
      self->enemy = enemy;
      self->think = cr_think_chase_route;
      return true;
      }
    return false;
}

void cr_call_teamers( edict_t*self, edict_t *enemy )
{
    edict_t* bot;

    if (self->client->pers.team_no<=0) return;

    for ( bot=g_edicts+1; bot<=(g_edicts+game.maxclients); bot++) {
       if ( !bot->inuse || bot==self || bot->deadflag==DEAD_DEAD ||
            stricmp( bot->classname, "bot" )!=0 ) continue;

       if ( bot->client->pers.team_no==self->client->pers.team_no && 
            !cr_low_health(bot) &&
            !bot->enemy && (bot->think==cr_think || bot->think==cr_think_pickup) ) {
         cr_force_attack_enemy(bot,enemy);
         }
       }
}

void cr_team_help( edict_t* self )
{
    edict_t* bot;

    if (self->client->pers.team_no<=0) {
      gi.cprintf( self, PRINT_HIGH, "You are not in any team!\n");
      return;
      }

    for ( bot=g_edicts+1; bot<=(g_edicts+game.maxclients); bot++) {
       if ( !bot->inuse || bot==self || bot->deadflag==DEAD_DEAD ||
            stricmp( bot->classname, "bot" )!=0 ) continue;

       if ( bot->client->pers.team_no!=self->client->pers.team_no || 
            cr_low_health(self) ||
            bot->enemy || (bot->think!=cr_think && bot->think!=cr_think_pickup) ) continue;

       if (!cr_find_route( bot, self->s.origin, true )) {
         gi.cprintf ( self, PRINT_CHAT, "%s: Don't know how to get to your location, Sir!\n", bot->client->pers.netname ); 
         }
       else {
         gi.cprintf ( self, PRINT_CHAT, 
                      random()<0.5f ? "%s: Yes, Sir!\n" : "%s: On my way, Sir!\n", bot->client->pers.netname ); 
         bot->bot_info->team_leader = self;
         bot->bot_info->bot_assignment = ASSN_HELP;
         bot->think = cr_think_team_help;
         }
       }
}

void cr_team_group( edict_t* self )
{
    edict_t* bot;

    if (self->client->pers.team_no<=0) {
      gi.cprintf( self, PRINT_HIGH, "You are not in any team!\n");
      return;
      }

    for ( bot=g_edicts+1; bot<=(g_edicts+game.maxclients); bot++) {
       if ( !bot->inuse || bot==self || bot->deadflag==DEAD_DEAD ||
            stricmp( bot->classname, "bot" )!=0 ) continue;

       if ( bot->client->pers.team_no!=self->client->pers.team_no || 
            cr_low_health(self) ||
            bot->enemy || (bot->think!=cr_think && bot->think!=cr_think_pickup) ) continue;

       if (!cr_find_route( bot, self->s.origin, true )) {
         gi.cprintf ( self, PRINT_CHAT, "%s: Don't know how to get to your location, Sir!\n", bot->client->pers.netname ); 
         }
       else {
         gi.cprintf ( self, PRINT_CHAT, 
                      random()<0.6f ? "%s: Yes, Sir!\n" : "%s: On my way, Sir!\n", bot->client->pers.netname ); 
         bot->bot_info->team_leader = self;
         bot->bot_info->bot_assignment = ASSN_GROUP;
         bot->think = cr_think_team_group;
         }
       }
}

void cr_team_patrol( edict_t* self, char* name )
{
    edict_t* bot;
    char* bot_name = NULL;

    if (name && *name) bot_name=name;

    if (self->client->pers.team_no<=0) {
      gi.cprintf( self, PRINT_HIGH, "You are not in any team!\n");
      return;
      }

    for ( bot=g_edicts+1; bot<=(g_edicts+game.maxclients); bot++) {
       if ( !bot->inuse || bot==self || bot->deadflag==DEAD_DEAD ||
            stricmp( bot->classname, "bot" )!=0 ) continue;

       if (bot->client->pers.team_no!=self->client->pers.team_no) continue;
       if (cr_low_health(self) || bot->enemy) continue;
       if ( bot->bot_info->bot_assignment==ASSN_NONE && 
            bot->think!=cr_think && bot->think!=cr_think_pickup ) continue;

       if (!bot_name) {
         if (!infront(self,bot)) continue;
         if (!pos_visible(self->s.origin,bot->s.origin)) continue;
         }
       else
         if (stricmp(bot->client->pers.netname,bot_name)!=0 ) continue;

       if (!cr_find_route( bot, self->s.origin, true )) {
         gi.cprintf ( self, PRINT_CHAT, "%s: Don't know how to get to your location, Sir!\n", bot->client->pers.netname ); 
         }
       else {
         gi.cprintf ( self, PRINT_CHAT, 
                      random()<0.6f ? "%s: Yes, Sir! Patroling the area.\n" : "%s: On my way, Sir!\n", bot->client->pers.netname ); 
         bot->bot_info->team_leader = self;
         VectorCopy ( self->s.origin, bot->bot_info->bot_anchor );
         bot->bot_info->bot_assignment = ASSN_PATROL;
         bot->think = cr_think_team_patrol;
         }
       }
}

void cr_team_guard( edict_t* self, char* name )
{
    edict_t* bot;
    char* bot_name = NULL;

    if (name && *name) bot_name=name;

    if (self->client->pers.team_no<=0) {
      gi.cprintf( self, PRINT_HIGH, "You are not in any team!\n");
      return;
      }

    for ( bot=g_edicts+1; bot<=(g_edicts+game.maxclients); bot++) {
       if ( !bot->inuse || bot==self || bot->deadflag==DEAD_DEAD ||
            stricmp( bot->classname, "bot" )!=0 ) continue;

       if (bot->client->pers.team_no!=self->client->pers.team_no) continue;
       if (cr_low_health(self) || bot->enemy) continue;
       if ( bot->bot_info->bot_assignment==ASSN_NONE && 
            bot->think!=cr_think && bot->think!=cr_think_pickup ) continue;

       if (!bot_name) {
         if (!infront(self,bot)) continue;
         if (!pos_visible(self->s.origin,bot->s.origin)) continue;
         }
       else
         if (stricmp(bot->client->pers.netname,bot_name)!=0 ) continue;

       if (!cr_find_route( bot, self->s.origin, true )) {
         gi.cprintf ( self, PRINT_CHAT, "%s: Don't know how to get to your location, Sir!\n", bot->client->pers.netname ); 
         }
       else {
         gi.cprintf ( self, PRINT_CHAT, 
                      random()<0.6f ? "%s: Yes, Sir! Guarding the area.\n" : "%s: On my way, Sir!\n", bot->client->pers.netname ); 
         bot->bot_info->team_leader = self;
         VectorCopy ( self->s.origin, bot->bot_info->bot_anchor );
         bot->bot_info->bot_assignment = ASSN_GUARD;
         bot->think = cr_think_team_guard;
         }
       }
}

void cr_dismiss( edict_t* self )
{
    self->bot_info->team_leader = NULL;
    self->bot_info->bot_assignment = ASSN_NONE;
    VectorClear(self->bot_info->bot_anchor);
}

void cr_team_free( edict_t* self, char* name )
{
    char* bot_name = NULL;
    edict_t* bot;

    if (name && *name) bot_name=name;

    if (self->client->pers.team_no<=0) {
      gi.cprintf( self, PRINT_HIGH, "You are not in any team!\n");
      return;
      }

    for ( bot=g_edicts+1; bot<=(g_edicts+game.maxclients); bot++) {
       if ( !bot->inuse || bot==self || bot->deadflag==DEAD_DEAD ||
            stricmp( bot->classname, "bot" )!=0 ) continue;

       if ( bot->client->pers.team_no!=self->client->pers.team_no ) continue;
       if ( bot_name && stricmp(bot->client->pers.netname,bot_name)!=0 ) continue;

       if (bot->bot_info->bot_assignment!=ASSN_NONE) {
         gi.cprintf( self, PRINT_CHAT, "%s: Yes, Sir!\n", bot->client->pers.netname ); 
         cr_dismiss(bot);
         }
       }
}

void cr_pain ( edict_t *self, edict_t *other, float kick, int damage )
{
    if (self->deadflag==DEAD_DEAD) return;

    if (level.time>self->pain_debounce_time) {
      int r, l;

      if (random()<0.4f) cr_message_pain ( self, other );

      self->pain_debounce_time = level.time + 0.8f;

      r = 1 + (rand()&1);
           if (self->health < 25) l = 25;
      else if (self->health < 50) l = 50;
      else if (self->health < 75) l = 75;
      else l = 100;
      gi.sound ( self, CHAN_VOICE, gi.soundindex(va("*pain%i_%i.wav", l, r)), 0.6f, ATTN_NORM, 0);
      }

    if (other && other->client) {
      if (other!=self && self->client->pers.team_no>0) {
       // call other teamers!
        if ( self->client->pers.team_no != other->client->pers.team_no && 
             level.time > self->bot_info->time_next_callforhelp ) {
           cr_call_teamers( self, other );
           self->bot_info->time_next_callforhelp = level.time + 5.f;
           }
        }

      if ( other!=self && other!=self->enemy && 
           (!self->enemy || random()<0.3f) && other->client &&
           ( self->client->pers.team_no<=0 || 
             self->client->pers.team_no!=other->client->pers.team_no || random()<0.2f ) ) {
        cr_forget_pickup_target(self);
        cr_forget_enemy(self);

        self->enemy = other;
        cr_remove_unreachable( self, self->enemy );

        if ( level.time > self->bot_info->time_next_shot )
          self->bot_info->time_next_shot = level.time + TIME_WEAPON_CHANGE*(11-self->bot_pers->skill)/5;

        self->think = cr_think_chase;
        }
      }
}

edict_t* cr_respawn_bot( bot_info_pers_t* bot_pers, client_respawn_t* resp )
{
    edict_t  *new_crbot;

    new_crbot = g_edicts + bot_pers->playernum + 1; //G_Spawn();
    G_InitEdict(new_crbot);
    cr_create_bot_structure(new_crbot);

    *new_crbot->bot_pers = *bot_pers;

   // new_crbot->client = &game.clients[new_crbot->bot_pers->playernum]; // not requaired any more
    new_crbot->client->inuse = true;
    InitClientResp(new_crbot->client);
    InitClientPersistant(new_crbot->client);

    if (resp) new_crbot->client->resp = *resp;

    cr_init_per_info(new_crbot);

    new_crbot->classname = "bot";
    new_crbot->think = cr_respawn;
    new_crbot->nextthink = level.time + 2.f+random(); // 2-3 secs

    return new_crbot;
}

void cr_free_bot( edict_t *self )
{
    gi.TagFree(self->bot_pers); self->bot_pers=NULL;
    gi.TagFree(self->bot_info); self->bot_info=NULL;

   // G_FreeEdict (self);
    self->s.modelindex = 0;
    self->s.modelindex2 = 0;
    self->solid = SOLID_NOT;
    self->inuse = false;
    self->classname = "disconnected";
}

void cr_death( edict_t *self )
{
    if ( self->s.frame < (self->client->anim_end-10) ) {
      self->s.frame = self->client->anim_end-6;
      }
    
    self->s.frame++;
    if (self->s.frame<self->client->anim_end) {
      self->nextthink = level.time + FRAMETIME;
      }
    else {
      bot_info_pers_t  save_bot_pers;
      client_respawn_t resp;

      self->s.frame = self->client->anim_end;
      save_bot_pers = *self->bot_pers;
      resp = self->client->resp;

      self->client->anim_priority = ANIM_BASIC;

      if ( self->takedamage!=DAMAGE_NO &&
           self->s.frame >= FRAME_crdeath1 ) {
        self->s.effects = 0;
        self->s.renderfx = 0;
        CopyToBodyQue(self);
        }

      cr_free_bot( self );
      cr_respawn_bot( &save_bot_pers, &resp );
      }
}

void cr_die ( edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
    self->think = cr_death;
    self->nextthink = level.time + FRAMETIME;

    VectorClear (self->avelocity);

    self->takedamage = DAMAGE_YES;
    self->movetype = MOVETYPE_TOSS;

    self->s.modelindex2 = 0;    // remove linked weapon model
    self->s.modelindex3 = 0;    // remove linked ctf flag

    self->s.angles[0] = 0;
    self->s.angles[2] = 0;

    self->s.sound = 0;
    self->client->weapon_sound = 0;

    self->maxs[2] = -8;

    self->svflags |= SVF_DEADMONSTER;

    if (!self->deadflag) {
      if (attacker && attacker->client && !attacker->bot_info) self->bot_pers->adapt_count++;
      ClientObituary( self, inflictor, attacker );
      CTFFragBonuses(self, inflictor, attacker);
      TossClientWeapon(self);
      CTFPlayerResetGrapple(self);
      CTFDeadDropFlag(self);
      CTFDeadDropTech(self);
      self->client->pers.weapon = NULL;
      memset ( self->client->pers.inventory, 0, sizeof(self->client->pers.inventory) );
      if (random()<0.4f) cr_message_death( self, attacker );
      }

    // remove powerups
    self->client->quad_framenum = 0;
    self->client->invincible_framenum = 0;
    self->client->breather_framenum = 0;
    self->client->enviro_framenum = 0;

    if (self->health < -40) {
      int n;
        
      gi.sound (self, CHAN_VOICE, gi.soundindex ("misc/udeath.wav"), 1, ATTN_NORM, 0);
      for ( n=0; n<3; n++) 
         ThrowGib ( self, rand()&1 ? "models/objects/gibs/bone/tris.md2" :
                                     "models/objects/gibs/sm_meat/tris.md2", damage, GIB_ORGANIC );
      self->takedamage = DAMAGE_NO;
      self->client->anim_priority = ANIM_DEATH;
      self->client->anim_end = 0;
      }
    else {
      if (!self->deadflag) {
        static int i;

        i = (i+1)%3;
        // start a death animation
        self->client->anim_priority = ANIM_DEATH;
        if (self->bot_info->b_crouch) {
          self->s.frame = FRAME_crdeath1-1;
          self->client->anim_end = FRAME_crdeath5;
          }
        else switch (i) {
                   case 0:
                       self->s.frame = FRAME_death101-1;
                       self->client->anim_end = FRAME_death106;
                       break;
                   case 1:
                       self->s.frame = FRAME_death201-1;
                       self->client->anim_end = FRAME_death206;
                       break;
                   case 2:
                       self->s.frame = FRAME_death301-1;
                       self->client->anim_end = FRAME_death308;
                       break;
                       }
        gi.sound (self, CHAN_VOICE, gi.soundindex(va("*death%i.wav", (rand()%4)+1)), 1, ATTN_NORM, 0);
        }
      }

    self->deadflag = DEAD_DEAD;

    gi.linkentity(self);
}


path_node_t* cr_get_next_path_node( edict_t *self )
{ 
    self->bot_info->path_nodes--;
    if (self->bot_info->path_nodes<0) {
      self->bot_info->path_nodes=-1;
      return NULL;
      }
    return self->bot_info->path[self->bot_info->path_nodes];
}

path_node_t* cr_get_current_path_node( edict_t *self )
{ 
    if (self->bot_info->path_nodes<0) return NULL;
    return self->bot_info->path[self->bot_info->path_nodes];
}

path_node_t* cr_find_closest_node ( edict_t *self )
{
    float        d, min_dist, min_idist;
    path_node_t *best, *node, *ibest;
    vec3_t       pos;

    VectorCopy ( self->s.origin, pos );

    best = NULL;
    ibest = NULL;
    min_dist = 1e32;
    min_idist = 1e32;
    for ( node=cr_node_head; node!=NULL; node=node->next ) {
       d = dist2( pos, node->position );
       if (d<min_idist) {
         min_idist=d;
         ibest=node;
         }
       if (d>min_dist) continue;
       if (is_touched(self,node->position)) {
         best = node;
         break;
         }
       if (!pos_reachable(pos,node->position)) continue;
       //if (!can_reach( self, node->position )) continue;
       best = node;
       min_dist = d;
       }

    // can't find any visible nodes :(
    if (!best) best=ibest;

    return best;
}

path_node_t* cr_find_closest_enemy_node( edict_t *self )
{
    path_node_t* node;

    if (!self->enemy) return NULL;

    VectorCopy ( self->s.origin, self->s.old_origin);
    VectorCopy ( self->enemy->s.origin, self->s.origin );
    node = cr_find_closest_node(self);
    VectorCopy ( self->s.old_origin, self->s.origin );
    
    return node;
}

path_node_t* cr_find_closest_node_pos ( edict_t* self, vec3_t pos )
{
    path_node_t* node;

    VectorCopy ( self->s.origin, self->s.old_origin);
    VectorCopy ( pos, self->s.origin );
    node = cr_find_closest_node(self);
    VectorCopy ( self->s.old_origin, self->s.origin );
    
    return node;
}

void cr_register_position( edict_t* self, int flags )
{
    vec3_t       pos;
    path_node_t *node;
    int          n;
    float        d;

    VectorCopy ( self->s.origin, pos );

    if (self->prev_node) {
      if ( is_closer_b( pos, self->prev_node->position, NODE_MIN_DIST ) &&
           pos_reachable( pos, self->prev_node->position ) ) return;
      }

    // must be far enough from all other nodes
    n=0;
    for ( node=cr_node_head; node!=NULL; node=node->next ) {
       d = dist2( pos, node->position ); 
       if (d<50*50) break;
       if (d>(NODE_MAX_DIST*NODE_MAX_DIST*0.5f)) continue;
       if (!pos_reachable( pos, node->position )) continue;
       n++;
       if (d>NODE_MIN_DIST*NODE_MIN_DIST) continue;
       if (n>=2) break;
       }

    if (!node) 
      self->prev_node = cr_insert_node( pos, self->prev_node, flags );
    else 
      self->prev_node = node;
}

void cr_update_routes ( edict_t *self )
{
    vec3_t       pos;

    if (self->watertype & (CONTENTS_LAVA|CONTENTS_SLIME)) return;
    if (!self->groundentity && self->waterlevel==0) return;

    if (node_count>MAX_NODE_COUNT) return;

    VectorCopy ( self->s.origin, pos );

    if (self->bot_info->last_node) {
      if ( is_closer_b( pos, self->bot_info->last_node->position, NODE_MIN_DIST ) &&
           pos_reachable( pos, self->bot_info->last_node->position ) ) return;
      }
    if (self->bot_info->next_node) {
      if ( is_closer_b( pos, self->bot_info->next_node->position, NODE_MIN_DIST ) &&
           pos_reachable( pos, self->bot_info->next_node->position ) ) return;
      }

    cr_register_position( self, self->bot_info->b_on_ladder ? NF_LADDER : 0 );
}

void cr_pick_random_destination( edict_t * self )
{
    float dist, an;

    dist = NODE_MIN_DIST + (NODE_MAX_DIST-NODE_MIN_DIST)*random();
    an = random()*M_PI*2;
    self->bot_info->move_target[0] = self->s.origin[0] + dist*cos(an);
    self->bot_info->move_target[1] = self->s.origin[1] + dist*sin(an);
    self->bot_info->move_target[2] = self->s.origin[2] + STEPSIZE;

   // gi.bprintf ( PRINT_MEDIUM, "roaming to %.1f %.1f %.1f\n", self->bot_info->move_target[0], self->bot_info->move_target[1], self->bot_info->move_target[2] );
}

float cr_pickup_importance ( edict_t *self, edict_t *item_ent  )
{
    float    res = -1;
    gitem_t* item = item_ent->item;

    if (!item) return -1;

    if (item->pickup==CTFPickup_Flag) {
      if (item_ent->spawnflags & DROPPED_ITEM) {
        res = 5000;
        } 
      else {
        if (strcmp(item_ent->classname, "item_flag_team1") == 0) {
          if ( self->client->resp.ctf_team != CTF_TEAM1 ||
               self->client->pers.inventory[INDEX_FLAG2] ) res = 5000;
          }
        else 
        if (strcmp(item_ent->classname, "item_flag_team2") == 0) {
          if ( self->client->resp.ctf_team != CTF_TEAM2 ||
               self->client->pers.inventory[INDEX_FLAG1] ) res = 1100;
          }
        }
      } 
    else
    if (item->pickup==CTFPickup_Tech) {
     //FIXME: teach bot to drop old powerup!
      if ( !self->client->pers.inventory[INDEX_TECH1] && 
           !self->client->pers.inventory[INDEX_TECH2] && 
           !self->client->pers.inventory[INDEX_TECH3] && 
           !self->client->pers.inventory[INDEX_TECH4] ) { 
         switch (item_ent->classname[9]) {
               case '1': res = 1000;
               case '2': res = 1500;
               case '3': res = 1500;
               case '4': res = 2000;
               }
         }
      }
    else 
    if (item->pickup==Pickup_Adrenaline) {
      res = (95-self->health)*4;
      if (res<0) res=-1;
      }
    else
    if (item->pickup==Pickup_Powerup) {
      if (item->use==Use_Quad ||
          item->use==Use_Invulnerability) res=1000;
      else if (item->use==Use_Breather) res=100;
      }
    else
    if (item->pickup==Pickup_PowerArmor) {
      res = (self->flags & FL_POWER_ARMOR) ? 1 : 1000;
      }
    else
    if (item->pickup==Pickup_Health) {
   //== health
      if (item_ent->style & HEALTH_IGNORE_MAX) res=item_ent->count*10;
      else {
        if (self->health >= self->max_health) 
          res=-1;
        else 
          res = cr_low_health(self) ? 20*item_ent->count : 0.2f*item_ent->count*(self->max_health - self->health);
        }
      }
    else
  //== armor
    if (item->pickup==Pickup_Armor) {
      int old_armor_index;
      gitem_armor_t *newinfo;
  
      // get info on new armor
      newinfo = (gitem_armor_t *)item->info;

      old_armor_index = ArmorIndex(self);

      // handle armor shards specially
      if (item->tag == ARMOR_SHARD) {
        res = 5.f;
        }
      else
      if (!old_armor_index) {
        res = 2.f*newinfo->base_count;
        }
      else 
        res = 2.f*(newinfo->base_count - self->client->pers.inventory[old_armor_index]);
      if (cr_low_health(self)) res*=4;
      }
    else
   //== weapon
    if (item->pickup==Pickup_Weapon) {
      int      index, ammo_idx;
      qboolean bWeaponsStay = ((int)(dmflags->value) & DF_WEAPONS_STAY);

      index = ITEM_INDEX(item);

      if ( self->client->pers.inventory[index]>0 ) res = bWeaponsStay ? -1 : 0.5f;
      else
      if ( index>ITEM_INDEX(self->client->pers.weapon) )  {
        ammo_idx = ITEM_INDEX(FindItem(item->ammo));
        if ( self->client->pers.inventory[ammo_idx]>0 ||
             !(item_ent->spawnflags & DROPPED_ITEM) ) 
          res = self->client->pers.weapon == FindItem ("blaster") ? 400 : 100;
        else 
          res = bWeaponsStay ? 2.f : 5.f; // this weapon is better than we have, 
                                          // but we don't have ammo for it yet
        }
      else
        res = 0.5f; // this weapon is worse than we have
      }
    else
   //== ammo
    if (item->pickup==Pickup_Ammo) {
      int  count, cur_count, max=0, mod=1, index;
      
      index = ITEM_INDEX(item);
      if (item_ent->count>0) count = item_ent->count;
                        else count = item->quantity;

      if (item->tag == AMMO_BULLETS)       { max = self->client->pers.max_bullets;  mod=1; if (self->client->pers.inventory[INDEX_MACHINEGUN] || self->client->pers.inventory[INDEX_CHAINGUN]) mod*=2; }
      else if (item->tag == AMMO_SHELLS)   { max = self->client->pers.max_shells;   mod=1; if (self->client->pers.inventory[INDEX_SUPER_SHOTGUN] || self->client->pers.inventory[INDEX_SHOTGUN]) mod*=2; }
      else if (item->tag == AMMO_ROCKETS)  { max = self->client->pers.max_rockets;  mod=3; if (self->client->pers.inventory[INDEX_ROCKET_LAUNCHER]) mod*=2; }
      else if (item->tag == AMMO_GRENADES) { max = self->client->pers.max_grenades; mod=2; if (self->client->pers.inventory[INDEX_GRENADE_LAUNCHER]) mod*=2; }
      else if (item->tag == AMMO_CELLS)    { max = self->client->pers.max_cells;    mod=3; if (self->client->pers.inventory[INDEX_HYPERBLASTER] || self->client->pers.inventory[INDEX_BFG]) mod*=3; }
      else if (item->tag == AMMO_SLUGS)    { max = self->client->pers.max_slugs;    mod=2; if (self->client->pers.inventory[INDEX_RAILGUN]) mod*=2; }

      cur_count = self->client->pers.inventory[index];
      if (cur_count>=max) res = -1;
      else res = mod*10.f*count*(max-cur_count)/sqr(max);
      }
    else 
    if (item->pickup==Pickup_Bandolier) {
      if (self->client->pers.max_bullets < 250) res = 50;
                                           else res =  5;
      }
    else
    if (item->pickup==Pickup_Pack) {
      if (self->client->pers.max_bullets < 300) res = 100;
                                           else res = 10;
      }
//FIXME: support the rest of pickups (items, etc.)

    if (res<=0) res=-1;
    return res;
}

float last_map_search=0;

void cr_find_pickup_on_map( edict_t *self )
{
    path_node_t *node_stack1[MAX_STACK_NODE], *node_stack2[MAX_STACK_NODE];
    path_node_t *node, *link_node, *next_link;
    path_node_t **nodes, **new_nodes;
    int          cur_stack, stack_nodes, new_stack_nodes, i, j, max_nodes, n_count;
    float        cur_dist, best_dist, d, imp, best_score;
    path_node_t *best_node;
    edict_t*     hit;
   // qboolean  b_show=false;

    if (last_map_search==level.time) return;
    last_map_search=level.time;

    self->bot_info->last_node = cr_find_closest_node(self);

    if (self->bot_info->bot_assignment==ASSN_GROUP) return;

   // initialize node network
    for ( node=cr_node_head; node!=NULL; node=node->next ) node->route_dist=-1;

    self->bot_info->last_node->route_dist = 0.001;
    stack_nodes = 1;
    node_stack1[0] = self->bot_info->last_node;
    new_stack_nodes = 1;
    cur_stack = 0;

    max_nodes = 20 + 2*self->bot_pers->skill;
    if (max_nodes>(MAX_PATH_NODES-2)) max_nodes = MAX_PATH_NODES-2;

    best_score = self->bot_info->pickup_target_score;
    best_node =  NULL;
    n_count=0;
    while ( (best_score>TOUCH_DIST || n_count<(max_nodes/2)) && 
            n_count<max_nodes) {
       stack_nodes = new_stack_nodes;
       new_stack_nodes = 0;

       if (cur_stack==0) {
         nodes     = node_stack1;
         new_nodes = node_stack2;
         cur_stack=1;
         }
       else {
         nodes     = node_stack2;
         new_nodes = node_stack1;
         cur_stack=0;
         }

       best_dist = 1e32;
       for ( i=0; i<stack_nodes; i++ ) {
          node = *nodes++;
          cur_dist = node->route_dist;
          if (best_dist>cur_dist) best_dist=cur_dist;

         // go through all links
          for ( j=0; j<MAX_NODE_LINKS; j++ ) {
             link_node = node->link_to[j];
             if ( !link_node ) break;
             d = cur_dist + node->link_dist[j];

             if (link_node->route_dist<0 && new_stack_nodes<MAX_STACK_NODE) {
              // add new downlink node
               *new_nodes = link_node;
               new_nodes++;
               new_stack_nodes++;
               }

             if ( link_node->route_dist<0 || link_node->route_dist>d ) {
              // update route_dist if this route is shorter
               link_node->route_dist = d;
               hit = link_node->item;

               if (!hit) continue;
               if (!hit->inuse) continue;
               if ( (hit->svflags & SVF_NOCLIENT) || hit->solid==SOLID_NOT ) continue;
               if (cr_check_unreachable(self,hit)) continue;
               imp = cr_pickup_importance(self,hit);
               if (imp>0) {
                 d /= imp;
                 if (!best_node || d<best_score) {
                   best_score = d;
                   best_node = link_node;
                   }
                 }
/*
               if (imp>1001) {
                 b_show = true;
                 gi.dprintf( "%s scored %.1f\n", hit->classname, d );
                 } */
               }
             }
          }

       if (!new_stack_nodes) break;
       n_count++;
       }

/*
   // remove path indication
    { edict_t* line, *next_line;
    for ( line = global_path_route; line!=NULL; line=next_line ) {
       next_line = line->teamchain;
       G_FreeEdict(line);
       }
    } */

    if (!best_node) return;

    self->bot_info->path_nodes=-1;
    n_count=0;
    node = best_node;
    while (n_count<=max_nodes) {
        // search for shortest possible distance from "node"
         best_dist=0;
         link_node=NULL;
         for ( i=0; i<MAX_NODE_LINKS; i++ ) {
            if ( !(next_link = node->link_from[i]) ) break;
            d = next_link->route_dist;
            if (d>=0 && (!link_node || best_dist>d) ) {
              link_node = next_link;
              best_dist = d;
              } 
            }

         self->bot_info->path[++self->bot_info->path_nodes]=node;

         if (!link_node || link_node==self->bot_info->last_node) break;

         node->route_dist = -2;

#ifdef _DEBUG
         cr_draw_line( node->position, link_node->position, COLOR_BEAM_BLUE, &global_path_route );
#endif
         node = link_node;
         n_count++;
         }

    self->think = cr_think_pickup;
    self->bot_info->pickup_target = best_node->item;
    self->bot_info->pickup_target_score = best_score;
    self->bot_info->next_node = node;
    cr_set_move_target( self, node->position );

 //   if (b_show) {
 //    gi.bprintf ( PRINT_MEDIUM, "gonna get %s with score %.1f from map\n", self->bot_info->pickup_target->classname, best_score );
 //     }
}

qboolean cr_force_pickup_target(  edict_t *self, edict_t *target )
{
    if (cr_find_route( self, target->s.origin, true )) {
      self->think = cr_think_pickup;
      self->bot_info->pickup_target = target;
      self->bot_info->pickup_target_score = 1e-6f;
      return true;
      }
    return false;
}

void cr_find_pickup_target( edict_t *self )
{
    edict_t *hit, *best=NULL;
    vec3_t   orig;
    float    min_dist, dd, imp;

    if (self->bot_info->time_next_pickup>level.time) return;
    self->bot_info->time_next_pickup = level.time + TIME_NEXT_PICKUP;

    VectorCopy ( self->s.origin, orig );

    min_dist = self->bot_info->pickup_target_score;
    for ( hit=g_edicts+1; hit<&g_edicts[globals.num_edicts]; hit++) {
       if (!hit->inuse) continue;
       if (!hit->item || !hit->item->pickup) continue;
       if ((hit->svflags & SVF_NOCLIENT) || (hit->solid==SOLID_NOT)) continue;
       if (cr_check_unreachable(self,hit)) continue;

       imp = cr_pickup_importance(self,hit);
       if (imp<=0) continue;

       dd = sqrt(sqr(orig[0]-hit->s.origin[0]) + sqr(orig[1]-hit->s.origin[1]) + sqr(orig[2]-hit->s.origin[2]));
       dd /= imp;
       if (dd>min_dist) continue;
       if (!can_reach(self,hit)) continue;

      // gi.bprintf ( PRINT_MEDIUM, "rated %s at %.2f\n", hit->classname, imp );

       best = hit;
       min_dist = dd;
       if (min_dist<TOUCH_DIST) break;
       }

    if (best) {
      self->bot_info->pickup_target = best;
      self->bot_info->pickup_target_score = min_dist;
      cr_set_move_target(self,self->bot_info->pickup_target->s.origin);
      }
}

qboolean cr_find_pickup_urgent( edict_t *self )
{
    edict_t *hit, *best=NULL;
    vec3_t   orig;
    float    min_dist, dd, imp, pickup_range;
    float    bottom_limit, top_limit;

    if (self->bot_info->time_next_pickup>level.time) return false;
    self->bot_info->time_next_pickup = level.time + TIME_NEXT_PICKUP;

    VectorCopy ( self->s.origin, orig );
    top_limit    = orig[2] + self->maxs[2]*0.9f;
    bottom_limit = orig[2] + self->mins[2]*0.9f;

    pickup_range = PICKUP_RANGE_MIN + (PICKUP_RANGE_MAX - PICKUP_RANGE_MIN)*0.1f*self->bot_pers->skill;
    if (self->client->pers.weapon->weaponthink == Weapon_Blaster) pickup_range *= 3;

    min_dist = 1e32;//self->bot_info->pickup_target_score;
    for ( hit=g_edicts+1; hit<&g_edicts[globals.num_edicts]; hit++) {
       if (!hit->inuse) continue;
       if (!hit->item || !hit->item->pickup) continue;
       if ((hit->svflags & SVF_NOCLIENT) || (hit->solid==SOLID_NOT)) continue;
       if (cr_check_unreachable(self,hit)) continue;

       if ( hit->s.origin[2]<bottom_limit || 
            hit->s.origin[2]>top_limit ) continue;

       imp = cr_pickup_importance(self,hit);
       if (imp<=0) continue;

       dd = sqrt(sqr(orig[0]-hit->s.origin[0]) + sqr(orig[1]-hit->s.origin[1]) + sqr(orig[2]-hit->s.origin[2]));
       if (dd>pickup_range) continue;
       dd /= imp;
      // gi.bprintf ( PRINT_MEDIUM, "%s rated with %.2f score\n", hit->classname, dd );

       if (dd>min_dist) continue;

       if (!can_reach(self,hit)) continue;
      // if (!can_reach(self,hit->s.origin)) continue;

       best = hit;
       min_dist = dd;
       if (min_dist<TOUCH_DIST) break;
       }

    if (min_dist>pickup_range) return false;

    if (best) {
    // gi.bprintf ( PRINT_MEDIUM, "going for %s with %.2f score\n", best->classname, min_dist );
      self->bot_info->pickup_target = best;
      self->bot_info->pickup_target_score = min_dist;
      cr_set_move_target( self, self->bot_info->pickup_target->s.origin );
      return true; 
      }

    return false;
}


qboolean cr_vertical_ok ( edict_t* self )
{
    return self->bot_info->next_node && 
           ( (self->bot_info->next_node->flags & NF_ELEVATOR) ||
             (self->bot_info->next_node->flags & NF_LADDER) );
}

qboolean cr_wait_ok ( edict_t* self )
{
    return self->think==cr_think_wait  || self->think==cr_think_salute ||
           self->think==cr_think_taunt || self->think==cr_think_flip ||
           self->think==cr_think_attack ||
           ( self->bot_info->next_node && 
             ( (self->bot_info->next_node->flags & NF_DOOR) ||
               (self->bot_info->next_node->flags & NF_ELEVATOR) ) );
}

qboolean cr_no_way( edict_t *self, vec3_t spot )
{
    float dd;

    if (cr_vertical_ok(self)) return false;
    if (!self->groundentity && spot[2]<self->s.origin[2]) return false;

    dd = dist( self->s.origin, spot );
    if (dd<TOUCH_DIST) return false;

    return is_touched2d( self, spot );
}

void cr_crouch( edict_t* self, qboolean bCrouch ) 
{
    if (bCrouch) {
      if (self->bot_info->b_crouch) return;

      self->maxs[2] -= CROUCH_DELTA;
      self->bot_info->b_crouch = true;
      }
    else {
      if (!self->bot_info->b_crouch) return;

      self->maxs[2] += CROUCH_DELTA;
      self->bot_info->b_crouch = false;
      }

   // gi.linkentity(self);
}


qboolean cr_check_bottom( edict_t *self )
{
    vec3_t  mins, maxs, start, stop;
    trace_t trace;
    int     x, y, fail_count;
    float   mid;
    
    VectorAdd (self->s.origin, self->mins, mins);
    VectorAdd (self->s.origin, self->maxs, maxs);

// if all of the points under the corners are solid world, don't bother
// with the tougher checks
// the corners must be within 16 of the midpoint
    start[2] = mins[2] - 1;
    for (x=0 ; x<=1 ; x++)
        for (y=0 ; y<=1 ; y++) {
           start[0] = x ? maxs[0] : mins[0];
           start[1] = y ? maxs[1] : mins[1];
           if (gi.pointcontents (start) != CONTENTS_SOLID)
             goto realcheck;
           }

    return true;        // we got out easy

realcheck:
//
// check it for real...
//
    start[2] = mins[2];
    
    start[0] = stop[0] = (mins[0] + maxs[0])*0.5;
    start[1] = stop[1] = (mins[1] + maxs[1])*0.5;
    stop[2] = start[2] - (STEPSIZE + JUMP_HEIGHT);
    trace = gi.trace( start, vec3_origin, vec3_origin, stop, self, MASK_PLAYERSOLID );

    if (trace.fraction == 1.f) return false;
    mid = trace.endpos[2];
    
// the corners must be within 16 of the midpoint    
    fail_count=0;
    for (x=0 ; x<=1 ; x++)
        for (y=0 ; y<=1 ; y++) {
           start[0] = stop[0] = x ? maxs[0] : mins[0];
           start[1] = stop[1] = y ? maxs[1] : mins[1];
           
           trace = gi.trace( start, vec3_origin, vec3_origin, stop, self, MASK_PLAYERSOLID );
           
           if ( trace.fraction==1.f || (mid - trace.endpos[2]) > JUMP_HEIGHT ) {
             fail_count++;
             if (fail_count>2) return false;
             }
           }

    return true;
}

qboolean cr_try_move ( edict_t *self, float d_yaw, qboolean bCheckFall )
{
    vec3_t   move, oldorg, start, end;
    trace_t  trace, trace_lava;
    float    dist, yaw;

    if (!self->groundentity) return true; 

    dist = FRAMETIME*self->bot_pers->speed;
         if (self->bot_info->b_crouch) dist *= CROUCH_SPEED_COEF;
    else if (self->waterlevel>1) dist *= WATER_SPEED_COEF;
    
    yaw = (self->ideal_yaw + d_yaw)*M_PI*2/360;

    move[0] = dist*cos(yaw);
    move[1] = dist*sin(yaw);
    move[2] = 0;

    VectorScale ( move, FRAMETIME, self->velocity );
    VectorCopy ( self->velocity, self->movedir );

  // try the move 
    VectorCopy ( self->s.origin, oldorg );
    VectorAdd ( self->s.origin, move, start );

    VectorCopy ( start, end );
    start[2] += STEPSIZE;
    end[2] -= /*STEPSIZE +*/ JUMP_HEIGHT;

    trace = gi.trace ( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );

    if (trace.allsolid) return false;

    if (trace.startsolid) {
      start[2] -= STEPSIZE;
      trace = gi.trace ( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );
      if (trace.allsolid) return false;
      if (trace.startsolid) {
        if (self->bot_info->b_crouch) return false;
       // can we crouch?!
        trace = gi.trace ( start, self->mins, 
                           tv( self->maxs[0], self->maxs[1], self->maxs[2]-CROUCH_DELTA), 
                           end, self, MASK_PLAYERSOLID );
        if (trace.allsolid || trace.startsolid) return false;
        cr_crouch(self,true);
        }
      }

    if (bCheckFall && !(trace.contents & CONTENTS_WATER)) {
      if (trace.fraction == 1.f) return false; // walked off an edge

     // check point traces down for dangling corners
      VectorCopy ( trace.endpos, self->s.origin );

      if (!cr_check_bottom(self)) {
        VectorCopy ( oldorg, self->s.origin );
        return false;
        }
      }

    VectorScale ( move, 1.1f, move );
    VectorAdd ( oldorg, move, start );
    VectorCopy( start, end );
    end[2] -= 300.f;
    trace_lava = gi.trace ( start, vec3_origin, vec3_origin, end, self, MASK_PLAYERSOLID | CONTENTS_LAVA | CONTENTS_SLIME );
    if (trace_lava.fraction>0.99f || trace_lava.contents & (CONTENTS_LAVA|CONTENTS_SLIME)) {
      VectorCopy ( oldorg, self->s.origin );
      return false;
      }

    VectorCopy ( trace.endpos, self->s.origin );

    self->groundentity = trace.ent;
    self->groundentity_linkcount = trace.ent->linkcount;

/*!!!
// double-check if inside something
    VectorCopy ( self->s.origin, start );
    VectorCopy ( start, end );
    end[2] += 0.25f;

    trace = gi.trace ( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );

    if (trace.allsolid || trace.startsolid) {
      VectorCopy ( oldorg, self->s.origin );
      return false; 
      }
*/

    self->ideal_yaw = anglemod( self->ideal_yaw + d_yaw );

// the move is ok
    gi.linkentity (self);

    return true;
}

qboolean cr_can_stand( edict_t* self )
{
    vec3_t  start, end;
    trace_t trace;

    if (!self->bot_info->b_crouch) return false;
    if ( self->bot_info->time_next_crouch > level.time ) return false;
    self->bot_info->time_next_crouch = level.time + TIME_CROUCH;

    VectorCopy ( self->s.origin, start );
    VectorCopy ( start, end );
    end[2] += 0.25f;
   // end[2] += CROUCH_DELTA;

    self->maxs[2] += CROUCH_DELTA;
    trace = gi.trace ( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );
    self->maxs[2] -= CROUCH_DELTA;

    if (trace.startsolid || trace.allsolid || trace.fraction<0.99) return false;

    return true;
}

void cr_jump( edict_t *self )
{
    if (!self->groundentity) return;
    if (self->bot_info->b_crouch || self->bot_info->b_on_ladder) return;

    YawVector( self->ideal_yaw, self->velocity );
    self->velocity[0] *= BOT_MAX_SPEED;
    self->velocity[1] *= BOT_MAX_SPEED;
    self->velocity[2] = BOT_JUMP_SPEED;
    VectorCopy( self->velocity, self->movedir );
    self->groundentity = NULL;
    gi.sound( self, CHAN_VOICE, gi.soundindex("*jump1.wav"), 1, ATTN_NORM, 0);
    self->s.frame = FRAME_jump1;
}

qboolean cr_can_jump ( edict_t *self )
{
    vec3_t  move, neworg, start, end, oldorg;
    trace_t trace, trace_lava;
    float   cs, sn, yaw;

    if (self->bot_info->b_crouch || self->bot_info->b_on_ladder) return false;
    if (self->bot_info->time_next_jump>level.time) return false;
    
    yaw = self->ideal_yaw*M_PI*2/360;
    cs = cos(yaw);
    sn = sin(yaw);

    move[0] = MOVE_JUMP_DIST*cs;
    move[1] = MOVE_JUMP_DIST*sn;
    move[2] = 0;

  // try the move 
    VectorAdd ( self->s.origin, move, neworg );

    VectorCopy ( neworg, end );
    neworg[2] += JUMP_HEIGHT;
    end[2]    -= STEPSIZE;

    trace = gi.trace ( neworg, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );

    if (trace.allsolid || trace.startsolid) return false;

    neworg[0] -= move[0];
    neworg[1] -= move[1];

    end[0] = neworg[0] + JUMP_DIST*cs; 
    end[1] = neworg[1] + JUMP_DIST*sn;
    end[2] = neworg[2];

    trace = gi.trace ( neworg, self->mins, self->maxs, end, self, MASK_PLAYERSOLID | CONTENTS_LADDER );
    if (trace.allsolid || trace.startsolid) return false;
    if (trace.contents & CONTENTS_LADDER) return true;

    VectorCopy ( trace.endpos, neworg );
    VectorCopy ( neworg, end );
    end[2] -= 300.f;
    trace = gi.trace ( neworg, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );
    if (trace.allsolid || trace.startsolid || trace.fraction>0.99f) return false;

    if ( trace.endpos[2]<(self->bot_info->move_target[2]-STEPSIZE) ) return false;

   // check for solid ground
    VectorCopy ( self->s.origin, oldorg );
    VectorCopy ( trace.endpos, self->s.origin );
    if (!cr_check_bottom(self)) {
      VectorCopy ( oldorg, self->s.origin );
      return false;
      }
    VectorCopy ( oldorg, self->s.origin );

    VectorCopy( trace.endpos, start );
    VectorCopy( start, end );
    end[2] -= 300.f;
    trace_lava = gi.trace ( start, vec3_origin, vec3_origin, end, self, MASK_PLAYERSOLID | CONTENTS_LAVA | CONTENTS_SLIME  );
    if (trace_lava.fraction>0.99f || trace_lava.contents & (CONTENTS_LAVA|CONTENTS_SLIME)) return false;

    return true;
}

void cr_effect( edict_t *self, int eff )
{
    gi.WriteByte  ( svc_muzzleflash );
    gi.WriteShort ( self - g_edicts );
    gi.WriteByte  ( eff );
    gi.multicast  ( self->s.origin, MULTICAST_PVS );
}

qboolean cr_try_rocketjump( edict_t *self )
{
    edict_t* pt;
    float    d, max_importance;
    vec3_t   move, start, end, oldorg, aim;
    trace_t  trace;
    int      armor_index;

    if ( !self->groundentity || self->bot_info->b_crouch || 
         self->bot_info->time_next_jump>level.time) return false;
    if ( self->client->invincible_framenum < level.framenum &&
         (self->health<80.f || self->client->breather_framenum > level.framenum) ) return false;

    if ( self->bot_info->time_next_shot > (level.time-TIME_WEAPON_CHANGE) ||
         self->client->pers.inventory[INDEX_ROCKETS]<1 ||
         self->client->pers.inventory[INDEX_ROCKET_LAUNCHER]<1 ) return false;

    pt = self->bot_info->pickup_target;
    if ( !pt || pt->s.origin[2]<(self->s.origin[2]+JUMP_HEIGHT) ||
                pt->s.origin[2]>(self->s.origin[2]+ROCKETJUMP_HEIGHT) ) return false;

    d = dist2d2(pt->s.origin,self->s.origin);
    if ( d>ROCKETJUMP_MAXDIST*ROCKETJUMP_MAXDIST || 
         d<ROCKETJUMP_MINDIST*ROCKETJUMP_MINDIST) return false;

    armor_index = ArmorIndex(self);
    if (!armor_index) max_importance = 600;
                 else max_importance = 600 - 4*self->client->pers.inventory[armor_index];

    if (cr_pickup_importance(self,pt)<max_importance) return false;
    
    VectorSubtract( pt->s.origin, self->s.origin, move );
    move[2]=0;
    VectorNormalize( move );
    VectorScale( move, ROCKETJUMP_DIST, move );

    VectorCopy( self->s.origin, start );
    VectorAdd( start, move, end );
    end[2] += ROCKETJUMP_HEIGHT;

   // cr_draw_line( start, end, COLOR_BEAM_GREEN, &global_path_route );

    trace = gi.trace ( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );
    if (trace.allsolid || trace.startsolid) return false;

    if (trace.fraction==1.f) {
     // continue the jump
      VectorCopy( end, start );
      VectorAdd( start, move, end );
      end[2] -= ROCKETJUMP_HEIGHT-JUMP_HEIGHT;

     // cr_draw_line( start, end, COLOR_BEAM_GREEN, &global_path_route );

      trace = gi.trace ( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );
      if ( trace.allsolid || trace.startsolid || trace.fraction>0.9f ) return false;
      }

    if (trace.endpos[2]<(pt->s.origin[2]-STEPSIZE)) return false;

  // cast a ray down
    VectorCopy( trace.endpos, start );
    VectorCopy( start, end );
    end[2] = pt->s.origin[2]-JUMP_HEIGHT;

   // cr_draw_line( start, end, COLOR_BEAM_BLUE, &global_path_route );

    trace = gi.trace ( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID | CONTENTS_LAVA | CONTENTS_SLIME );
    if ( trace.allsolid || trace.startsolid || trace.fraction==1.f || 
         trace.contents & (CONTENTS_LAVA|CONTENTS_SLIME) ) return false;

    VectorCopy ( self->s.origin, oldorg );
    VectorCopy ( trace.endpos, self->s.origin );
    if (!cr_check_bottom(self)) {
      VectorCopy ( oldorg, self->s.origin );
      return false;
      }
    VectorCopy ( oldorg, self->s.origin );

  // victory!
    VectorCopy ( pt->s.origin, self->bot_info->move_target );
    self->ideal_yaw = vectoyaw(move);
    cr_jump(self);

  // fire a rocket down below!
    VectorSet( aim, 0,0,-1);
    fire_rocket ( self, self->s.origin, aim, 
                  ROCKET_DAMAGE, ROCKET_SPEED, 120/*damage_rad*/, ROCKET_DAMAGE/*radius_damage*/ );
    cr_effect( self, MZ_ROCKET );
    self->bot_info->time_next_shot = level.time + TIME_WEAPON_CHANGE;
    self->client->pers.inventory[INDEX_ROCKETS]--;

    return true;
}

qboolean cr_try_jump ( edict_t *self, float d_yaw )
{
    float oldyaw;

    oldyaw = self->ideal_yaw;
    self->ideal_yaw = anglemod( self->ideal_yaw + d_yaw );

    if (cr_can_jump(self)) {
      cr_jump(self);
      return true;
      }

    self->ideal_yaw = oldyaw;

    return false;
}

qboolean cr_move_avoid ( edict_t *self, qboolean bSafe )
{
    qboolean bRes = true;

    if (!self->groundentity) return false;

    if (!bSafe && self->bot_info->b_on_platform) bSafe = true;

    if (self->bot_pers->skill>3) cr_avoid_rocket(self);
    if (self->bot_pers->skill>5 && cr_try_rocketjump(self)) return false;

  // jump on ladder
    if ( self->bot_info->next_node && (self->bot_info->next_node->flags & NF_LADDER) ) {
      if ( dist2d2( self->s.origin, self->bot_info->next_node->position ) < sqr(JUMP_DIST*0.6f) ) {
        cr_jump(self);
        return false;
        }
      }

    if (!cr_try_move( self, 0, bSafe )) {
      float dy;
      qboolean bReverse;

      if (cr_try_jump( self, 0 )) return false;

      bRes = false;
      bReverse = random()>0.5f;
      for ( dy=30; ; dy+=30 ) {
         if (bReverse) dy = -dy;
         if (cr_try_move( self,  dy, bSafe )) break;
         if (cr_try_jump( self,  dy )) return false;
         if (cr_try_move( self, -dy, bSafe )) break; 
         if (cr_try_jump( self, -dy )) return false;
         if (bReverse) dy = -dy;
         if (dy>90) break;
         }
      }

    if (self->bot_info->b_crouch && cr_can_stand( self )) {
      cr_crouch(self,false);
      }

    return bRes;
}

qboolean cr_swim( edict_t *self )
{
    vec3_t  start, end, normal, velocity, end_pos;
    trace_t trace;
    int     x, y;
    float   d;

    VectorSubtract ( self->bot_info->move_target, self->s.origin, self->velocity );
    VectorNormalize ( self->velocity );
    VectorScale ( self->velocity, self->bot_pers->speed*WATER_SPEED_COEF, self->velocity );
    VectorCopy ( self->velocity, velocity );

    VectorMA( self->s.origin, FRAMETIME, velocity, end_pos );

   // check if we can go where we want
    VectorCopy( self->s.origin, start );
    VectorCopy( end_pos, end );
    trace = gi.trace( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );
    if (!trace.allsolid && !trace.startsolid && trace.fraction==1.f) {
     // check that we are not leaving water by this move
      end[2] += self->mins[2];
      if ( !(gi.pointcontents(end) & (CONTENTS_WATER|CONTENTS_SOLID)) ) {
        self->velocity[2] = 0;
        VectorNormalize ( self->velocity );
        VectorScale ( self->velocity, self->bot_pers->speed*WATER_SPEED_COEF, self->velocity );
        return false;    
        }
      return true;
      }
    VectorCopy ( trace.plane.normal, normal );

   // check if we can go out of water
    VectorCopy( end_pos, end );
    VectorCopy( end, start );
    start[2] += 2*STEPSIZE;

    trace = gi.trace( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );
    if (trace.startsolid) {
      start[2] -= STEPSIZE;
      trace = gi.trace( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );
      }
    if ( !trace.startsolid && !trace.allsolid && trace.fraction<0.99f &&
         !(gi.pointcontents(trace.endpos) & CONTENTS_WATER) ) {
     // yes, we can get out of the water!
      VectorCopy ( trace.endpos, self->s.origin );
      gi.linkentity(self);
      return true;
      }

    d = DotProduct( normal, velocity );
    if ( fabs(d)>0.9f*self->bot_pers->speed*WATER_SPEED_COEF ) {
      vec3_t right, up, angles;

      VectorCopy( end_pos, end );

      vectoangles ( velocity, angles );
      AngleVectors ( angles, NULL, right, up );

      VectorScale( right, (fabs(self->maxs[0])+fabs(self->maxs[0]))/2, right );
      VectorScale( up, (fabs(self->maxs[2])+fabs(self->maxs[2]))/2, up );
      for ( x=-1; x<=1; x++ ) 
         for ( y=-1; y<=1; y++ ) {
            if ( x==0 && y==0 ) continue;
            VectorCopy( end, start );
            if (x) VectorMA( start, x, right, start );
            if (y) VectorMA( start, y, up, start );
            if ( !(gi.pointcontents(start) & CONTENTS_SOLID) ) {
              VectorSubtract ( end, self->s.origin, self->velocity );
              VectorMA ( self->velocity, y, up, self->velocity );
              VectorNormalize ( self->velocity );
              VectorScale ( self->velocity, self->bot_pers->speed*WATER_SPEED_COEF, self->velocity );
              return true;
              }
            }
      }

    VectorMA( velocity, -0.6f*d, normal, self->velocity );
    VectorNormalize( self->velocity );
    VectorScale( self->velocity, self->bot_pers->speed*WATER_SPEED_COEF, self->velocity );

    return false;
}

qboolean cr_move( edict_t *self, qboolean bFacing, qboolean bSafe )
{
    qboolean bStuck;

    if (bFacing) M_ChangeYaw (self);

    if (self->bot_info->b_on_ladder) {
      vec3_t  start, end;
      trace_t trace;

      VectorSubtract ( self->bot_info->move_target, self->s.origin, self->velocity );
      VectorNormalize( self->velocity );
      VectorScale( self->velocity, 200, self->velocity );
      VectorCopy ( self->velocity, self->movedir );

      VectorCopy ( self->s.origin, start );
      VectorCopy ( start, end );
      end[2] += STEPSIZE; 
      trace = gi.trace ( start, self->mins, self->maxs, end, self, MASK_PLAYERSOLID );
      if (!trace.allsolid && !trace.startsolid && trace.fraction==1.f) {
        VectorCopy ( end, self->s.origin );
        gi.linkentity( self );
        self->bot_info->time_last_stuck=0;
        return true;
        }
     // we're stuck!
      if (!self->bot_info->time_last_stuck) {
        self->bot_info->time_last_stuck=level.time;
        return true;
        }
      return (level.time - self->bot_info->time_last_stuck)<3*TIME_STUCK;
      } 

    if ( self->waterlevel>2 || 
         (self->waterlevel && !self->groundentity)) {
      if (self->bot_info->b_crouch) cr_crouch(self,false);
      bStuck = !cr_swim(self);
      if (bStuck) {
        if (!self->bot_info->time_last_stuck) {
          self->bot_info->time_last_stuck=level.time;
          return true;
          }
        return (level.time - self->bot_info->time_last_stuck)<3*TIME_STUCK;
        }
      self->bot_info->time_last_stuck=0;
      return true;
      }

    if (!self->groundentity) {
      if ( is_touched2d( self, self->bot_info->move_target ) ) {
        VectorClear(self->movedir); 
        }
      self->velocity[0] = self->movedir[0];
      self->velocity[1] = self->movedir[1];
      return true;
      }
   // we hit the slope!
    if (self->bot_info->b_on_slope) {
      self->bot_info->b_on_slope = false;
      self->bot_info->time_last_stuck = 0;
      return false;
      }

    bStuck = !cr_move_avoid( self, bSafe ); 

    if (bStuck) {
      self->bot_info->move_block_count=0;
      // can't move
      if (!self->bot_info->time_last_stuck) {
        self->bot_info->time_last_stuck=level.time;
        return true;
        }
      else
      if ( (level.time - self->bot_info->time_last_stuck) > 
           (cr_vertical_ok(self) ? 10*TIME_STUCK : (cr_wait_ok(self) ? 5*TIME_STUCK : TIME_STUCK) ) ) {
       // gi.bprintf ( PRINT_MEDIUM, "%s reporting stuck!\n", self->client->pers.netname );
        self->bot_info->time_last_stuck=0;
        return false;
        }
      else
        return true;
      }

    if (self->bot_info->move_block_count>1) 
      self->bot_info->time_last_stuck=0;
    else 
      self->bot_info->move_block_count++;
    return true;
}

qboolean cr_moveto( edict_t *self )
{
    vec3_t move;
    float  dt;

    VectorSubtract ( self->bot_info->move_target, self->s.origin, move );

    if (!is_closer_b( self->bot_info->move_target, self->bot_info->last_move_target, 1 )) {
      VectorCopy ( self->bot_info->move_target, self->bot_info->last_move_target );
      dt =  0.5f + 1.2f*VectorLength(move)/self->bot_pers->speed;
      if (self->waterlevel>1) dt /= WATER_SPEED_COEF;
      else if (self->bot_info->b_crouch) dt /= CROUCH_SPEED_COEF;
      self->bot_info->time_last_move_target = level.time + dt;
     // gi.dprintf( "%s has %.1f sec to cover %.1f distance!\n", self->client->pers.netname, dt, VectorLength(move) );
      }
    else {
      if (level.time > self->bot_info->time_last_move_target) {
       // gi.dprintf( "time is up for %s!\n", self->client->pers.netname );
        return false;
        }
      }

    self->ideal_yaw = vectoyaw(move);
    return cr_move( self, true, (self->bot_info->move_target[2]-self->s.origin[2])>-STEPSIZE );
}

qboolean cr_move_yaw( edict_t *self, qboolean bFacing, qboolean bSafe )
{
    vec3_t move;

    YawVector( self->ideal_yaw, move );
    VectorMA( self->s.origin, 80, move, self->bot_info->move_target );
    self->bot_info->move_target[2] += STEPSIZE;
    if (self->waterlevel==1) self->bot_info->move_target[2] -= 2*STEPSIZE;
    return cr_move( self, bFacing, bSafe );
}

void cr_update_enemy( edict_t *self )
{
    if (!self->enemy) return;

    if ( !self->enemy->inuse ||
         self->enemy->deadflag>=DEAD_DYING || !visible(self,self->enemy)) {
     // we lost him!  
      cr_forget_enemy(self);
      return;
      } 

    VectorCopy( self->enemy->s.origin, self->monsterinfo.last_sighting );
    self->monsterinfo.trail_time = level.time;
}

void cr_try_attack( edict_t *self )
{
    if (!self->enemy) return;

    if ( distance(self, self->enemy)<self->bot_pers->attack_range &&
         fabs(self->s.origin[2]-self->enemy->s.origin[2])<(self->bot_pers->attack_range*0.5f) ) {
      self->think = cr_think_attack;
      }
}

void cr_check_ground( edict_t *self )
{
    vec3_t      point;
    trace_t     trace;
    qboolean    was_on_ladder;

    self->bot_info->b_on_platform = false;
    was_on_ladder = self->bot_info->b_on_ladder;
    self->bot_info->b_on_ladder = false;

    if (self->velocity[2] > 200) {
      self->groundentity = NULL;
      return;
      }

   // check if we are on ladder
    if ( self->deadflag!=DEAD_DEAD && 
         self->bot_info->move_target[2]>self->s.origin[2] ) {
      vec3_t  start, end;

      if (!was_on_ladder) YawVector( self->ideal_yaw, self->bot_info->ladder_dir );
      VectorCopy( self->s.origin, start );
      VectorMA ( start, was_on_ladder ? 60 : 50, self->bot_info->ladder_dir, end );
      trace = gi.trace ( start, self->mins, self->maxs, end, self, CONTENTS_LADDER | CONTENTS_SOLID );
      if ( trace.contents & CONTENTS_LADDER ) {
        self->bot_info->b_on_ladder = true;
        if (!was_on_ladder) {
         // face the ladder!
          VectorScale ( trace.plane.normal, -1, self->bot_info->ladder_dir ); 
          }
       // gi.dprintf("%.1f %.1f %.1f \n", trace.plane.normal[0], trace.plane.normal[1], trace.plane.normal[2] );
        }
      else {
        if (was_on_ladder) {
          VectorScale( self->bot_info->ladder_dir, 150, self->velocity );
          self->velocity[2] = 250;
          VectorCopy ( self->velocity, self->movedir );
          }
        }
      }

// if the hull point one-quarter unit down is solid the entity is on ground
    point[0] = self->s.origin[0];
    point[1] = self->s.origin[1];
    point[2] = self->s.origin[2] - 0.25;

    trace = gi.trace ( self->s.origin, self->mins, self->maxs, point, self, MASK_PLAYERSOLID );

    // check steepness
    if ( trace.plane.normal[2] < 0.7 && !trace.startsolid) {
      self->groundentity = NULL;

      if (trace.plane.normal[0]!=0 || trace.plane.normal[1]!=0) {
        VectorScale( trace.plane.normal, 100, self->velocity );
        VectorCopy ( self->velocity, self->movedir );
        self->bot_info->b_on_slope = true;
        }
      return;
      }

    if (!trace.startsolid && !trace.allsolid) {
      VectorCopy (trace.endpos, self->s.origin);
      self->groundentity = trace.ent;
      self->groundentity_linkcount = trace.ent->linkcount;
      self->velocity[2] = 0;
      }

    if (self->groundentity && self->groundentity->blocked)
      self->bot_info->b_on_platform = true;

}

void cr_skip_enemy( edict_t *self )
{
    if (!self->enemy) return;
    cr_add_unreachable( self, self->enemy, UNREACHABLE_ENEMY_TIMEOUT );
    cr_forget_enemy(self);
}

void cr_skip_pickup_target( edict_t *self )
{
    if (self->bot_info->pickup_target) {
      cr_add_unreachable( self, self->bot_info->pickup_target, 
        strcmp( self->bot_info->pickup_target->classname, "item_flag_team1" )==0 ||
        strcmp( self->bot_info->pickup_target->classname, "item_flag_team2" )==0 ?
        UNREACHABLE_TIMEOUT/3 : UNREACHABLE_TIMEOUT );
      }
    cr_forget_pickup_target( self );
}

void cr_do_physics( edict_t* self )
{
    cr_check_ground (self);

    if (self->bot_info->b_airborn && self->groundentity) {
      self->bot_info->time_next_jump = level.time + JUMP_DELAY;
      }
    self->bot_info->b_airborn = !self->groundentity;
}

void cr_catagorize_position (edict_t *self)
{
    vec3_t      point;
    int         cont;

// get waterlevel
    point[0] = self->s.origin[0];
    point[1] = self->s.origin[1];
    point[2] = self->s.origin[2] + self->mins[2] + 1;
    cont = gi.pointcontents (point);

    if ( !(cont & CONTENTS_SOLID) ) 
      self->bot_info->time_next_solid = level.time+1.f;
    else {
      if ( self->bot_info->time_next_solid < level.time ) {
        self->bot_info->time_next_solid = level.time+1.f;
        T_Damage ( self, self, self, vec3_origin, self->s.origin, vec3_origin, 20, 0, 0, MOD_SUICIDE );
       // gi.dprintf( "%s is in solid!\n", self->client->pers.netname );
        // FIXME: push bot along normal!
        if (cont & MASK_WATER) {
          self->s.origin[2] -= 0.2f; 
          gi.linkentity(self);
          }
        } 
      }

    if (!(cont & MASK_WATER)) {
      self->waterlevel = 0;
      self->watertype = 0;
      return;
      }

    self->watertype = cont;
    self->waterlevel = 1;
    point[2] += 24;
    cont = gi.pointcontents (point);
    if (!(cont & MASK_WATER)) return;

    self->waterlevel = 2;
    point[2] += 22;
    cont = gi.pointcontents (point);
    if (cont & MASK_WATER) self->waterlevel = 3;
}

void cr_update_environment( edict_t *self )
{
    cr_do_physics(self);
    cr_catagorize_position (self); 
}

void cr_check_stuck( edict_t *self )
{
    if ( level.time < self->bot_info->time_stuck_check ) return;

    if ( is_closer_b( self->s.origin, self->bot_info->old_origin, 5 ) &&
         !cr_vertical_ok(self) && !cr_wait_ok(self)) {
      self->ideal_yaw = 360*random();
      YawVector( self->ideal_yaw, self->velocity );
      self->bot_info->stuck_count++;
      if (self->bot_info->stuck_count>4) cr_jump(self);
      }
    else 
      self->bot_info->stuck_count=0;

    if (self->bot_info->stuck_count>8) {
      T_Damage ( self, self, self, vec3_origin, self->s.origin, vec3_origin, 20, 0, 0, MOD_SUICIDE );
      self->bot_info->stuck_count=0;
     // gi.bprintf( PRINT_MEDIUM, "%s punished at %.2f %.2f %.2f\n", self->client->pers.netname, self->s.origin[0], self->s.origin[1], self->s.origin[2] );
      }

    VectorCopy ( self->s.origin, self->bot_info->old_origin );
    self->bot_info->time_stuck_check = level.time + TIME_CHECK_STUCK;
}

qboolean cr_update( edict_t *self, qboolean bTryEnemy )
{
    if (!self->inuse) return false;
    self->nextthink = level.time + FRAMETIME;

    if (self->bot_pers->b_adapting) {
      if (self->bot_pers->adapt_count<-1 && self->bot_pers->skill>1) {
        self->bot_pers->skill--;
        cr_compute_skills(self,self->bot_pers->skill);
        }
      if (self->bot_pers->adapt_count>1 && self->bot_pers->skill<10) {
        self->bot_pers->skill++;
        cr_compute_skills(self,self->bot_pers->skill);
        }
      }

    cr_update_environment(self);

    if (bTryEnemy) {
      cr_update_enemy(self);
      if ( cr_low_health(self) ) {
        if ( self->think==cr_think_attack ||
             self->think==cr_think_chase ||
             self->think==cr_think_chase_route ) {
          self->think = cr_think_run_for_life;
          }
        }
      else {
        cr_try_attack(self);
        } 
      }

    cr_update_routes(self);
    cr_check_stuck(self);

    return true;
}

void cr_fire_and_run( edict_t *self )
{
    qboolean bFire;

    bFire = false;
   //FIXME: face the enemy!
    if (self->enemy) {
      if (cr_infront(self,self->enemy)) 
        bFire = cr_fire_weapon( self, self->enemy );
      }
    else {
      edict_t *enemy, *best_enemy=NULL;
      float    dist, best_dist=1e10;
      // do a breaf search for enemy
      for ( enemy=g_edicts+1; enemy<=(g_edicts+game.maxclients); enemy++) {
         if (!enemy->inuse || enemy==self || (enemy->flags & FL_NOTARGET)) continue;

        // attack only bots from other teams, unless we are in team #0
         if ( self->client->pers.team_no>0 && 
              enemy->client->pers.team_no==self->client->pers.team_no ) continue;

        // is it dead already?
         if ( enemy->deadflag>DEAD_DYING || (enemy->svflags & SVF_NOCLIENT) ) continue;

        // can we see him?
         if (!cr_infront (self,enemy)) continue;
         if (!can_reach(self,enemy)) continue;

         dist = dist2(self->s.origin,enemy->s.origin);
         if (dist>ATTACK_RANGE_MAX*ATTACK_RANGE_MAX) continue;

         if (!best_enemy || dist<best_dist ) {
           best_enemy = enemy;
           best_dist = dist;
           if (best_dist<MELEE_COMBAT_DIST*MELEE_COMBAT_DIST) break;
           }
         }
      if (best_enemy) {
        bFire = cr_fire_weapon( self, best_enemy );
        }
      }

    if (bFire) {
      self->client->anim_end = self->s.frame;
      if (!self->bot_info->b_crouch) self->s.frame = FRAME_attack1;
                                else self->s.frame = FRAME_crattak1;
      }
    else {
      if (!self->bot_info->b_crouch) cr_animate_frames( self, FRAME_run1, FRAME_run6 );
                                else cr_animate_frames( self, FRAME_crwalk1, FRAME_crwalk6 );
      }
}

void cr_post_think( edict_t *self )
{
    int      i;
    edict_t* other;

    if ( !self->bot_info->b_shot_this_frame &&
         self->bot_info->time_stop_shoting > level.time ) {
      cr_fire_weapon( self, NULL );
      }
    self->bot_info->b_shot_this_frame = false;

    VectorCopy ( self->s.angles, self->client->ps.viewangles);
    VectorCopy ( self->s.angles, self->client->v_angle);

   // turn on powerups
    if (!(self->flags & FL_POWER_ARMOR) && 
         (self->client->pers.inventory[INDEX_POWER_SCREEN]>0 || self->client->pers.inventory[INDEX_POWER_SHIELD]>0) &&
         self->client->pers.inventory[INDEX_CELLS]>0 ) {
      self->flags |= FL_POWER_ARMOR;
      gi.sound( self, CHAN_AUTO, gi.soundindex("misc/power1.wav"), 1, ATTN_NORM, 0 );
      }
    if ( self->client->quad_framenum<level.framenum &&
         self->client->pers.inventory[INDEX_QUAD]>0 ) {
      gitem_t   *item;
      item = FindItem("Quad Damage");
      if (item && item->use) item->use( self, item );
      }
    if ( self->client->invincible_framenum<level.framenum &&
         self->client->pers.inventory[INDEX_INVULN]>0 ) {
      gitem_t   *item;
      item = FindItem("Invulnerability");
      if (item && item->use) item->use( self, item );
      }
    if ( self->waterlevel>2 &&
         self->client->breather_framenum<level.framenum &&
         self->client->pers.inventory[INDEX_BREATHER]>0 ) {
      gitem_t   *item;
      item = FindItem("Rebreather");
      if (item && item->use) item->use( self, item );
      }

// CTF stuff
    if (self->client->ctf_grapple) {
      CTFGrapplePull(self->client->ctf_grapple);
      gi.linkentity(self);
      }
    CTFApplyRegeneration(self);
    for (i = 1; i <= maxclients->value; i++) {
       other = g_edicts + i;
       if (other->inuse && other->client->chase_target == self)
          UpdateChaseCam(other);
       }
}

qboolean cr_try_pickup_urgent ( edict_t *self )
{
    if ( self->enemy && 
         self->enemy->health>0 && self->enemy->health<50 ) return false;

    if (!cr_find_pickup_urgent(self)) return false;

    self->bot_info->old_think = self->think;
    self->think = cr_think_temp_target;

    self->bot_info->last_node=NULL;
    self->bot_info->next_node=NULL;

   // gi.bprintf ( PRINT_MEDIUM, "urgently going for %s\n", self->bot_info->pickup_target->classname );

    return true;
}

void cr_think_team_help( edict_t *self )
{
    qboolean bStuck;

    if (!cr_update(self,true)) return;
    if (self->bot_info->bot_assignment!=ASSN_HELP) {
      self->think = cr_think;
      return;
      }

   // if (cr_try_pickup_urgent(self)) return;
    cr_find_enemy(self);

    if ( level.time>self->bot_info->time_next_chase_update ) {
      self->bot_info->time_next_chase_update = level.time + 0.5f;
      if ( cr_find_closest_node( self->bot_info->team_leader ) != 
           self->bot_info->target_node ) {
        if (!cr_find_route( self, self->bot_info->team_leader->s.origin, false )) {
          cr_dismiss(self);
          self->think = cr_think;
          }
        }
      }

   //FIXME: dodge while following the path
    bStuck = !cr_moveto(self);

   // move towards enemy by following the route
    if (is_touched( self, self->bot_info->move_target )) {
      // got there!
      self->bot_info->last_node = self->bot_info->next_node;
      self->bot_info->next_node = cr_get_next_path_node(self);
      if (self->bot_info->next_node)
        cr_set_move_target( self, self->bot_info->next_node->position );
      else {
        cr_dismiss(self);
        self->think = cr_think;
        }
      }
    else
    if ( bStuck || cr_no_way( self, self->bot_info->move_target ) ) {
     // can't get there 
      cr_remove_direct_route( self->bot_info->last_node, self->bot_info->next_node );
     // let's find detour!
      if (!cr_find_route( self, self->bot_info->team_leader->s.origin, false )) {
        cr_dismiss(self);
        self->think = cr_think;
        }
      }

   // check if we are near our destination
    if (self->bot_info->team_leader) {
      if ( (int)dist2(self->s.origin,self->bot_info->team_leader->s.origin) < (rand()&0x2FFF) ) {
        cr_dismiss(self);
        if (self->enemy) self->think = cr_think_attack;
                    else self->think = cr_think_salute;
        }
      }

    cr_fire_and_run(self);
}

void cr_think_team_group( edict_t *self )
{
    qboolean bStuck;

    if (!cr_update(self,true)) return;
    if (self->bot_info->bot_assignment!=ASSN_GROUP) {
      self->think = cr_think;
      return;
      }

   // if (cr_try_pickup_urgent(self)) return;
    cr_find_enemy(self);

    if ( level.time>self->bot_info->time_next_chase_update ) {
      self->bot_info->time_next_chase_update = level.time + 0.5f;
      if ( cr_find_closest_node( self->bot_info->team_leader ) != 
           self->bot_info->target_node ) {
        if (!cr_find_route( self, self->bot_info->team_leader->s.origin, false )) {
          self->bot_info->target_node = NULL; 
          self->think = cr_think;
          }
        }
      }

   //FIXME: dodge while following the path
    bStuck = !cr_moveto(self);

   // move towards enemy by following the route
    if (is_touched( self, self->bot_info->move_target )) {
      // got there!
      self->bot_info->last_node = self->bot_info->next_node;
      self->bot_info->next_node = cr_get_next_path_node(self);
      if (self->bot_info->next_node)
        cr_set_move_target( self, self->bot_info->next_node->position );
      else {
        self->bot_info->target_node = NULL; 
        self->think = cr_think;
        }
      }
    else
    if ( bStuck || cr_no_way( self, self->bot_info->move_target ) ) {
     // can't get there 
      cr_remove_direct_route( self->bot_info->last_node, self->bot_info->next_node );
     // let's find detour!
      if (!cr_find_route( self, self->bot_info->team_leader->s.origin, false )) {
        self->bot_info->target_node = NULL; 
        self->think = cr_think;
        }
      }

   // check if we are near our destination
    if (self->bot_info->team_leader) {
      if ( (int)dist2(self->s.origin,self->bot_info->team_leader->s.origin) < (rand()&0x2FFF) ) {
        if (self->enemy) self->think = cr_think_attack;
                    else self->think = cr_think_salute;
        }
      }

    cr_fire_and_run(self);
}

void cr_think_wait( edict_t *self )
{
    if (!cr_update(self,false)) return;

    self->bot_info->time_last_stuck=0;
    self->bot_info->stuck_count=0;

   // scan around
    self->ideal_yaw = anglemod( self->ideal_yaw + self->bot_info->strafe_dir*100*FRAMETIME );
    M_ChangeYaw(self);

    if (level.time > self->bot_info->time_last_strafe_switch) {
      self->bot_info->time_last_strafe_switch = level.time + 2.f+random();
      self->bot_info->strafe_dir = -self->bot_info->strafe_dir;
      }

    if (cr_find_enemy(self)) 
      cr_sight(self,self->enemy);
    else
    if (self->bot_info->bot_assignment==ASSN_GROUP) {
      if ( (int)dist2(self->s.origin,self->bot_info->team_leader->s.origin) > 0x5000 ) {
        self->think = cr_think_team_group;
        }
      }
    else 
    if (self->bot_info->bot_assignment==ASSN_NONE) {
      self->think = cr_think;
      }

    if (!self->bot_info->b_crouch) cr_animate_frames( self, FRAME_stand01, FRAME_stand40 );
                              else cr_animate_frames( self, FRAME_crstnd01, FRAME_crstnd19 );
}

void cr_think_team_patrol( edict_t *self )
{
    qboolean bStuck;

    if (!cr_update(self,true)) return;
    if (self->bot_info->bot_assignment!=ASSN_PATROL) {
      self->think = cr_think;
      return;
      }

    if (cr_try_pickup_urgent(self)) return;
    cr_find_enemy(self);

    if ( level.time>self->bot_info->time_next_chase_update ) {
      self->bot_info->time_next_chase_update = level.time + 0.5f;
      if ( cr_find_closest_node_pos( self, self->bot_info->bot_anchor ) != 
           self->bot_info->target_node ) {
        if (!cr_find_route( self, self->bot_info->bot_anchor, false )) {
          cr_dismiss(self);
          self->think = cr_think;
          }
        }
      }

   //FIXME: dodge while following the path
    bStuck = !cr_moveto(self);

   // move towards enemy by following the route
    if (is_touched( self, self->bot_info->move_target )) {
      // got there!
      self->bot_info->last_node = self->bot_info->next_node;
      self->bot_info->next_node = cr_get_next_path_node(self);
      if (self->bot_info->next_node)
        cr_set_move_target( self, self->bot_info->next_node->position );
      else {
        self->bot_info->target_node = NULL;
        self->think = cr_think;
        }
      }
    else
    if ( bStuck || cr_no_way( self, self->bot_info->move_target ) ) {
     // can't get there 
      cr_remove_direct_route( self->bot_info->last_node, self->bot_info->next_node );
     // let's find detour!
      if (!cr_find_route( self, self->bot_info->bot_anchor, false )) {
        cr_dismiss(self);
        self->think = cr_think;
        }
      }

   // check if we are near our destination
    if (self->bot_info->team_leader) {
      if ( (int)dist2(self->s.origin,self->bot_info->team_leader->s.origin) < (rand()&0x2FFF) ) {
        if (self->enemy) self->think = cr_think_attack;
                    else self->think = cr_think_salute;
        }
      }

    cr_fire_and_run(self);
}

void cr_think_team_guard( edict_t *self )
{
    qboolean bStuck;

    if (!cr_update(self,true)) return;
    if (self->bot_info->bot_assignment!=ASSN_GUARD) {
      self->think = cr_think;
      return;
      }

   // if (cr_try_pickup_urgent(self)) return;
    cr_find_enemy(self);

    if ( level.time>self->bot_info->time_next_chase_update ) {
      self->bot_info->time_next_chase_update = level.time + 0.5f;
      if ( cr_find_closest_node_pos( self, self->bot_info->bot_anchor ) != 
           self->bot_info->target_node ) {
        if (!cr_find_route( self, self->bot_info->bot_anchor, false )) {
         // got to the point!
          self->think = cr_think_salute;
          self->bot_info->target_node = NULL;
          }
        }
      }

   //FIXME: dodge while following the path
    bStuck = !cr_moveto(self);

   // move towards enemy by following the route
    if (is_touched( self, self->bot_info->move_target )) {
      // got there!
      self->bot_info->last_node = self->bot_info->next_node;
      self->bot_info->next_node = cr_get_next_path_node(self);
      if (self->bot_info->next_node)
        cr_set_move_target( self, self->bot_info->next_node->position );
      else {
       // got to the point!
        self->think = cr_think_salute;
        self->bot_info->target_node = NULL;
        }
      }
    else
    if ( bStuck || cr_no_way( self, self->bot_info->move_target ) ) {
     // can't get there 
      cr_remove_direct_route( self->bot_info->last_node, self->bot_info->next_node );
     // let's find detour!
      if (!cr_find_route( self, self->bot_info->bot_anchor, false )) {
       // got to the point!
        self->think = cr_think_salute;
        self->bot_info->target_node = NULL;
        }
      }

   // check if we are near our destination
    if (self->bot_info->team_leader) {
      if ( (int)dist2(self->s.origin,self->bot_info->team_leader->s.origin) < (rand()&0x2FFF) ) {
        if (self->enemy) self->think = cr_think_attack;
                    else self->think = cr_think_salute;
        }
      }

    cr_fire_and_run(self);
}

void cr_think_temp_target( edict_t *self )
{
    if (!cr_update(self,false)) return;

    if ( self->bot_info->pickup_target==NULL ||
        (self->bot_info->pickup_target->svflags & SVF_NOCLIENT) || 
         self->bot_info->pickup_target->solid==SOLID_NOT ||
        !cr_moveto( self ) || is_touched(self, self->bot_info->move_target) ||
         cr_no_way( self, self->bot_info->move_target ) ) {
     // can't get there, let's return to our old target
      cr_skip_pickup_target(self);
      if (self->bot_info->old_think) self->think = self->bot_info->old_think;
                                else self->think = cr_think;
      self->bot_info->old_think = NULL;
      }

   if (!self->bot_info->b_crouch) cr_animate_frames( self, FRAME_run1, FRAME_run6 );
                             else cr_animate_frames( self, FRAME_crwalk1, FRAME_crwalk6 );
}

void cr_change_weapon( edict_t *self )
{
    int i;

    self->client->pers.weapon = self->client->newweapon;
    self->client->newweapon = NULL;
    self->client->machinegun_shots = 0;

    // set visible model
    if (self->s.modelindex == 255) {
    	if (self->client->pers.weapon)
    		i = ((self->client->pers.weapon->weapmodel & 0xff) << 8);
    	else
    		i = 0;
    	self->s.skinnum = (self - g_edicts - 1) | i;
    }

   // gi.bprintf ( PRINT_MEDIUM, "new weapon selected %s\n", self->client->pers.weapon->classname );
    self->bot_info->time_next_shot = level.time + TIME_WEAPON_CHANGE;

    if (self->client->pers.weapon && self->client->pers.weapon->ammo)
      self->client->ammo_index = ITEM_INDEX(FindItem(self->client->pers.weapon->ammo));
    else
      self->client->ammo_index = 0;
}

void cr_choose_best_weapon( edict_t *self )
{
    float enemy_dist = -1;

    if (self->enemy) enemy_dist = dist( self->s.origin, self->enemy->s.origin );
    else if (self->oldenemy) enemy_dist = dist( self->s.origin, self->oldenemy->s.origin );

    if ( self->client->pers.inventory[INDEX_CELLS] &&  
         self->client->pers.inventory[INDEX_HYPERBLASTER] ) {
      self->client->newweapon = FindItem ("hyperblaster");
      return;
      }
    if ( self->client->pers.inventory[INDEX_CELLS]>=50 &&  
         self->client->pers.inventory[INDEX_BFG] ) {
      self->client->newweapon = FindItem ("BFG10K");
      return;
      }
    if ( self->client->pers.inventory[INDEX_SLUGS] &&  
         self->client->pers.inventory[INDEX_RAILGUN] ) {
      self->client->newweapon = FindItem ("railgun");
      return;
      }
    if ( (enemy_dist<0 || enemy_dist>(120-self->health)) &&
         self->client->pers.inventory[INDEX_ROCKETS] &&  
         self->client->pers.inventory[INDEX_ROCKET_LAUNCHER] ) {
      self->client->newweapon = FindItem ("rocket launcher");
      return;
      }
    if ( (enemy_dist<0 || (enemy_dist>(120-self->health) && enemy_dist<500)) &&
         self->client->pers.inventory[INDEX_GRENADES] &&  
         self->client->pers.inventory[INDEX_GRENADE_LAUNCHER] ) {
      self->client->newweapon = FindItem ("grenade launcher");
      return;
      }
    if ( (enemy_dist<0 || enemy_dist<1000) &&
         self->client->pers.inventory[INDEX_BULLETS] &&  
         self->client->pers.inventory[INDEX_CHAINGUN] ) {
      self->client->newweapon = FindItem ("chaingun");
      return;
      }
    if ( self->client->pers.inventory[INDEX_BULLETS] &&
         self->client->pers.inventory[INDEX_MACHINEGUN] ) {
      self->client->newweapon = FindItem ("machinegun");
      return;
      }
    if ( (enemy_dist<0 || enemy_dist<600) &&
         self->client->pers.inventory[INDEX_SHELLS] &&  
         self->client->pers.inventory[INDEX_SUPER_SHOTGUN] ) {
      self->client->newweapon = FindItem ("super shotgun");
      return;
      }
    if ( (enemy_dist<0 || enemy_dist<800) &&
         self->client->pers.inventory[INDEX_SHELLS] &&  
         self->client->pers.inventory[INDEX_SHOTGUN] ) {
      self->client->newweapon = FindItem ("shotgun");
      return;
      }
    self->client->newweapon = FindItem ("blaster");
}

static vec3_t  flash_offset = { 18.4, 7.4, 9.6 };

qboolean cr_aim_ahead ( edict_t *self, edict_t *target, float aim_coeff, float speed,
                        vec3_t start, vec3_t aim, qboolean aim_at_feet )
{
    vec3_t  end, forward, right, up, to;
    float   tt, dist;
    int     tries;
    trace_t trace;
    float   skill_mod;
    vec3_t  dir;
    float   r, u, dd;

    AngleVectors ( self->s.angles, forward, right, NULL );
    G_ProjectSource ( self->s.origin, flash_offset, forward, right, start );

    if (!target) {
      VectorSubtract( self->bot_info->shoot_last_target, start, aim );
      VectorNormalize( aim );
      return true;
      }

    skill_mod = (11-self->bot_pers->skill)*0.1f;

    VectorCopy( target->s.origin, to );
    if (aim_at_feet) to[2] += target->mins[2]*(0.5f+0.4f*random())*(1.f-skill_mod);

    VectorSubtract( to, start, aim );
    dist = VectorLength(aim);
    VectorCopy( to, self->bot_info->shoot_last_target );

    for ( tries=0; ; tries++) {

      tt = 0.8f*aim_coeff*crandom()*skill_mod;
      if (speed>0) tt += dist/speed;
      VectorMA ( to, tt, target->velocity, end );

      VectorSubtract ( end, start, aim );

      vectoangles ( aim, dir );
      AngleVectors ( dir, forward, right, up );

      dd = dist*skill_mod*(0.05f+random());
      r = dd*crandom()*BOT_ACCURACY_HORZ*aim_coeff;
      u = dd*crandom()*BOT_ACCURACY_VERT*aim_coeff;
      VectorMA ( aim, r, right, aim );
      VectorMA ( aim, u, up, aim );

      VectorNormalize(aim);

     // test if we can get to the target shooting like this
      VectorMA( start, dist, aim, end );
      trace = gi.trace ( start, vec3_origin, vec3_origin, end, self, MASK_SHOT );

      if (trace.startsolid || trace.allsolid) return false;
      if (trace.fraction>0.9f || trace.ent==target) break;
      if (tries>2 && self->client->pers.team_no>0 && trace.ent->client &&
          self->client->pers.team_no!=trace.ent->client->pers.team_no) break;
      if (tries>4) return false;
      }

    VectorMA( start, dist, aim, self->bot_info->shoot_last_target );

    return true;  
}

qboolean cr_fire_weapon( edict_t *self, edict_t *target )
{
    vec3_t   start, aim;
    int      damage, damage_amp, cur_weapon;
    qboolean b_quad = false;

    self->bot_info->b_shot_this_frame = true;

    if (self->bot_info->time_next_weapon_change>self->bot_info->time_stop_shoting)
      self->bot_info->time_next_weapon_change = self->bot_info->time_stop_shoting;

    if (self->bot_info->time_next_weapon_change < level.time) {
      cr_choose_best_weapon(self);
      if ( self->client->pers.weapon != self->client->newweapon ) {
        self->bot_info->time_next_weapon_change = level.time + 1.5f*TIME_WEAPON_CHANGE;
        cr_change_weapon(self);
        return false;
        }
      }

    cur_weapon = ITEM_INDEX(self->client->pers.weapon);

    if (target) {
      if (cur_weapon == INDEX_RAILGUN) {
        self->bot_info->time_stop_shoting = level.time + TIME_RAILGUN_AFTERSHOT;
        }
      else if ( cur_weapon==INDEX_HYPERBLASTER ) {
        self->bot_info->time_stop_shoting = level.time + TIME_HYPERBLASTER_AFTERSHOT;
        }
      else if ( cur_weapon==INDEX_ROCKET_LAUNCHER ) {
        self->bot_info->time_stop_shoting = level.time + TIME_ROCKET_AFTERSHOT;
        }
      else if ( cur_weapon==INDEX_GRENADE_LAUNCHER ) {
        self->bot_info->time_stop_shoting = level.time + TIME_GRENADE_AFTERSHOT;
        }
      else if ( cur_weapon==INDEX_CHAINGUN ) {
        self->bot_info->time_stop_shoting = level.time + TIME_CHAINGUN_AFTERSHOT;
        }
      else if ( cur_weapon==INDEX_MACHINEGUN ) {
        self->bot_info->time_stop_shoting = level.time + TIME_MACHINEGUN_AFTERSHOT;
        }
      else if ( cur_weapon==INDEX_SUPER_SHOTGUN ) {
        self->bot_info->time_stop_shoting = level.time + TIME_SUPERSHOTGUN_AFTERSHOT;
        }
      else if ( cur_weapon==INDEX_SHOTGUN ) {
        self->bot_info->time_stop_shoting = level.time + TIME_SHOTGUN_AFTERSHOT;
        }
      else if ( cur_weapon==INDEX_BLASTER ) {
        self->bot_info->time_stop_shoting = level.time + TIME_BLASTER_AFTERSHOT;
        }
      else if ( cur_weapon==INDEX_BFG ) {
        if ( self->bot_info->time_next_shot <= level.time &&
             self->bot_info->time_weapon_spin_down <= level.time &&
             self->bot_info->time_weapon_spin_up < level.time ) {
          cr_effect(self,MZ_BFG);
          self->bot_info->time_stop_shoting = level.time + TIME_BFG_AFTERSHOT;
          self->bot_info->time_weapon_spin_up = level.time + TIME_BFG_SPINUP;
          return true;
          }
        }
      else {
        self->bot_info->time_stop_shoting = level.time + FRAMETIME/2;
        }
      }
    else {
      if ( cur_weapon==INDEX_HYPERBLASTER ) {
        self->bot_info->time_weapon_spin_down = level.time + TIME_WEAPON_CHANGE;
        }
      else if ( cur_weapon==INDEX_CHAINGUN ) {
        self->bot_info->time_weapon_spin_down = level.time + TIME_WEAPON_CHANGE;
        }
      else {
        self->bot_info->time_weapon_spin_down = level.time - FRAMETIME;
        }
      }

    if ( self->bot_info->time_next_shot > level.time ) return false;
    if ( self->bot_info->time_weapon_spin_down > level.time ) return false;
    if ( self->bot_info->time_weapon_spin_up > level.time ) return false;

    if ( (int)dmflags->value & DF_INFINITE_AMMO ) {
      if (self->client->ammo_index) self->client->pers.inventory[self->client->ammo_index] = 999;
      }

    if (self->bot_info->time_next_fight_message<level.time) {
      cr_message_fight ( self, target );
      self->bot_info->time_next_fight_message = level.time + FIGHT_MSG_DELAY*(0.7f+0.6f*random());
      }

    b_quad = self->client->quad_framenum > level.framenum;
    damage_amp = 1;
    if ( b_quad ) damage_amp = 4;
    if ( self->client->pers.inventory[INDEX_TECH2] ) damage_amp *= 2;

#define hasted(x) (CTFApplyHaste(self) ? (x)/2 : (x))

    if ( cur_weapon==INDEX_RAILGUN ) {
      // fire "railgun"
      if (!cr_aim_ahead( self, target, 2.f, RAILGUN_SPEED, start, aim, false )) return false;
      damage = RAILGUN_DAMAGE*damage_amp;
      fire_rail ( self, start, aim, damage, b_quad ? 800 : 200 );
      cr_effect( self, MZ_RAILGUN );
      self->bot_info->time_next_shot = level.time + hasted(TIME_RAILGUN_SHOT);
      self->client->pers.inventory[self->client->ammo_index] -= self->client->pers.weapon->quantity;
      return true;
      }
    if ( cur_weapon==INDEX_HYPERBLASTER ) {
      // fire "hyperblaster"
      if (!cr_aim_ahead( self, target, 1.6f, HYPERBLASTER_SPEED, start, aim, false )) return false;
      damage = HYPERBLASTER_DAMAGE*damage_amp;
      fire_blaster ( self, start, aim, damage, BLASTER_SPEED, EF_HYPERBLASTER, true );
      cr_effect( self, MZ_HYPERBLASTER );
      self->bot_info->time_next_shot = level.time + hasted(TIME_HYPERBLASTER_SHOT);
      self->client->pers.inventory[self->client->ammo_index] -= self->client->pers.weapon->quantity;
      return true;
      }
    if ( cur_weapon==INDEX_BFG ) {
      // fire "BFG"
      if (!cr_aim_ahead( self, target, 1.f, BFG_SPEED, start, aim, false )) return false;
      damage = BFG_DAMAGE*damage_amp;
      fire_bfg ( self, start, aim, damage, BFG_SPEED, 1000 );
      self->bot_info->time_next_shot = level.time + hasted(TIME_BFG_SHOT);
      self->bot_info->time_weapon_spin_down = level.time + TIME_BFG_SPINUP;
      self->client->pers.inventory[self->client->ammo_index] -= 50;
      return true;
      }
    if ( cur_weapon==INDEX_ROCKET_LAUNCHER ) {
      // fire "hyperblaster"
      if (!cr_aim_ahead( self, target, 0.6f, ROCKET_SPEED, start, aim, true )) return false;
      VectorMA( start, 5.f, aim, start );
      damage = (1.f+0.2f*random())*ROCKET_DAMAGE*damage_amp;
      fire_rocket ( self, start, aim, damage, ROCKET_SPEED, 120/*damage_rad*/, damage/*radius_damage*/ );
      cr_effect( self, MZ_ROCKET );
      self->bot_info->time_next_shot = level.time + hasted(TIME_ROCKET_SHOT);
      self->client->pers.inventory[self->client->ammo_index] -= self->client->pers.weapon->quantity;
      return true;
      }
    if ( cur_weapon==INDEX_GRENADE_LAUNCHER ) {
      // fire "grenade"
      if (!cr_aim_ahead( self, target, 0.4f, GRENADE_SPEED, start, aim, rand()%1 )) return false;
      damage = GRENADE_DAMAGE*damage_amp;
      fire_grenade ( self, start, aim, damage, GRENADE_SPEED, 2.5, damage+40 );
      cr_effect( self, MZ_GRENADE );
      self->bot_info->time_next_shot = level.time + hasted(TIME_GRENADE_SHOT);
      self->client->pers.inventory[self->client->ammo_index] -= self->client->pers.weapon->quantity;
      return true;
      }
    if ( cur_weapon==INDEX_CHAINGUN ) {
      // fire "chaingun"
      if (!cr_aim_ahead( self, target, 1.8f, CHAINGUN_SPEED, start, aim, false )) return false;
      damage = CHAINGUN_DAMAGE*damage_amp;
      fire_bullet ( self, start, aim, damage, b_quad ? 8 : 2, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN );
      cr_effect( self, MZ_CHAINGUN2 );
      self->bot_info->time_next_shot = level.time + hasted(TIME_CHAINGUN_SHOT);
      self->client->pers.inventory[self->client->ammo_index] -= self->client->pers.weapon->quantity;
      return true;
      }
    if ( cur_weapon==INDEX_MACHINEGUN ) {
      // fire "machinegun"
      if (!cr_aim_ahead( self, target, 1.1f, MACHINEGUN_SPEED, start, aim, false )) return false;
      damage = MACHINEGUN_DAMAGE*damage_amp;
      fire_bullet ( self, start, aim, damage, b_quad ? 8 : 2, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_MACHINEGUN );
      cr_effect( self, MZ_MACHINEGUN );
      self->bot_info->time_next_shot = level.time + hasted(TIME_MACHINEGUN_SHOT);
      self->client->pers.inventory[self->client->ammo_index] -= self->client->pers.weapon->quantity;
      return true;
      }
    if ( cur_weapon==INDEX_SUPER_SHOTGUN ) {
      // fire "super shotgun"
      if (!cr_aim_ahead( self, target, 1.4f, SUPERSHOTGUN_SPEED, start, aim, false )) return false;
      damage = SUPERSHOTGUN_DAMAGE*damage_amp;
      fire_shotgun ( self, start, aim, damage, b_quad ? 4*12 : 12, 
                     DEFAULT_SHOTGUN_HSPREAD, 1.6f*DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT, MOD_SSHOTGUN );
      cr_effect( self, MZ_SSHOTGUN );
      self->bot_info->time_next_shot = level.time + hasted(TIME_SUPERSHOTGUN_SHOT);
      self->client->pers.inventory[self->client->ammo_index] -= self->client->pers.weapon->quantity;
      return true;
      }
    if ( cur_weapon==INDEX_SHOTGUN ) {
      // fire "shotgun"
      if (!cr_aim_ahead( self, target, 1.f, SHOTGUN_SPEED, start, aim, false )) return false;
      damage = SHOTGUN_DAMAGE*damage_amp;
      fire_shotgun ( self, start, aim, damage, b_quad ? 4*8 : 8, 
                     500, 500, DEFAULT_DEATHMATCH_SHOTGUN_COUNT, MOD_SHOTGUN );
      cr_effect( self, MZ_SHOTGUN );
      self->bot_info->time_next_shot = level.time + hasted(TIME_SHOTGUN_SHOT);
      self->client->pers.inventory[self->client->ammo_index] -= self->client->pers.weapon->quantity;
      return true;
      }
    if ( cur_weapon==INDEX_BLASTER ) {
      // fire "blaster"
      if (!cr_aim_ahead( self, target, 0.8f, BLASTER_SPEED, start, aim, false )) return false;
      damage = BLASTER_DAMAGE*damage_amp;
      fire_blaster ( self, start, aim, damage, BLASTER_SPEED, EF_BLASTER, false );
      cr_effect( self, MZ_BLASTER );
      self->bot_info->time_next_shot = level.time + hasted(TIME_BLASTER_SHOT);
      return true;
      }

#undef hasted

    return false;
}

qboolean cr_find_route( edict_t *self, vec3_t target, qboolean b_important )
{
    path_node_t *node_stack1[MAX_STACK_NODE], *node_stack2[MAX_STACK_NODE];
    path_node_t *node, *link_node, *target_node, *next_link;
    path_node_t **nodes, **new_nodes;
    int          cur_stack, stack_nodes, new_stack_nodes, i, j, max_nodes, n_count;
    float        cur_dist, best_dist, d;

    self->bot_info->next_node=NULL;
    self->bot_info->last_node = cr_find_closest_node(self);

    VectorCopy ( self->s.origin, self->s.old_origin);
    VectorCopy ( target, self->s.origin );
    target_node = cr_find_closest_node(self);
    VectorCopy ( self->s.old_origin, self->s.origin );

    self->bot_info->target_node = target_node;

    if (target_node == self->bot_info->last_node) {
     // gi.bprintf ( PRINT_MEDIUM, "Can't find route: starting node == target node\n" );
      return false;
      }

   // initialize node network
    for ( node=cr_node_head; node!=NULL; node=node->next ) node->route_dist=-1;

    max_nodes = 20 + 3*self->bot_pers->skill;
    if (b_important) max_nodes *= 2;
    if (max_nodes>(MAX_PATH_NODES-2)) max_nodes = MAX_PATH_NODES-2;

   // find actual route
    self->bot_info->last_node->route_dist = 0.001;
    stack_nodes = 1;
    node_stack1[0] = self->bot_info->last_node;
    new_stack_nodes = 1;
    cur_stack = 0;

    n_count=0;
    while (n_count<max_nodes) {
       stack_nodes = new_stack_nodes;
       new_stack_nodes = 0;

       if (cur_stack==0) {
         nodes     = node_stack1;
         new_nodes = node_stack2;
         cur_stack=1;
         }
       else {
         nodes     = node_stack2;
         new_nodes = node_stack1;
         cur_stack=0;
         }

       best_dist = 1e32;
       for ( i=0; i<stack_nodes; i++ ) {
          node = *nodes++;
          cur_dist = node->route_dist;
          if (best_dist>cur_dist) best_dist=cur_dist;

         // go through all links
          for ( j=0; j<MAX_NODE_LINKS; j++ ) {
             if ( !(link_node = node->link_to[j]) ) break;
             d = cur_dist + node->link_dist[j];

             if (link_node!=target_node && link_node->route_dist<0 && new_stack_nodes<MAX_STACK_NODE) {
              // add new downlink node
               *new_nodes = link_node;
               new_nodes++;
               new_stack_nodes++;
               }

             if ( link_node->route_dist<0 || link_node->route_dist>d ) {
              // update route_dist if this route is shorter
               link_node->route_dist = d;
               }
             }
          }

       if (!new_stack_nodes) {
        // gi.bprintf ( PRINT_MEDIUM, "no new nodes! \n" );
         break;
         }
       if ( target_node->route_dist>=0 && target_node->route_dist<best_dist ) {
        // gi.bprintf ( PRINT_MEDIUM, "Route found! %.2f\n", target_node->route_dist );
         break;
         }

       n_count++;
       }

    if ( target_node->route_dist<0 ) {
      //gi.bprintf ( PRINT_MEDIUM, "Can't find route: no route to target\n" );
      return false;
      }

/*
   // remove path indication
    { edict_t* line, *next_line;
    for ( line = global_path_route; line!=NULL; line=next_line ) {
       next_line = line->teamchain;
       G_FreeEdict(line);
       }
    } */

    self->bot_info->path_nodes=-1;
    n_count=0;
    node = target_node;
    while (n_count<=max_nodes) {
        // search for shortest possible distance from "node"
         best_dist=0;
         link_node=NULL;
         for ( i=0; i<MAX_NODE_LINKS; i++ ) {
            if ( !(next_link = node->link_from[i]) ) break;
            d = next_link->route_dist;
            if (d>=0 && (!link_node || best_dist>d) ) {
              link_node = next_link;
              best_dist = d;
              } 
            }

         self->bot_info->path[++self->bot_info->path_nodes]=node;

         if (!link_node || link_node==self->bot_info->last_node) break;

         node->route_dist = -2;

#ifdef _DEBUG
         cr_draw_line( node->position, link_node->position, COLOR_BEAM_GREEN, &global_path_route );
#endif
         node = link_node;
         n_count++;
         }

    self->bot_info->next_node = node;
    cr_set_move_target( self, node->position );

    return true;
}

void cr_try_to_find_route(  edict_t *self, vec3_t target )
{
    if (!cr_find_route( self, target, false )) {
      cr_skip_enemy(self);
      self->think = cr_think;
      }
    else {
      self->think = cr_think_chase_route;
      }
}

/*
qboolean cr_try_to_hide( edict_t *self )
{
    if ( (level.time - self->bot_info->time_indarkness) > FRAMETIME*5 ) {
      gi.dprintf( "%s is hiding at %.1f %.1f %.1f!\n", self->client->pers.netname, self->s.origin[0], self->s.origin[1], self->s.origin[2] );
      cr_crouch(self,true);
      self->think = cr_think_hide;
      return true;
      } 
    return false;
}

void cr_think_hide( edict_t *self )
{
    if (!cr_update(self,true)) return;

   cr_animate_frames( self, FRAME_crstnd01, FRAME_crstnd19 );
}
*/

void cr_think_run_for_life ( edict_t *self )
{
    qboolean bStuck;

    if (!cr_update(self,true)) return;
//FIXME: implement searching for health only!

    if (!self->bot_info->pickup_target) {
      self->think = cr_think;
      return; 
      }

    bStuck = !cr_moveto(self);

    if ( is_touched( self, self->bot_info->move_target ) ) {
      if ( self->bot_info->pickup_target &&
           is_touched( self, self->bot_info->pickup_target->s.origin ) ) {
        self->bot_info->pickup_target=NULL;
        }
      else {
        self->bot_info->last_node = self->bot_info->next_node;
        self->bot_info->next_node = cr_get_next_path_node(self);
        if (self->bot_info->next_node)
          cr_set_move_target( self, self->bot_info->next_node->position );
        else {
          self->bot_info->pickup_target=NULL;
          cr_find_pickup_on_map(self);
          }
        }
      }
    else
    if ( bStuck || cr_no_way( self, self->bot_info->move_target ) ) {
      cr_remove_direct_route( self->bot_info->last_node, self->bot_info->next_node );
      cr_skip_pickup_target(self);
      }

    if (self->bot_info->pickup_target) {
      if ( !self->bot_info->pickup_target->inuse ||
           (self->bot_info->pickup_target->svflags & SVF_NOCLIENT) || 
           self->bot_info->pickup_target->solid==SOLID_NOT ) {
        cr_forget_pickup_target(self);
        }
      }

    cr_fire_and_run(self);
}

void cr_think_chase_route( edict_t *self )
{
    qboolean bStuck;

    if (!cr_update(self,true)) return;

    if (cr_try_pickup_urgent(self)) return;

    cr_find_enemy(self);
    if (!self->enemy) {
      if ( level.time > self->bot_info->time_chase ) {
        self->bot_info->next_node=NULL;
        self->think = cr_think;
        return;
        }
      if ( self->oldenemy && 
          (level.time-self->monsterinfo.trail_time)>(5.f*random()*(11-self->bot_pers->skill)) ) {
        VectorCopy( self->oldenemy->s.origin, self->monsterinfo.last_sighting );
        self->monsterinfo.trail_time = level.time;
        cr_try_to_find_route( self, self->monsterinfo.last_sighting );
        }
      }
    else {
      self->bot_info->time_chase = level.time + CHASE_TIME*(0.9f+random()*0.2f);
      }

//FIXME: check for new position not so often
    if ( self->enemy &&
         cr_find_closest_enemy_node(self)!=self->bot_info->target_node ) {
      cr_try_to_find_route( self, self->enemy->s.origin ); 
      }

   //FIXME: dodge while following the path
    bStuck = !cr_moveto(self);

   // move towards enemy by following the route
    if (is_touched( self, self->bot_info->move_target )) {
      // got there!
      self->bot_info->last_node = self->bot_info->next_node;
      self->bot_info->next_node = cr_get_next_path_node(self);
      if (self->bot_info->next_node)
        cr_set_move_target( self, self->bot_info->next_node->position );
      else {
        self->think = cr_think;
        }
      }
    else
    if ( bStuck || cr_no_way( self, self->bot_info->move_target ) ) {
     // can't get there 
      cr_remove_direct_route( self->bot_info->last_node, self->bot_info->next_node );
     // let's find detour!
      if (self->enemy) cr_try_to_find_route( self, self->enemy->s.origin );
                  else cr_try_to_find_route( self, self->monsterinfo.last_sighting );
      }

    cr_fire_and_run(self);
}

void cr_think_follow_route( edict_t *self )
{
    qboolean bStuck;

    if (!cr_update(self,true)) return;

    bStuck = !cr_moveto(self);

   // move towards enemy by following the route
    if (is_touched( self, self->bot_info->move_target )) {
      // got there!
      self->bot_info->last_node = self->bot_info->next_node;
      self->bot_info->next_node = cr_get_next_path_node(self);
      if (self->bot_info->next_node)
        cr_set_move_target( self, self->bot_info->next_node->position );
      else {
        self->think = cr_think;
        }
      }
    else
    if ( bStuck || cr_no_way( self, self->bot_info->move_target ) ) {
     // can't get there 
      cr_remove_direct_route( self->bot_info->last_node, self->bot_info->next_node );
      if (!cr_find_route( self, self->monsterinfo.last_sighting, false )) {
        self->think = cr_think;
        }
      }

    if ( (level.time-self->monsterinfo.trail_time)>TEAM_HELP_TIME ) {
      self->think = cr_think;
      }

    cr_fire_and_run(self);
}

void cr_think_chase (edict_t *self)
{
    qboolean bStuck;

    if (!cr_update(self,true)) return;

    if (cr_try_pickup_urgent(self)) return;

    cr_find_enemy(self);
    // move towards enemy
    if (!self->enemy) {
      if ( level.time > self->bot_info->time_chase ) {
        self->think = cr_think;
        return;
        }
      cr_set_move_target( self, self->monsterinfo.last_sighting );
      }
    else {
      self->bot_info->time_chase = level.time + CHASE_TIME*(0.9f+random()*0.2f);
      cr_set_move_target( self, self->enemy->s.origin );
      }

   //FIXME: dodge while going towards the enemy
    bStuck = !cr_moveto(self);

    if (is_touched( self, self->bot_info->move_target )) {
      if ( self->oldenemy && (10*random())<self->bot_pers->skill &&
           ITEM_INDEX(self->client->pers.weapon)>INDEX_SUPER_SHOTGUN ) {
        VectorCopy( self->oldenemy->s.origin, self->monsterinfo.last_sighting );
        self->monsterinfo.trail_time = level.time;
        cr_try_to_find_route( self, self->monsterinfo.last_sighting );
        }
      else
        self->think = cr_think;
      }
    else
    if ( bStuck || cr_no_way( self, self->bot_info->move_target ) ) {
     // can't get there, let's find detour!
      cr_try_to_find_route( self, self->bot_info->move_target );
      }

    cr_fire_and_run(self);
}

void cr_switch_strafe( edict_t *self )
{
     self->bot_info->time_last_strafe_switch = level.time + 0.1f;
     self->bot_info->strafe_dir = -self->bot_info->strafe_dir;
}

void cr_think_attack( edict_t *self )
{
    float    dist, strafe_a;
    vec3_t   move;
    qboolean bSwitchStrafeDir;

    if (!cr_update(self,true)) return;

    if (!self->enemy && self->oldenemy) {
      if (self->oldenemy->deadflag==DEAD_DEAD) {
        if (self->oldenemy->client && !self->oldenemy->bot_info) self->bot_pers->adapt_count--;
        if (random()<0.3f) cr_message_kill( self, self->oldenemy );
        if ( (int)bot_taunt->value && !cr_find_enemy(self) && 
             !self->bot_info->b_crouch && !self->waterlevel ) 
          self->think = random()<0.3f ? cr_think_taunt : cr_think_flip;
        self->oldenemy=NULL;
        return;
        }
      }

    if (cr_try_pickup_urgent(self)) return;

    dist = distance( self, self->enemy );
    if (!self->enemy || dist>self->bot_pers->attack_range*1.1f) {
      cr_switch_strafe(self);
      self->think = cr_think_chase;
      return;
      } 

    if (self->bot_info->strafe_dir>0) self->bot_info->strafe_dir=1; 
                                 else self->bot_info->strafe_dir=-1;

    if ( 50*random()<(self->bot_pers->attack_range - dist) ) strafe_a = 98 + 8*random();
                                                        else strafe_a = 82 - 8*random();

   // face the enemy
    VectorSubtract ( self->enemy->s.origin, self->s.origin, move );
    self->ideal_yaw = vectoyaw(move);
    M_ChangeYaw(self);

    self->ideal_yaw = anglemod( self->ideal_yaw + self->bot_info->strafe_dir*strafe_a );

    bSwitchStrafeDir = (3+1.6f*(10-self->bot_pers->skill))*random()<(level.time-self->bot_info->time_last_strafe_switch);

    if (!cr_move_yaw( self, false, true )) bSwitchStrafeDir=true;

    if (bSwitchStrafeDir) cr_switch_strafe(self);

    cr_fire_and_run(self);
}

void cr_sight(edict_t *self, edict_t *other)
{
    self->think = cr_think_chase;
}

void cr_think_taunt ( edict_t *self )
{
    if (!cr_update(self,false)) return;

    self->bot_info->time_last_stuck=0;
    self->bot_info->stuck_count=0;

    cr_run_frames( self, FRAME_taunt01, FRAME_taunt17 );

    if (self->s.frame==FRAME_taunt17) {
      self->think = cr_think;
      }
}

void cr_think_flip ( edict_t *self )
{
    if (!cr_update(self,false)) return;

    self->bot_info->time_last_stuck=0;
    self->bot_info->stuck_count=0;

    cr_run_frames( self, FRAME_flip01, FRAME_flip12 );

    if (self->s.frame==FRAME_flip12) {
      self->think = cr_think;
      }
}

void cr_think_salute ( edict_t *self )
{
    if (!cr_update(self,false)) return;

    self->bot_info->time_last_stuck=0;
    self->bot_info->stuck_count=0;

    cr_run_frames( self, FRAME_salute01, FRAME_salute11 );

    if ( level.time < self->bot_info->time_next_salute ||
         self->s.frame==FRAME_salute11 ) {
      self->bot_info->time_next_salute = level.time + 10.f+3*random();
      if (self->bot_info->bot_assignment==ASSN_GROUP) 
        self->think = cr_think_wait; 
      else
      if (self->bot_info->bot_assignment==ASSN_GUARD) 
        self->think = cr_think_wait;
      else
      if (self->bot_info->bot_assignment==ASSN_PATROL) 
        self->think = cr_think_team_patrol; 
      else 
        self->think = cr_think;
      }
}

void cr_think_pickup ( edict_t *self )
{
    qboolean bStuck;
     
    if (!cr_update(self,true)) return;

    if (!self->bot_info->pickup_target) {
      self->bot_info->last_node=NULL;
      self->bot_info->next_node=NULL;
      self->think = cr_think;
      return; 
      }

    if (cr_find_enemy(self)) {
      cr_sight(self,self->enemy);
      return;
      }

    bStuck = !cr_moveto(self);

    if (is_touched( self, self->bot_info->move_target )) {
     // gi.bprintf ( PRINT_MEDIUM, "got to waypoint!\n" );
      if ( self->bot_info->pickup_target &&
           is_touched( self, self->bot_info->pickup_target->s.origin ) ) {
        self->bot_info->pickup_target=NULL;
        }
      else {
        self->bot_info->last_node = self->bot_info->next_node;
        self->bot_info->next_node = cr_get_next_path_node(self);
        if (self->bot_info->next_node)
          cr_set_move_target( self, self->bot_info->next_node->position );
        else {
          self->bot_info->pickup_target=NULL;
          // gi.bprintf ( PRINT_MEDIUM, "path ended!\n" );
          }
        //cr_find_pickup_on_map(self);
        }
      }
    else
    if ( bStuck || cr_no_way( self, self->bot_info->move_target ) ) {
      cr_remove_direct_route( self->bot_info->last_node, self->bot_info->next_node );
      cr_skip_pickup_target(self);
      // gi.bprintf ( PRINT_MEDIUM, "can't get there!\n" );
      }

    if ( self->bot_info->pickup_target &&
    // special case: missing flag on CTF base
        !( (self->client->pers.inventory[INDEX_FLAG2]>0 && 
            strcmp( self->bot_info->pickup_target->classname, "item_flag_team1")==0) ||
           (self->client->pers.inventory[INDEX_FLAG1]>0 && 
            strcmp( self->bot_info->pickup_target->classname, "item_flag_team2")==0) ) ) {
      if (!self->bot_info->pickup_target->inuse ||
          (self->bot_info->pickup_target->svflags & SVF_NOCLIENT) || 
           self->bot_info->pickup_target->solid==SOLID_NOT ) {
        // gi.bprintf ( PRINT_MEDIUM, "somebody picked it up before me!\n" );
        cr_forget_pickup_target(self);
        }
      }

    cr_fire_and_run(self);
}

qboolean cr_return_to_base( edict_t *self )
{
    edict_t *hit;
    for ( hit=g_edicts+1; hit<&g_edicts[globals.num_edicts]; hit++) {
       if (!hit->inuse || !hit->item) continue;
      // if ((hit->svflags & SVF_NOCLIENT) || (hit->solid==SOLID_NOT)) continue;
       if (hit->spawnflags & DROPPED_ITEM) continue;

       if (hit->item->pickup==CTFPickup_Flag) {
         if ( self->client->resp.ctf_team==CTF_TEAM1 && 
              strcmp(hit->classname, "item_flag_team1")==0 ) break;
         if ( self->client->resp.ctf_team==CTF_TEAM2 && 
              strcmp(hit->classname, "item_flag_team2")==0 ) break;
         }
       }

    if (!hit) {
     // gi.dprintf("NO BASE FLAG FOUND!\n"); //!!!
      return false;
      }

    return cr_force_pickup_target( self, hit );
}

qboolean cr_try_special_assignment( edict_t *self )
{
    edict_t *hit, *enemy_flag, *team_flag;
    int      flag;

    if (!ctf->value) return false;

    if ( self->bot_info->time_next_special_assignment > level.time ) return false;
    self->bot_info->time_next_special_assignment = level.time + 8.f + 2*random();

  // special case: we have enemy flag, let's go return it
    if ((self->client->pers.inventory[INDEX_FLAG1]>0) ||
        (self->client->pers.inventory[INDEX_FLAG2]>0)) {
      if (cr_return_to_base(self)) return true;
      }

    enemy_flag=NULL;
    team_flag=NULL;
    for ( hit=g_edicts+1; hit<&g_edicts[globals.num_edicts]; hit++) {
       if (!hit->inuse) continue;
       if ((hit->svflags & SVF_NOCLIENT) || (hit->solid==SOLID_NOT)) continue;

       if (hit->item && hit->item->pickup==CTFPickup_Flag) {
         if (strcmp(hit->classname, "item_flag_team1")==0) flag=CTF_TEAM1;
         else if (strcmp(hit->classname, "item_flag_team2")==0) flag=CTF_TEAM2;
         else continue;

         if ( flag==self->client->resp.ctf_team ) team_flag=hit;
                                             else enemy_flag=hit;
         }
       else 
       if ( hit->client && 
           (hit->client->pers.inventory[INDEX_FLAG1] || hit->client->pers.inventory[INDEX_FLAG2]) ) {
         if ( hit->client->resp.ctf_team!=self->client->resp.ctf_team ) team_flag=hit;
                                                                   else enemy_flag=hit;
         }

       if (enemy_flag && team_flag) break;
       }

    if (team_flag) {
     // check if team_flag at base and thus alright
      if (!team_flag->client && !(team_flag->spawnflags & DROPPED_ITEM)) {
       // if we have enemy flag, let's go score a capture!
        if (self->client->pers.inventory[INDEX_FLAG1] || self->client->pers.inventory[INDEX_FLAG2]) {
          if (team_flag && cr_force_pickup_target( self, team_flag )) {
           // gi.dprintf ( "%s going to finish the capture!\n", self->client->pers.netname ); 
            return true;
            }
        //  else {
        //    gi.dprintf ( "%s doesn't know going to get enemy flag to the base!\n", self->client->pers.netname ); 
        //    }
          }
        }
      else {
       // rescue our team_flag! 
        if (team_flag->client) {
          if ( (rand()%1) && cr_force_attack_enemy( self, team_flag )) {
           // gi.dprintf ( "%s going to kill flag carrier!\n", self->client->pers.netname ); 
            return true;
            }
          }
        else {
          if (cr_force_pickup_target( self, team_flag )) {
           // gi.dprintf ( "%s going to return the flag!\n", self->client->pers.netname ); 
            return true;
            }
          }
        }
      }
                                            
    if (enemy_flag && !enemy_flag->client) {
      if (cr_force_pickup_target( self, enemy_flag )) {
       // gi.dprintf ( "%s going to capture the flag!\n", self->client->pers.netname ); 
        return true;
        }
      }

    return false;
}

void cr_think ( edict_t *self )
{
    if (!cr_update(self,true)) return;

    if (level.time > self->bot_info->time_next_assignment_check) {
      self->bot_info->time_next_assignment_check = level.time+3.f;
      if (self->bot_info->bot_assignment==ASSN_GROUP) {
        self->think = cr_think_team_group;
        return;
        }
      else
      if (self->bot_info->bot_assignment==ASSN_GUARD) {
        self->think = cr_think_team_guard;
        return;
        }
      else 
      if (self->bot_info->bot_assignment!=ASSN_NONE) {
        cr_dismiss(self);
        }
      }

    if (cr_find_enemy(self)) {
      cr_sight(self,self->enemy);
      return;
      } 
    if (cr_try_special_assignment(self)) return;

    cr_find_pickup_target(self);
    if (!self->bot_info->pickup_target && 
          self->bot_info->bot_assignment==ASSN_NONE) {
     // let's go for pickups we saw once
      self->bot_info->pickup_target_score = 1e32;
      cr_find_pickup_on_map(self);
      }

    if (!self->bot_info->pickup_target) {
      if ( level.time > self->bot_info->time_next_roam_dir_change ||
           !cr_move_yaw( self, true, random()>0.02f ) ) {
        float    dy;
        qboolean bReverse;

        self->ideal_yaw = anglemod( self->ideal_yaw + 10*(0.5f-random()) );

        bReverse = random()>0.5f;
        for ( dy=60; ; dy+=30*(0.8f+0.2f*random()) ) {
           if (bReverse) dy = -dy;
           if (cr_try_move(self,  dy, true )) break;
           if (cr_try_move(self, -dy, true )) break;
           if (bReverse) dy = -dy;
           if (dy>120) { self->ideal_yaw = 360*random(); break; }
           }
        self->bot_info->time_next_roam_dir_change = level.time + TIME_ROAM_DIR_CHANGE;
        }
      }
    else
    if ( !cr_moveto(self) ||
         is_touched2d( self, self->bot_info->move_target ) ) {
      cr_skip_pickup_target(self);
      cr_pick_random_destination(self); 
      }

    if (self->bot_info->pickup_target) {
      if ( !self->bot_info->pickup_target->inuse ||
           (self->bot_info->pickup_target->svflags & SVF_NOCLIENT) || 
           self->bot_info->pickup_target->solid==SOLID_NOT ) 
        cr_forget_pickup_target(self);
      }

    if (!self->bot_info->b_crouch) cr_animate_frames( self, FRAME_run1, FRAME_run6 );
                              else cr_animate_frames( self, FRAME_crwalk1, FRAME_crwalk6 );
}

void cr_run_frames( edict_t *self, int start, int end )
{
    if ( self->s.frame<start || 
         self->s.frame>=end ) 
      self->s.frame = start;
    else
      self->s.frame++;
}

void cr_animate_frames( edict_t *self, int start, int end )
{
    if (self->client->anim_priority==ANIM_DEATH) {
      self->s.frame = self->client->anim_end;
      self->think = cr_death;
      return;
      }

    if (!self->groundentity && !self->waterlevel) {
      if (self->s.frame<FRAME_jump1 || self->s.frame>FRAME_jump6) self->s.frame = FRAME_jump1;
      if (self->s.frame!=FRAME_jump6) self->s.frame++;
      return; 
      }

    cr_run_frames(self,start,end);

    if (!self->waterlevel) {
      switch (self->s.frame) {
        case FRAME_crwalk3: 
            gi.sound ( self, CHAN_BODY, sound_footstep[(int)(random()*4)], 0.4f, ATTN_NORM, 0 );
            break;
        case FRAME_run2: 
        case FRAME_run5: 
            gi.sound ( self, CHAN_BODY, sound_footstep[(int)(random()*4)], 0.6f, ATTN_NORM, 0 );
            break;
        }
      }
}

void cr_routes_load();

void cr_init_node_net()
{
    int         count=0;
    edict_t     *hit, *other;
    path_node_t *node, *node2;
    vec3_t       pos;

    for ( hit=g_edicts+1; hit<&g_edicts[globals.num_edicts]; hit++) {
       if (!hit->inuse) continue;
       
       if (hit->touch==teleporter_touch) {
         other = G_Find( NULL, FOFS(targetname), hit->target );
         if (!other) continue;
         node = cr_insert_node( hit->s.origin, NULL, 0 );
         node2 = cr_insert_node( other->s.origin, NULL, NF_TELEPORT );
         cr_add_direct_route( node, node2, false );
         continue;
         }  
       if (hit->touch == Touch_Plat_Center) {
         pos[0] = (hit->absmin[0]+hit->absmax[0])/2;
         pos[1] = (hit->absmin[1]+hit->absmax[1])/2;
         pos[2] = hit->absmin[2] + STEPSIZE;
         node = cr_insert_node( pos, NULL, NF_ELEVATOR );
         pos[2] = hit->absmax[2] + STEPSIZE;
         node2 = cr_insert_node( pos, NULL, NF_ELEVATOR );
         cr_add_direct_route_uni( node, node2 );
         continue;
         }
       if (hit->use == door_use) {
         pos[0] = (hit->absmax[0]+hit->absmin[0])/2;
         pos[1] = (hit->absmax[1]+hit->absmin[1])/2;
         pos[2] = (hit->absmax[2]+hit->absmin[2]*3)/4;
         node = cr_insert_node( pos, NULL, NF_DOOR );
         continue;
         }

       if (!hit->item || !hit->item->pickup) continue; 

       // insert new node
       node = cr_insert_node( hit->s.origin, NULL, 0 );
       node->item = hit;

       count++;
       }

    cr_routes_load();
}

void cr_check_pending_bots()
{
    int      i, pn;
    edict_t* new_bot;

    for ( i=0; i<global_bots_count; i++ ) {
       if (!global_bots[i].b_inuse || global_bots[i].playernum>=0) continue;
       pn = cr_find_unused_client_slot();
       if (pn<0) break;
       global_bots[i].playernum = pn;
       new_bot = cr_respawn_bot( global_bots+i, NULL );
       if (new_bot) {
         new_bot->classname = "new_bot";
         cr_update_userinfo(new_bot->bot_pers);
         new_bot->client->resp.ctf_team = CTF_NOTEAM;
         }
       }
}

#ifdef _DEBUG
void cr_debug_draw ( edict_t* self )
{
    edict_t *hit;

   //!!! remove path indication
    { edict_t* line, *next_line;
    for ( line = global_path_route; line!=NULL; line=next_line ) {
       next_line = line->teamchain;
       G_FreeEdict(line);
       }
    } 

    for ( hit=g_edicts+1; hit<&g_edicts[globals.num_edicts]; hit++) {
       if (!hit->inuse) continue;
       if ((hit->svflags & SVF_NOCLIENT) || (hit->solid==SOLID_NOT)) continue;

       if (hit->item && hit->item->pickup==CTFPickup_Flag) {
         self->bot_info = gi.TagMalloc ( sizeof(bot_info_t), TAG_GAME );
         memset( self->bot_info, 0, sizeof(bot_info_t) );
         self->bot_pers = gi.TagMalloc ( sizeof(bot_info_pers_t), TAG_GAME );
         memset(self->bot_pers,0,sizeof(bot_info_pers_t));
         cr_find_route( self, hit->s.origin, true );
         gi.TagFree(self->bot_info); 
         self->bot_info=NULL;
         gi.TagFree(self->bot_pers); 
         self->bot_pers=NULL;
         }
       }

}
#endif

void cr_node_keeper_think ( edict_t* self )
{
    int      count;
    edict_t *client;
    path_node_t *node;
    path_node_t *next_node;

    self->nextthink = level.time + 0.3f;
    if (!cr_node_head) cr_init_node_net();
    cr_check_pending_bots();

//    rcount=0;
    node_count=0;
    count = random()*11;
    for ( node=cr_node_head; node!=NULL; node=next_node ) {
       node_count++;
       next_node = node->next;
       if (!next_node) break;
       if (count++<11) continue;
       if ( !next_node->link_from[2/*MAX_NODE_LINKS/2*/] &&
            (level.time - next_node->time)>10) {
         cr_add_links_radius(next_node);
         next_node->time = level.time;
         count=0;
         }
       }
   // if ((node_count%10)==0) gi.bprintf ( PRINT_MEDIUM, "nodes: %d  removed: %d\n", node_count, rcount );

    if ( node_count<MAX_NODE_COUNT*(0.7f+0.5f*random()) ) {
      trace_t trace;
      vec3_t  dir, start, end;
      int     flags;

      for ( client=g_edicts+1; client<=(g_edicts+game.maxclients); client++) {
         if (!client->inuse || !client->client || stricmp(client->classname, "player")!=0 ) continue;

         flags = 0;
         if (client->watertype & (CONTENTS_LAVA|CONTENTS_SLIME)) continue;
         if (!client->groundentity && client->waterlevel==0) {
           AngleVectors( client->client->v_angle, dir, NULL, NULL );
           VectorCopy( client->s.origin, start );
           VectorMA ( start, 60, dir, end );
           trace = gi.trace ( start, client->mins, client->maxs, end, client, CONTENTS_LADDER | CONTENTS_SOLID );
           if ( !(trace.contents & CONTENTS_LADDER) ) continue;
           flags |= NF_LADDER;
           }

         cr_register_position( client, flags );

        // cr_debug_draw(client);
         }
      }
}

void cr_init_node_keeper()
{
    if (cr_node_keeper) return;

    cr_node_keeper = G_Spawn();

    cr_node_keeper->classname = "node keeper";
    cr_node_keeper->svflags |= SVF_NOCLIENT;
    cr_node_keeper->solid = SOLID_NOT;

    cr_node_keeper->think = cr_node_keeper_think;
    cr_node_keeper->nextthink = level.time + 1.5f;

    gi.linkentity(cr_node_keeper);
}

void cr_join_ctf( edict_t *self )
{
    int team;

    if (self->bot_pers->team_no>0) {
      if (self->bot_pers->team_no&1) team = CTF_TEAM1;
                                else team = CTF_TEAM2;
      if (self->client->resp.ctf_team==team) return;
      }
    else 
    if ( self->client->resp.ctf_team==CTF_TEAM1 ||
         self->client->resp.ctf_team==CTF_TEAM2 ) {
      team = self->client->resp.ctf_team;
      }
    else {
      edict_t *player;
      int i, team1count = 0, team2count = 0;

      for (i = 1; i <= maxclients->value; i++) {
         player = &g_edicts[i];

         if (!player->inuse || player==self)
             continue;

         switch (player->client->resp.ctf_team) {
             case CTF_TEAM1: team1count++; break;
             case CTF_TEAM2: team2count++; break;
             }
         }

      team = CTF_TEAM1;
      if (team2count<team1count || 
          (team2count==team1count && (rand()&1))) team = CTF_TEAM2;
      }

    CTFJoinTeam(self,team);
}


float last_bot_respawn_time=0;

void cr_respawn( edict_t *self )
{
    vec3_t  spawn_origin, spawn_angles;
    int     i;

    if (level.time<last_bot_respawn_time) last_bot_respawn_time=level.time;
    if (!cr_node_head || level.time<(last_bot_respawn_time+0.6f)) {
      self->nextthink = level.time + FRAMETIME;
      return;
      }
    last_bot_respawn_time=level.time;

    if ( strcmp(self->classname,"new_bot")==0 ) {
      if (self->bot_pers->team_no>0) 
        gi.bprintf ( PRINT_MEDIUM, "%s has entered the game, team: %d\n", 
                                   self->client->pers.netname, self->bot_pers->team_no );
      else
        gi.bprintf ( PRINT_MEDIUM, "%s has entered the game\n", self->client->pers.netname );
      }

    if (ctf->value) cr_join_ctf(self);

    // spawn the bot on a spawn spot
    SelectSpawnPoint( self, spawn_origin, spawn_angles );
    spawn_origin[2] += 3.f; // make sure off ground
    self->ideal_yaw = spawn_angles[YAW];
    VectorCopy ( spawn_origin, self->s.origin );
    VectorCopy ( self->s.origin, self->s.old_origin );

    // clear playerstate values
    memset (&self->client->ps, 0, sizeof(self->client->ps));

    self->client->ps.pmove.origin[0] = spawn_origin[0]*8;
    self->client->ps.pmove.origin[1] = spawn_origin[1]*8;
    self->client->ps.pmove.origin[2] = spawn_origin[2]*8;

// set the delta angle
    for (i=0; i<3; i++)
        self->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(spawn_angles[i] - self->client->resp.cmd_angles[i]);
    self->s.angles[PITCH] = 0;
    self->s.angles[YAW] = spawn_angles[YAW];
    self->s.angles[ROLL] = 0;
    VectorCopy ( self->s.angles, self->client->ps.viewangles);
    VectorCopy ( self->s.angles, self->client->v_angle);

    // clear entity values
    self->classname = "bot";
    self->groundentity = NULL;
    self->takedamage = DAMAGE_AIM;
    self->deadflag = DEAD_NO;
    self->movetype = MOVETYPE_STEP; //MOVETYPE_NONE;
    self->mass = 200;
    self->solid = SOLID_BBOX;
    self->clipmask = MASK_PLAYERSOLID;
    self->max_health = 100;
    self->gib_health = -40;
    self->waterlevel = 0;
    self->watertype = 0;
    self->health = 100;
    self->max_health = 100;
    self->viewheight = 22;
    self->inuse = true;
    self->yaw_speed = self->bot_pers->rot_speed;
    self->air_finished = level.time + 12;
    self->flags &= ~FL_NO_KNOCKBACK;
    self->svflags &= ~SVF_DEADMONSTER;

    self->bot_info->strafe_dir=1;

    self->bot_info->pickup_target=NULL;
    self->bot_info->pickup_target_score = 1e32;
    self->bot_info->last_node=NULL;
    self->bot_info->next_node=NULL;
    self->bot_info->target_node=NULL;
    self->bot_info->time_last_stuck=0;
    self->bot_info->time_next_enemy=0;
    self->bot_info->time_next_pickup=0;
    self->bot_info->time_next_shot=0;
    self->bot_info->time_last_strafe_switch=0;
    self->bot_info->old_think = NULL;

    self->bot_info->stuck_count = 0;
    self->bot_info->move_block_count = 0;
    self->bot_info->path_nodes = 0;

    self->bot_info->time_next_crouch = level.time + TIME_CROUCH;
    self->bot_info->time_stuck_check = level.time + TIME_CHECK_STUCK;
    VectorCopy ( self->s.origin, self->bot_info->old_origin );

    self->bot_info->time_next_fight_message = level.time + FIGHT_MSG_DELAY;
    self->bot_info->time_last_message=0;

    self->model = self->bot_pers->model;
    self->s.modelindex = 255; //gi.modelindex(self->model);//255;
    self->s.skinnum = self->bot_pers->playernum; //self->bot_pers->skin_no; //game.maxclients-1;
    self->s.frame = 0;

    self->s.modelindex2 = 255;
    self->s.modelindex3 = 0;    // remove linked ctf flag
    self->client->newweapon = self->client->pers.weapon;
    cr_change_weapon (self);

    self->client->anim_priority = ANIM_BASIC;
    self->client->anim_end = 0;

    self->pain = cr_pain;
    self->die = cr_die;

    self->think = cr_think;
    self->nextthink = level.time + FRAMETIME;
    self->bot_info->postthink = cr_post_think;

    self->touch = NULL; //cr_touch;

    self->monsterinfo.stand = NULL;
    self->monsterinfo.walk = NULL;
    self->monsterinfo.run = NULL;
    self->monsterinfo.attack = NULL;
    self->monsterinfo.melee = NULL;
    self->monsterinfo.sight = cr_sight;
    self->monsterinfo.search = NULL;
    self->monsterinfo.checkattack = NULL;

    self->bot_info->b_crouch = false;
    VectorSet( self->mins, -16, -16, -24);
    VectorSet( self->maxs,  16,  16, 32);
    VectorClear( self->velocity );

    self->s.event = EV_PLAYER_TELEPORT;

    KillBox (self);

    gi.linkentity(self);

    self->client->pers.weapon = FindItem ("blaster");
    self->client->newweapon = self->client->pers.weapon;

    cr_pick_random_destination(self); 
}

void cr_show_stats( edict_t *self )
{
    edict_t *bot;

    gi.cprintf( self, PRINT_MEDIUM, "-------------------------------------------------------\n");
    for ( bot=g_edicts+1; bot<=(g_edicts+game.maxclients); bot++) {
       if (!bot->inuse || !bot->client || !bot->bot_info) continue;

       gi.cprintf( self, PRINT_MEDIUM, "%s -- skill:%d adp:%s team:%d frags:%d\n",
                         bot->client->pers.netname,  
                         bot->bot_pers->skill,
                         bot->bot_pers->b_adapting ? "yes" : "no ",
                         bot->bot_pers->team_no,
                        // bot->bot_pers->skin,
                        // bot->bot_pers->model,
                         bot->client->resp.score );
       }
    gi.cprintf( self, PRINT_MEDIUM, "-------------------------------------------------------\n" );
   // gi.cprintf( self, PRINT_MEDIUM, "total nodes in waypoints network = %d\n\n", node_count );
}

void cr_show_team_stats(  edict_t *self  )
{
    edict_t *bot;
    int      n, i, j;
    qboolean b_show;
    char     total[128];

    gi.cprintf( self, PRINT_MEDIUM, "--------- team scores ----------\n");

    for ( i=1; i<20; i++ ) {
       n=0;
       b_show = false;
       for ( bot=g_edicts+1; bot<=(g_edicts+game.maxclients); bot++) {
          if ( !bot->inuse || !bot->client || 
               bot->client->pers.team_no!=i ) continue;

          gi.cprintf( self, PRINT_MEDIUM, "%12s = %d\n",
                            bot->client->pers.netname,  
                            bot->client->resp.score );
          n += bot->client->resp.score;
          b_show = true;
          }
       if (b_show) {
         sprintf ( total, "           === %d", n );
         for ( j=0; j<strlen(total); j++ ) total[j] |= 0x80;
         gi.cprintf( self, PRINT_MEDIUM, "%s\n\n", total );
         }
       }
}


/*
void cr_scoreboard_message( char* string, edict_t *self )
{
#define MAX_SB_BOT 64
#define MAX_LEN    (1400-1)
    gclient_t* bot[MAX_SB_BOT], *cl;
    int        i, j, sl, el, total;
    edict_t   *client;
    char       entry[1024];

    total=0;
    for ( client=g_edicts; client<&g_edicts[globals.num_edicts]; client++ ) {
         if (!client->client) continue;
         if (!client->classname) continue;
         if ( Q_stricmp (client->classname, "bot") &&
              Q_stricmp (client->classname, "player") ) continue;
         bot[total] = client->client;
         total++;
         if (total>=MAX_SB_BOT) break;
         }

    for ( i=0; i<total; i++ ) {
       sl = i;
       el = bot[i]->resp.score;
       for ( j=i+1; j<total; j++ ) {
          if ( el<bot[j]->resp.score ) {
            sl = j;
            el = bot[j]->resp.score;
            } 
          }
       if (sl!=i) {
         cl = bot[i];
         bot[i] = bot[sl];
         bot[sl] = cl;
         }
       }

    if (total>16) total=16;

    sl = strlen(string);
    for ( i=0; i<total; i++ ) {
       if (self->client==bot[i]) {
         char str1[80], str2[8], *pch;
         strncpy(str1,bot[i]->pers.netname,sizeof(str1));
         for( pch=str1; *pch; pch++) *pch|=0x80;
         sprintf(str2, "%d", bot[i]->resp.score);
         for( pch=str2; *pch; pch++) *pch|=0x80;
         Com_sprintf ( entry, sizeof(entry), 
                       "xv 170 yv %i string2 \"%s\" " 
                       "xv 290 yv %i string2 \"%s\" ", 
                       5+i*9, str1,
                       5+i*9, str2 );
         }
       else {
         Com_sprintf ( entry, sizeof(entry), 
                       "xv 170 yv %i string2 \"%s\" " 
                       "xv 290 yv %i string2 \"%i\" ", 
                       5+i*9, bot[i]->pers.netname,
                       5+i*9, bot[i]->resp.score );
         }
       el = strlen(entry);
       if ( (sl+el)>=MAX_LEN ) break;
       strcpy( string+sl, entry );
       sl += el;
       }

}
*/

void cr_get_full_pathname( char* fullname, char* name )
{
    char* pc;

    strcpy( fullname, base_path->string );
    pc = fullname + strlen(fullname)-1;
    while (pc>=fullname && *pc==' ') *pc--='\0';
    if (*pc) {
      if (*pc!=PATHSEPERATOR_CHAR) *++pc=PATHSEPERATOR_CHAR;
      *++pc='\0';
      }
    strcpy( pc, game_path->string ); pc+=strlen(game_path->string);
    strcpy( pc, PATHSEPERATOR_STR ); pc++;
    strcpy( pc, name );
}

qboolean cr_load_message_file( char** msg_array, int* msg_count, char* filename )
{
    FILE  *f;
    char  msg[MAX_MSG_LEN];
    int   i, n;

    for (i=0; i<*msg_count; i++) gi.TagFree(msg_array[i]);
    *msg_count=0;

    cr_get_full_pathname( msg, filename );
    f = fopen( msg, "rt" );
    if (!f) {
      gi.dprintf( "ERROR loading %s\n", msg );
      return false;
      }

    while (1) {
         if (!fgets( msg, sizeof(msg), f )) break;
         n = strlen(msg);
         while(n>0) { if (msg[n-1]!='\r' && msg[n-1]!='\n') break; n--; msg[n]='\0'; }
         if (n<=0) continue;
         msg_array[*msg_count] = gi.TagMalloc( n+2, TAG_GAME );
         strcpy( msg_array[*msg_count], msg );
         (*msg_count)++;
         if (*msg_count>=MAX_MSG) break;
         }

    fclose (f);

    return true;
}


void cr_init()
{
    path_node_t *node, *next_node;

    gi.dprintf ( "-_-_-_-_-_- CRBot init -_-_-_-_-_-\n" );

    cr_init_variables();

    cr_load_message_file( msg_pain, &msg_pain_count, "pain.txt" );
    cr_load_message_file( msg_kill, &msg_kill_count, "kill.txt" );
    cr_load_message_file( msg_death, &msg_death_count, "death.txt" );
    cr_load_message_file( msg_fight, &msg_fight_count, "fight.txt" );
    cr_load_maplist();

    sound_footstep[0] = gi.soundindex ("player/step1.wav");
    sound_footstep[1] = gi.soundindex ("player/step2.wav");
    sound_footstep[2] = gi.soundindex ("player/step3.wav");
    sound_footstep[3] = gi.soundindex ("player/step4.wav");

   // free old node-net
    for ( node=cr_node_head; node!=NULL; node=next_node ) {
       next_node=node->next;
       gi.TagFree(node);
       }
    cr_node_head=NULL;

    if (cr_node_keeper) {
      G_FreeEdict(cr_node_keeper);
      cr_node_keeper=NULL;
      }
    cr_init_node_keeper();

    global_bot_number = 1;
}

void cr_remove_bot( edict_t *bot )
{
   // send effect
    gi.WriteByte( svc_muzzleflash );
    gi.WriteShort( bot-g_edicts );
    gi.WriteByte( MZ_LOGOUT );
    gi.multicast( bot->s.origin, MULTICAST_PVS );

    gi.bprintf ( PRINT_HIGH, "%s disconnected\n", bot->client->pers.netname );
    cr_free_bot( bot );
}

void cr_kill_bot( char* bot_name )
{
    edict_t *hit, *bot;

    bot = NULL;
    for ( hit=g_edicts+1; hit<&g_edicts[globals.num_edicts]; hit++) {
       if (!hit->inuse || !hit->client) continue;
       if (Q_stricmp (hit->classname, "bot")) continue;
       if (Q_stricmp ( hit->client->pers.netname, bot_name )) continue;
       bot = hit;
       break;
       }
    if (bot) {
      for ( hit=g_edicts+1; hit<&g_edicts[globals.num_edicts]; hit++) {
         if (!hit->inuse || !hit->client) continue;
         if (Q_stricmp (hit->classname, "bot")) continue;
         if (hit->enemy==bot) {
           hit->enemy=NULL;
           hit->oldenemy=NULL;
           }
         }
      cr_unregister_bot(bot->bot_pers);
      cr_remove_bot( bot );
      }
}

void cr_client_connect(edict_t *ent)
{
    int      i, playernum, pn;
    edict_t* bot;
    qboolean old_inuse;

    playernum = ent-g_edicts-1;
    old_inuse = ent->inuse;
    ent->inuse = true;

    for ( bot=g_edicts+1; bot<&g_edicts[globals.num_edicts]; bot++) {
       if (!bot->inuse || !bot->bot_pers) continue;
       if (bot->bot_pers->playernum==playernum) {
        // find new playernum 
/*
         pn = cr_find_unused_client_slot();
         if (pn<0) {
           bot->client->inuse = false;
           cr_remove_bot( bot );
           }
         else {
           gclient_t* old_client = bot->client;
           bot->client = &game.clients[pn];
           *bot->client = *old_client;
           old_client->inuse = false;
           bot->client->inuse = true;
           bot->bot_pers->playernum = pn;
           cr_update_userinfo(bot->bot_pers);
           }
*/
         pn = -1; 
         bot->client->inuse = false;
         cr_remove_bot( bot );
        // correct playernum in bot's list
         for ( i=0; i<global_bots_count; i++ ) {
            if (!global_bots[i].b_inuse) continue;
            if (global_bots[i].playernum==playernum) {
              global_bots[i].playernum=pn;
              }
            }
         break;
         }
       }

    ent->inuse = old_inuse;
}


void cr_init_globals()
{
    int size;

    global_bots_count = game.maxclients;

    if (global_bots) gi.TagFree(global_bots);
    size = global_bots_count*sizeof(bot_info_pers_t);
    global_bots = gi.TagMalloc ( size, TAG_GAME );
    memset(global_bots,0,size);

}

void cr_exit_level()
{
    int      i;
    edict_t* bot;

   // mark all bots to respawn on new level
    for ( i=0; i<global_bots_count; i++ ) {
       if (!global_bots[i].b_inuse) continue;
       game.clients[global_bots[i].playernum].inuse = false;
       global_bots[i].playernum = -1;
       } 

    for ( bot=g_edicts+1; bot<=(g_edicts+game.maxclients); bot++) {
       if (!bot->inuse || !bot->client || !bot->bot_info) continue;
       cr_free_bot( bot );
       }
}

void cr_load_maplist()
{
    FILE  *f;
    char  line[256];
    int   n;

    map_cycle_count=0;

    cr_get_full_pathname( line, "maplist.txt" );
    f = fopen( line, "rt" );
    if (!f) {
      gi.dprintf( "ERROR loading maplist.txt\n" );
      return;
      }

    while (map_cycle_count<(MAX_MAP-1)) {
         if (!fgets( line, sizeof(line), f )) break;
    if (!*line || *line=='#' || *line==';' || *line=='/' ||
        *line=='\\' || *line==' ' || *line=='\r' || *line=='\n' ) continue;
         n = strlen(line);
         while(n>0) { if (line[n-1]!='\r' && line[n-1]!='\n') break; n--; line[n]='\0'; }
         if (n<=0) continue;
         strncpy( (char*)(map_list + map_cycle_count++), line, 32 );
         }

    fclose (f);
    next_map %= map_cycle_count;
}

char next_map_name[64]="\0"; 

qboolean cr_map_cycle()
{
    int new_map; 
    if (!(int)map_cycle->value || map_cycle_count<2) return false;

    switch ((int)map_cycle->value) {
          case 2: 
              do { new_map = rand() % map_cycle_count;
                } while( new_map==next_map );
              next_map = new_map;
              break;
          default:
              next_map = (next_map+1) % map_cycle_count;
          } 

    next_map %= map_cycle_count;
    strcpy( next_map_name, (char*)(map_list + next_map) );

    return true;
}

void cr_check_ctf_routes( edict_t *self )
{
    edict_t *hit;
    edict_t *flag1=NULL, *flag2=NULL;
    vec3_t   old_origin;

    for ( hit=g_edicts+1; hit<&g_edicts[globals.num_edicts]; hit++) {
       if (!hit->inuse) continue;
       if (hit->spawnflags & DROPPED_ITEM) continue;
      // if ((hit->svflags & SVF_NOCLIENT) || (hit->solid==SOLID_NOT)) continue;

       if (hit->item && hit->item->pickup==CTFPickup_Flag) {
         if (!flag1) flag1=hit;
                else { flag2=hit; break; }
         }
       }

    if (!flag1) { 
      gi.dprintf("Warning: No flags found on this map!\n");
      return; 
      }
    if (!flag2) { 
      gi.dprintf("Warning: Only one flag found on this map!\n");
      return; 
      }

    VectorCopy( self->s.origin, old_origin );
    VectorCopy( flag1->s.origin, self->s.origin );

    self->bot_info = gi.TagMalloc ( sizeof(bot_info_t), TAG_GAME );
    memset( self->bot_info, 0, sizeof(bot_info_t) );
    self->bot_pers = gi.TagMalloc ( sizeof(bot_info_pers_t), TAG_GAME );
    memset(self->bot_pers,0,sizeof(bot_info_pers_t));
    self->bot_pers->skill = 100;

    if (!cr_find_route( self, flag2->s.origin, true )) {
      gi.dprintf("Warning: No route from one flag to another!\n");
      } 

    gi.TagFree(self->bot_info); 
    self->bot_info=NULL;
    gi.TagFree(self->bot_pers); 
    self->bot_pers=NULL;

    VectorCopy( old_origin, self->s.origin );
}


char file_signature[] = "CRBOT.ROUTE.MAP.01.";

void cr_routes_save( edict_t *self )
{
    path_node_t *node;
    FILE        *f;
    char         filename[256], rt_name[80];
    int          reserved[20], total, saved;

    if (ctf->value) {
      cr_check_ctf_routes(self);
      }

    cr_get_full_pathname( filename, "nodemaps" );
#if defined(WIN32) || defined(_WIN32)
    mkdir(filename);
#else
    mkdir(filename, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

    sprintf( rt_name, "nodemaps%s%s.crn", PATHSEPERATOR_STR, level.mapname );
    cr_get_full_pathname( filename, rt_name );
    f = fopen( filename, "wb" );
    if (!f) {
      gi.dprintf( "ERROR creating file: %s\n", rt_name );
      return;
      }

    if (!fwrite(file_signature,sizeof(file_signature),1,f)) {
      gi.dprintf( "ERROR writing file: %s\n", rt_name );
      fclose(f);
      return;
      }

    memset(filename,0,256);
    if (!fwrite(filename,256,1,f)) {
      gi.dprintf( "ERROR writing file: %s\n", rt_name );
      fclose(f);
      return;
      }

   // save nodes
    memset(reserved,0,sizeof(reserved));
    total=saved=0;
    for ( node=cr_node_head; node!=NULL; node=node->next ) {
       total++; 
       if (node->item) continue;
       saved++; 
       if (!fwrite(reserved,sizeof(reserved),1,f)) {
         gi.dprintf( "ERROR writing file: %s\n", rt_name );
         fclose(f);
         return;
         }
       if (!fwrite(&(node->position),sizeof(node->position),1,f)) {
         gi.dprintf( "ERROR writing file: %s\n", rt_name );
         fclose(f);
         return;
         }
       if (!fwrite(&(node->flags),sizeof(node->flags),1,f)) {
         gi.dprintf( "ERROR friting file: %s\n", rt_name );
         fclose(f);
         return;
         }
       }

    reserved[0]=1;
    if (!fwrite(reserved,sizeof(reserved),1,f)) {
      gi.dprintf( "ERROR writing file: %s\n", rt_name );
      fclose(f);
      return;
      }

    fclose(f);

    gi.dprintf( "total nodes: %d,  saved: %d\n", total, saved );
}

void cr_routes_load()
{
    vec3_t pos;
    int    flags;
    FILE  *f;
    char   filename[256], rt_name[80];
    int    reserved[20], total;

    sprintf( rt_name, "nodemaps%s%s.crn", PATHSEPERATOR_STR, level.mapname );
    cr_get_full_pathname( filename, rt_name );
    f = fopen( filename, "rb" );
    if (!f) return; 

    memset(filename,0,256);
    if (!fread(filename,sizeof(file_signature),1,f)) {
      gi.dprintf( "ERROR reading file: %s\n", rt_name );
      fclose(f);
      return;
      }
    if (strcmp(filename,file_signature)) return;

    if (!fread(filename,256,1,f)) {
      gi.dprintf( "ERROR reading file: %s\n", rt_name );
      fclose(f);
      return;
      }

   //read nodes
    total=0;
    while(true) {
       total++; 
       if (!fread(reserved,sizeof(reserved),1,f)) {
         gi.dprintf( "ERROR reading file: %s\n", rt_name );
         fclose(f);
         return;
         }
       if (reserved[0]) break;
       if (!fread(&pos,sizeof(pos),1,f)) {
         gi.dprintf( "ERROR reading file: %s\n", rt_name );
         fclose(f);
         return;
         }
       if (!fread(&flags,sizeof(flags),1,f)) {
         gi.dprintf( "ERROR reading file: %s\n", rt_name );
         fclose(f);
         return;
         }

       cr_insert_node( pos, NULL, flags );
       }

    fclose(f);

    gi.dprintf( "total nodes read from nodemap file: %d\n", total );
}

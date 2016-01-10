#ifndef __CR_MAIN_H__
#define __CR_MAIN_H__

#define CRBOT_VER  "v1.14"
#define CRBOT_INFO "CRBot " CRBOT_VER ", coded by Mike Malakhov\nwith CTF support\nhttp://www.pobox.com/~ww/bot.html \n\nUse inventory keys to access bot's menu\n\nTo add bots type 'sv addbots 0 <amount>'\nin the console window"

void SP_crbot( char* name, int skill_level, char* skin, int team, char* model );
void cr_kill_bot( char* bot_name );
void cr_team_help( edict_t* self );
void cr_team_group( edict_t* self );
void cr_team_guard( edict_t* self, char* name );
void cr_team_patrol( edict_t* self, char* name );
void cr_team_free( edict_t* self, char* name );

void cr_menu_init();
void cr_menu_draw( edict_t *self );
void cr_menu_select( edict_t *self, int ds );
void cr_menu_down( edict_t *self );
void cr_menu_up( edict_t *self );

void cr_init();
void cr_init_globals();
void cr_client_connect(edict_t *ent);
void cr_exit_level();
void cr_routes_save( edict_t *self );

// void cr_scoreboard_message( char* string, edict_t *self );
void cr_show_stats( edict_t *self );
void cr_show_team_stats( edict_t *self );

extern cvar_t *bot_skill;
extern cvar_t *bot_skin;
extern cvar_t *bot_model;
extern cvar_t *bot_team;
extern cvar_t *bot_chat;
extern cvar_t *bot_taunt;
extern cvar_t *obituary_msgs;

extern cvar_t *bot_menu;

extern cvar_t *game_path;
extern cvar_t *base_path;

extern cvar_t *map_cycle;
extern cvar_t *no_tech;
extern cvar_t *no_hook;

extern cvar_t *bot_debug;

void cr_get_full_pathname( char* fullname, char* name );

void ShowGun( edict_t *ent );

qboolean cr_map_cycle();
extern char next_map_name[64]; 

#endif //__CR_MAIN_H__

#include <stdio.h>
#if defined(WIN32) || defined(_WIN32)
#include <io.h>
#else
#include <dirent.h>
#include <string.h>
#endif

#include "g_local.h"
#include "cr_main.h"

void SP_crbot( char* name, int skill_level, char* skin, int team, char* model );
void cr_show_stats( edict_t *self );

#define MENU_MAIN     1
#define MENU_ADD_BOT  2

#define MI_MAIN_ADDBOT      1
#define MI_MAIN_STATS       2
#define MI_MAIN_TEAMS       3
#define MI_MAIN_MYTEAM      4
#define MI_MAIN_OBITUARY    5
#define MI_MAIN_BOTCHAT     6
#define MI_MAIN_TAUNT       7
#define MI_MAIN_MAPCYCLE    8
#define MI_MAIN_TECHS       9
#define MI_MAIN_HOOK        10
#define MI_MAIN_SAVENODES   11
#define MI_MAIN_MAX         11

#define MI_AB_PREVMENU   1
#define MI_AB_SKILL      2
#define MI_AB_MODEL      3
#define MI_AB_SKIN       4
#define MI_AB_TEAM       5 
#define MI_AB_ADDBOT     6
#define MI_AB_MAX        6

#define MAX_SKIN  1024
#define MAX_MODEL 128

typedef char stname[32];

stname skins[MAX_SKIN], models[MAX_MODEL];
int    model_count=0, current_model=0, current_skin=0;
int    skin_start[MAX_MODEL], skin_count[MAX_MODEL];

void cr_invert_string( char* pc )
{
    while(*pc!='"') { *pc |= 0x80; pc++; }
}

#if defined(WIN32) || defined(_WIN32)

void cr_menu_init()
{
    char   path[256], modeldir[256], skindir[256], *pc; 
    struct _finddata_t c_model, c_skin;
    long   h_model, h_skin;
    int    i, s_count, s_start, n;

    memset(skins,0,sizeof(skins));
    memset(models,0,sizeof(models));

    strcpy( path, base_path->string );
    pc = path + strlen(path)-1;
    while (pc>=path && *pc==' ') *pc--='\0';
    if (*pc) {
      if (*pc!='\\' && *pc!='/') *++pc='\\'; 
      *++pc='\0';
      }

    model_count=0;

    strcpy( modeldir, path );
    strcat( modeldir, "baseq2\\players\\*" );

    s_start=0;
    h_model = _findfirst( modeldir, &c_model );
    if (h_model==-1L) {
      current_model=0;
      current_skin=0;
      return;
      }

    do {
      if (model_count >= MAX_MODEL) { gi.dprintf("too many models"); continue; }

      if ( (c_model.attrib & _A_SUBDIR) && c_model.name[0]!='.' ) {

        strncpy( models[model_count], c_model.name, sizeof(models[0]));

        strcpy(skindir,path);
        strcat(skindir,"baseq2\\players\\");
        strcat(skindir,c_model.name);
        strcat(skindir,"\\*_i.pcx");

        s_count=0;
        h_skin = _findfirst( skindir, &c_skin );
        if (h_skin!=-1L) {
          do {
            pc = skins[s_start+s_count];
            memset( pc, 0, sizeof(skins[0]) );
            n = strlen(c_skin.name)-6; if (n>sizeof(skins[0])) n=sizeof(skins[0]);
            strncpy( pc, c_skin.name, n );
            s_count++;
            } while( _findnext( h_skin, &c_skin ) == 0 );
          _findclose( h_skin );
          }

        if (s_count>0) {
          skin_start[model_count] = s_start;
          skin_count[model_count] = s_count;
          model_count++;
          s_start += s_count;
          }
        }
      } while( _findnext( h_model, &c_model ) == 0 );

    _findclose( h_model );

    if (current_model>=model_count) current_model = model_count-1;
    for ( i=0; i<model_count; i++ ) {
       if ( stricmp( bot_model->string, models[i] )==0 )  {
         current_model = i;
         break; 
         }
       }
        
    if (current_skin>=skin_count[current_model]) current_skin = skin_count[current_model]-1;
    for ( i=0; i<skin_count[current_model]; i++ ) {
       if ( stricmp( bot_skin->string, skins[skin_start[current_model]+i] )==0 )  {
         current_skin = i;
         break; 
         }
       }

    gi.cvar_forceset( "bot_model", models[current_model] );
    gi.cvar_forceset( "bot_skin", skins[skin_start[current_model]+current_skin] );
}

#else // !WIN32

static int
ismodeldir (const struct dirent *thisdir)
{
	if (thisdir->d_name[0] == '.') return 0;
	//if (thisdir->d_type != DT_DIR) return 0; //always returns zero ?
	return 1;
}

static int
isskinpcx (const struct dirent *thisdir)
{
	if (strstr(thisdir->d_name, "_i.pcx") != 0) return 1;
	return 0;
}

void cr_menu_init()
{
	char   path[256], modeldir[256], skindir[256], *pc; 
   int    l, m, n, s_count, s_start;
   struct dirent **modelnames;
   struct dirent **skinnames;

   s_start = 0;
   memset(skins,0,sizeof(skins));
   memset(models,0,sizeof(models));

   // collect model names
   strcpy(path, base_path->string);
   strcpy(modeldir, path);
   strcat(modeldir, "/baseq2/players");

   m = scandir(modeldir, &modelnames, ismodeldir, alphasort);
   if (m < 0) gi.error("no models found");
   if (m > MAX_MODEL) gi.error("too many models");

   for (model_count=0; model_count < m; model_count++) {
		strncpy(models[model_count],
			  modelnames[model_count]->d_name,
			  sizeof(models[0]));
		free (modelnames[model_count]);

		// collect skin names
		strcpy(skindir, modeldir);
		strcat(skindir, "/");
		strcat(skindir, models[model_count]);

		n = scandir(skindir, &skinnames, isskinpcx, alphasort);
		if (n < 0) continue;

		for (s_count=0; s_count < n; s_count++) {
			pc = skins[s_start + s_count];
			memset(pc, 0, sizeof(skins[0]));
			l = strlen(skinnames[s_count]->d_name) - 6;
			if (l > sizeof(skins[0])) l = sizeof(skins[0]);
			strncpy(pc, skinnames[s_count]->d_name, l);
			free (skinnames[s_count]);
		}
		free (skinnames);

		skin_start[model_count] = s_start;
		skin_count[model_count] = s_count;
		s_start += s_count;
	}
	free (modelnames);
	// collect done

	for ( n=0; n<model_count; n++ ) {
		if ( strcasecmp( bot_model->string, models[n] )==0 )  {
			current_model = n;
			break; 
		}
	}
	for ( n=0; n<skin_count[current_model]; n++ ) {
		if ( strcasecmp( bot_skin->string,
			  skins[skin_start[current_model]+n] )==0 )  {
			current_skin = n;
			break; 
		}
	}
	gi.cvar_forceset("bot_model", models[current_model]);
	gi.cvar_forceset("bot_skin", 
		skins[skin_start[current_model] + current_skin]);
}

#endif // WIN32

void cr_menu_draw( edict_t *self )
{
    char  menu[1400], *pch;
    gclient_t *cl;

    cl = self->client;
    if (!cl || self->bot_info) return;

    if (cl->menu_no<=0) cl->menu_no=1;
    if (cl->menu_item_no<=0) cl->menu_item_no=0;

    pch = menu;
    sprintf ( pch, "xv 32 yv 8 picn inventory " );
    pch += strlen(pch);

    switch (cl->menu_no) {
          case MENU_MAIN:
              sprintf( pch, "xv 0 yv 30 cstring2 \"[CRBot] main menu:\" "
                            "xv 0 yv 32 cstring2 \"__________________\" " );
              break;
          case MENU_ADD_BOT:
              sprintf( pch, "xv 0 yv 30 cstring2 \"[CRBot] add new bot:\" "
                            "xv 0 yv 32 cstring2 \"____________________\" " );
              break;
          default:
              cl->menu_no=0;
              return;
          };
    pch += strlen(pch);

    switch (cl->menu_no) {
          case MENU_MAIN:
              sprintf( pch, "xv 56 yv 50 string2 \"add new bot...\" " ); 
              if (cl->menu_item_no!=MI_MAIN_ADDBOT) cr_invert_string(pch+21); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 60 string2 \"show bots statistics...\" " ); 
              if (cl->menu_item_no!=MI_MAIN_STATS) cr_invert_string(pch+21); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 70 string2 \"show team scores...\" " ); 
              if (cl->menu_item_no!=MI_MAIN_TEAMS) cr_invert_string(pch+21); pch += strlen(pch);
              if (self->client->pers.team_no)
                sprintf( pch, "xv 56 yv 80 string2 \"player's team     = %d\" ", self->client->pers.team_no ); 
              else
                sprintf( pch, "xv 56 yv 80 string2 \"player's team     = none\" " ); 
              if (cl->menu_item_no!=MI_MAIN_MYTEAM) cr_invert_string(pch+21); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 90 string2 \"obituary messages = %s\" ", obituary_msgs->value ? "show" : "skip" ); 
              if (cl->menu_item_no!=MI_MAIN_OBITUARY) cr_invert_string(pch+21); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 100 string2 \"bot chat          = %s\" ", bot_chat->value ? "on" : "off" ); 
              if (cl->menu_item_no!=MI_MAIN_BOTCHAT) cr_invert_string(pch+22); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 110 string2 \"bot taunts        = %s\" ", bot_taunt->value ? "on" : "off" ); 
              if (cl->menu_item_no!=MI_MAIN_TAUNT) cr_invert_string(pch+22); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 120 string2 \"map cycle         = %s\" ", ((int)map_cycle->value)==2 ? "random" : ((int)map_cycle->value)==1 ? "on" : "off" );
              if (cl->menu_item_no!=MI_MAIN_MAPCYCLE) cr_invert_string(pch+22); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 130 string2 \"techs in dm game  = %s\" ", !((int)no_tech->value) ? "enable" : "disable" );
              if (cl->menu_item_no!=MI_MAIN_TECHS) cr_invert_string(pch+22); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 140 string2 \"g.hook in dm game = %s\" ", !((int)no_hook->value) ? "enable" : "disable" );
              if (cl->menu_item_no!=MI_MAIN_HOOK) cr_invert_string(pch+22); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 150 string2 \"save nodemap file...\" " );
              if (cl->menu_item_no!=MI_MAIN_SAVENODES) cr_invert_string(pch+22); pch += strlen(pch);
              break;
          case MENU_ADD_BOT:
              sprintf( pch, "xv 56 yv 50 string2 \"...back to main menu...\" " ); 
              if (cl->menu_item_no!=MI_AB_PREVMENU) cr_invert_string(pch+21); pch += strlen(pch);
              if (bot_skill->value)
                sprintf( pch, "xv 56 yv 60 string2 \"skill  = %d\" ", (int)bot_skill->value ); 
              else
                sprintf( pch, "xv 56 yv 60 string2 \"skill  = adapting\" " ); 
              if (cl->menu_item_no!=MI_AB_SKILL) cr_invert_string(pch+21); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 70 string2 \"model   = %s\" ", bot_model->string ); 
              if (cl->menu_item_no!=MI_AB_MODEL) cr_invert_string(pch+21); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 80 string2 \"skin    = %s\" ", bot_skin->string ); 
              if (cl->menu_item_no!=MI_AB_SKIN) cr_invert_string(pch+21); pch += strlen(pch);
              if (bot_team->value)
                sprintf( pch, "xv 56 yv 90 string2 \"team    = %d\" ", (int)bot_team->value ); 
              else
                sprintf( pch, "xv 56 yv 90 string2 \"team    = none\" " ); 
              if (cl->menu_item_no!=MI_AB_TEAM) cr_invert_string(pch+21); pch += strlen(pch);
              sprintf( pch, "xv 56 yv 100 string2 \"-> add new bot <-\" " ); 
              if (cl->menu_item_no!=MI_AB_ADDBOT) cr_invert_string(pch+22); pch += strlen(pch);
              break;
          };

   gi.configstring ( CS_STATUSBAR, menu );
}


void cr_menu_select( edict_t *self, int ds )
{
    gclient_t *cl;
    int        no;
    char       buf[8];

    cl = self->client;
    if (!cl || self->bot_info) return;

    switch (cl->menu_no) {
          case MENU_MAIN:
              switch (cl->menu_item_no) {
                    case MI_MAIN_ADDBOT:
                        cl->menu_no = MENU_ADD_BOT;
                        cl->menu_item_no = 1;
                        break;
                    case MI_MAIN_OBITUARY:
                        gi.cvar_forceset( "obituary_msgs", obituary_msgs->value ? "0" : "1" );
                        break;
                    case MI_MAIN_MYTEAM:
                        self->client->pers.team_no += ds;
                        if (self->client->pers.team_no>10) self->client->pers.team_no=0;
                        if (self->client->pers.team_no<0) self->client->pers.team_no=10;
                        break;
                    case MI_MAIN_STATS:
                        cr_show_stats(self);
                        break; 
                    case MI_MAIN_TEAMS:
                        cr_show_team_stats(self);
                        break; 
                    case MI_MAIN_BOTCHAT:
                        gi.cvar_forceset( "bot_chat", bot_chat->value ? "0" : "1" );
                        break; 
                    case MI_MAIN_TAUNT:
                        gi.cvar_forceset( "bot_taunt", bot_taunt->value ? "0" : "1" );
                        break; 
                    case MI_MAIN_MAPCYCLE:
                        gi.cvar_forceset( "map_cycle", va( "%d", ((int)map_cycle->value+ds+3)%3 ) );
                        break; 
                    case MI_MAIN_TECHS:
                        gi.cvar_forceset( "no_tech", no_tech->value ? "0" : "1" );
                        break; 
                    case MI_MAIN_HOOK:
                        gi.cvar_forceset( "no_hook", no_hook->value ? "0" : "1" );
                        break; 
                    case MI_MAIN_SAVENODES:
                        cr_routes_save(self);
                        break; 
                    }
              break;

          case MENU_ADD_BOT:
              switch (cl->menu_item_no) {
                    case MI_AB_PREVMENU:
                        cl->menu_no = MENU_MAIN;
                        cl->menu_item_no = 1;
                        break;
                    case MI_AB_MODEL:
                        current_model+=ds;
                        if (current_model>=model_count) current_model=0;
                        if (current_model<0) current_model=model_count-1;
                        gi.cvar_forceset( "bot_model", models[current_model] );
                        if (current_skin>=skin_count[current_model]) 
                          current_skin = skin_count[current_model]-1;
                        gi.cvar_forceset( "bot_skin", skins[skin_start[current_model]+current_skin] );
                        break;
                    case MI_AB_SKIN:
                        if (current_model>=model_count) current_model=model_count-1;
                        current_skin+=ds; 
                        if (current_skin>=skin_count[current_model]) current_skin=0;
                        if (current_skin<0) current_skin = skin_count[current_model]-1;
                        gi.cvar_forceset( "bot_skin", skins[skin_start[current_model]+current_skin] );
                        break; 
                    case MI_AB_TEAM:
                        no = ((int)bot_team->value + ds + 10) % 10;
                        sprintf(buf,"%d",no);
                        gi.cvar_forceset( "bot_team", buf );
                        break;
                    case MI_AB_SKILL:
                        no = ((int)bot_skill->value + ds + 10) % 10;
                        sprintf(buf,"%d",no);
                        gi.cvar_forceset( "bot_skill", buf );
                        break;
                    case MI_AB_ADDBOT:
                        SP_crbot( NULL, bot_skill->value, bot_skin->string, bot_team->value, bot_model->string );
                        break;
                    }
              break;
          }

    cr_menu_draw(self);
}

void cr_menu_up( edict_t *self )
{
    gclient_t *cl;

    cl = self->client;
    if (!cl || self->bot_info) return;

    cl->menu_item_no--;
    if (cl->menu_item_no<=0) 
      switch (cl->menu_no) {
            case MENU_MAIN:
                cl->menu_item_no=MI_MAIN_MAX;
                break;
            case MENU_ADD_BOT:
                cl->menu_item_no=MI_AB_MAX;
                break;
            default: 
                cl->menu_item_no=1;
            }

    cr_menu_draw(self);
}

void cr_menu_down( edict_t *self )
{
    gclient_t *cl;

    cl = self->client;
    if (!cl || self->bot_info) return;

    cl->menu_item_no++;
    switch (cl->menu_no) {
          case MENU_MAIN:
              if (cl->menu_item_no>MI_MAIN_MAX) cl->menu_item_no=1;
              break;
          case MENU_ADD_BOT:
              if (cl->menu_item_no>MI_AB_MAX) cl->menu_item_no=1;
              break;
          default: 
              cl->menu_item_no=1;
          }

    cr_menu_draw(self);
}


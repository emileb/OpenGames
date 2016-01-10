
#include "g_local.h"
#include "cr_main.h"


void	Svcmd_Test_f (void)
{
	gi.cprintf (NULL, PRINT_HIGH, "Svcmd_Test_f()\n");
}

/*
=================
ServerCommand

ServerCommand will be called when an "sv" command is issued.
The game can issue gi.argc() / gi.argv() commands to get the rest
of the parameters
=================
*/
void	ServerCommand (void)
{
	char	*cmd;

	cmd = gi.argv(1);
	if (Q_stricmp (cmd, "test") == 0)
		Svcmd_Test_f ();
	else if (Q_stricmp (cmd, "addbots") == 0) 
        {
        char* p_skill = gi.argv(2);
        int   i, n = atoi(gi.argv(3)), skill;
        if (p_skill[1]=='.' && p_skill[2]=='.') {
          int min_skill, max_skill;
          p_skill[1]='\0';
          min_skill=atoi(p_skill);   if (min_skill<1) min_skill=1;   if (min_skill>10) min_skill=10;
          max_skill=atoi(p_skill+3); if (max_skill<1) max_skill=1;   if (max_skill>10) max_skill=10;
          if (n<=0) n=1;
          for (i=0; i<n; i++)
             SP_crbot( NULL, min_skill+(max_skill-min_skill+1)*random()*0.999f, bot_skin->string, bot_team->value, bot_model->string );
          }
        else {
          skill = atoi(p_skill);
          if (n<=0) n=1;
          for (i=0; i<n; i++) SP_crbot( NULL, skill, bot_skin->string, bot_team->value, bot_model->string );
          }
        }
    else if (Q_stricmp (cmd, "addbot") == 0) 
        {
        if (gi.argv(2) && *gi.argv(2)) 
          SP_crbot( gi.argv(2), atoi(gi.argv(3)), gi.argv(5), atoi(gi.argv(6)), gi.argv(4) );
        }
	else if (Q_stricmp (cmd, "killbot") == 0) 
        cr_kill_bot(gi.argv(2));
    else
		gi.cprintf (NULL, PRINT_HIGH, "Unknown server command \"%s\"\n", cmd);
}


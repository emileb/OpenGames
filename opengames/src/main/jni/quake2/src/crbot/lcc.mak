#
# Makefile for the CRBot game source: gamex86.dll
# Intended for LCC-Win32
# Mon Jän 21 01:48:27 CET 2002
#

CC=lcc
CFLAGS=-DC_ONLY -o
LD=lcclnk
LDFLAGS=-dll -entry GetGameAPI

OBJS=lcchack.obj\
	cr_main.obj \
	cr_menu.obj \
	g_ai.obj \
	g_chase.obj\
	g_cmds.obj \
	g_combat.obj \
	g_ctf.obj\
	g_func.obj \
	g_items.obj \
	g_main.obj \
	g_misc.obj \
	g_monster.obj \
	g_phys.obj \
	g_save.obj \
	g_spawn.obj \
	g_svcmds.obj \
	g_target.obj \
	g_trigger.obj \
	g_utils.obj \
	g_weapon.obj \
	m_move.obj \
	p_client.obj \
	p_hud.obj \
	p_menu.obj\
	p_trail.obj \
	p_view.obj \
	p_weapon.obj \
	q_shared.obj
	
all: gamex86.dll

%.obj: %.c
	$(CC) $(CFLAGS) $<

gamex86.dll: $(OBJS)
	$(LD) $(LDFLAGS) *.obj lccgame.def -o gamex86.dll

.PHONY: clean
clean:
	del *.obj gamex86.dll

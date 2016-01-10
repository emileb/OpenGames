#
# Makefile for CRbot
# gcc / Linux-i386
# Mon Jän 21 01:47:58 CET 2002
#

CC=gcc

ARCH=i386

#DEBUG=-g -Wall #-D_DEBUG
OPTIM=-mcpu=pentiumpro -march=pentiumpro -O3 -ffast-math -funroll-loops \
	-malign-loops=2 -malign-jumps=2 -malign-functions=2 \
	-fexpensive-optimizations -pipe
	#-fomit-frame-pointer # 2.96 has problems with this one

CFLAGS=$(DEBUG)$(OPTIM) -Dstricmp=strcasecmp

SHLIBEXT=so
SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

OBJS=cr_main.o\
	cr_menu.o\
	g_ai.o\
	g_chase.o\
	g_cmds.o\
	g_combat.o\
	g_ctf.o\
	g_func.o\
	g_items.o\
	g_main.o\
	g_misc.o\
	g_monster.o\
	g_phys.o\
	g_save.o\
	g_spawn.o\
	g_svcmds.o\
	g_target.o\
	g_trigger.o\
	g_utils.o\
	g_weapon.o\
	m_move.o\
	p_client.o\
	p_hud.o\
	p_menu.o\
	p_trail.o\
	p_view.o\
	p_weapon.o\
	q_shared.o

.c.o:
	$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<

game$(ARCH).$(SHLIBEXT): $(OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJS)

install: gamei386.so
	cp gamei386.so ..

#
# misc
#

.PHONY : clean tags depend dist
clean :
	-rm -f zwadconv $(OBJS) game$(ARCH).$(SHLIBEXT) *~

tags:
	ctags *.[ch]
	cscope -b
depend:
	gcc -M $(OBJS:.o=.c) >.depend

DISTFILES=LICENSE.TXT README README2 Makefile Changelog\
	game.def game.dsp game.dsw\
	lccmake.bat lcc.mak lccgame.def lcchack.c\
	cr_main.c\
	cr_main.h\
	cr_menu.c\
	g_ai.c\
	g_chase.c\
	g_cmds.c\
	g_combat.c\
	g_ctf.c\
	g_ctf.h\
	g_func.c\
	g_items.c\
	g_local.h\
	g_main.c\
	g_misc.c\
	g_monster.c\
	g_phys.c\
	g_save.c\
	g_spawn.c\
	g_svcmds.c\
	g_target.c\
	g_trigger.c\
	g_utils.c\
	g_weapon.c\
	game.h\
	m_move.c\
	m_player.h\
	p_client.c\
	p_hud.c\
	p_menu.c\
	p_menu.h\
	p_trail.c\
	p_view.c\
	p_weapon.c\
	q_shared.c\
	q_shared.h

DATE=`date +%y%m%d`

dist:
	@( cd .. && echo $(DISTFILES) | \
	sed -e "s: : src/:g" -e "s:^:src/:" | \
	xargs tar cvfj crbot-src-$(DATE).tar.bz2 )
	@zip -r ../crbot-src-$(DATE).zip $(DISTFILES)

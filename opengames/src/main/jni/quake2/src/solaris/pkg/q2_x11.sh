#!/bin/sh
/usr/local/share/games/quake2/sdlquake2 \
	+set basedir /usr/local/share/games/quake2 \
	+set vid_ref softx \
	+set nocdaudio 1 \
	+set gl_dynamic 1 \
	+set gl_ext_multitexture 0 \
	+set gl_shadows 1 \
	+set gl_stencilshadow 1 \
	$*


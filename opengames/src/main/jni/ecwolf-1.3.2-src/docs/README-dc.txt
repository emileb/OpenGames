Wolf4SDL\DC 1.5
ported to Dreamcast by fackue

A port of Wolf4SDL by Moritz "Ripper" Kroll.

What's new in 1.5:
- Mouse support
- Weapon switch support
- Controllers with C & Z button support
- Command line parameters via 'params.txt' support
- SOD mission pack support via menu
- Commercial CD-ROM swap support

Instructions:
- Extract the Wolf4SDL\DC archive to it's own folder.
- Put your *.wl6 files inside /wolf3d (if you have a commercial CD-ROM release
  of Wolfenstein you can skip this step).
- If you want use the command line, create a text file called 'params.txt'
  (without quotes) in the folder you extracted Wolf4SDL\DC to and add any
  valid parameters. Please keep them on one line.
- Download and install BootDreams (http://dchelp.dcemulation.org/?BootDreams).
- Open the folder you extracted Wolf4SDL\DC to in BootDreams.
- Select a task. If you'd like to burn Wolf4SDL\DC directly to a CD-R, select
  the CDRecord task and set the disc format to either Audio\Data or Data\Data
  (both do essentially the same thing).
- Click Process.
- If you created a CD image, burn it with the appropriate burning program
  e.g. Alcohol 120%.
- Pop the CD-R in your Dreamcast and play.
- If you have a commercial CD-ROM, wait until you're asked to swap in your
  copy of Wolfenstein 3D, swap your copy in, press start and play.

Valid Parameters:
--goobers (Wolfenstein 3D only)
--debugkeys (Spear of Destiny only)
--goodtimes (Spear of Destiny only)
--nowait
--baby
--easy
--normal
--hard
--tedlevel <level>
--res <width> <height> (default 320x200)
--samplerate <rate> (default 11025)
--audiobuffer <size> (default 4096)

Notes:
- The --res parameter will also except '640 400' but will slow rendering down
  greatly.
- The default samplerate is 11025 to keep the extra SOD missions from running
  out of memory. Wolfenstein 3D and the original SOD mission will happily run
  with the samplerate set to 22050.

Compiling:
This port was developed under Code::Blocks 8.02 with the DC Dev ISO R4
integrated within it. I've also included a Makefile in case you don't use
Code::Blocks. You will need SDL and SDL mixer to compile Wolf4SDL\DC.

DC Dev ISO can be downloaded from here:
http://dcemulation.org/?title=DC_Dev_ISO

It contains everything you need to compile Wolf4SDL. It includes Code::Blocks
8.02 and a tool to integrate Code::Blocks with DC Dev ISO. It uses Cygwin as
the terminal environment and includes the DC compilers and all the SDL
libraries that are needed to compile Wolf4SDL.

To compile Wolf4SDL\DC under Code::Blocks, extract the source to it's own
folder, open 'Wolf4SDL-DC.cbp' and goto 'Build' | 'Build'.

To compile Wolf4SDL\DC using the supplied Makefile, extract the source to it's
own directory in C:\cygwin\usr\local\dc and type 'make -f Makefile.dc' in your
terminal while inside the folder Wolf4SDL\DC was extracted to. By default the
Makefile will create a scrambled 1ST_READ.BIN, unscrambled Wolf4SDL.bin and of
course, Wolf4SDL.elf.

Credits & Thanks:
- OneThiryt8 for which parts of this port was based on his port, sdlWolf
- BlueCrab who wrote parts of the swap disc menu
- BlackAura for which I stole parts of his menu code from nxDoom
- Ripper for such a clean and friendly Wolfenstein 3D engine
- Bero for the Dreamcast port of SDL, which this port uses
- Dan Potter and team for KOS, which this port uses

RtCW-OpenPandora
================

Return to Castle Wolfenstein for OpenPandra (featuring ARM support and GLES renderer)

OpenPandora
===========

This fork is aimed at OpenPandra, so get:
 * ARM compatibilty (using -DARM)
 * Some NEON optimized code (using -DNEON)
 * GLES renderer (using -DHAVE_GLES)
 * Toggle Crouch function (using -DCROUCH), disabled by default, with a new variable in cfg to activate
 * Some Aim assist (using -AUTOAIM), where the mouse slow down and acceleration is set to 0 wen aiming someone not friendly
 * OpenPandora support of course (using -DPANDORA), for screen resolution mainly.
 
For the Pandora version, I used some Hardwired modif in the Construct file
To compile (using code::blocks PND), go inside src/unix and type "./cons -- release" to construct full release version

For more info on the OpenPandora go here: http://boards.openpandora.org/
Specific thread for Return to Castle Wolfenstein on the OpenPandora here: http://boards.openpandora.org/index.php/topic/13997-return-to-castle-wolfenstein/

/*
 * quake2-jni.h
 */

#ifndef QUAKE2JNI_H_
#define QUAKE2JNI_H_


// pass data between JNI and the Quake 2 code
// use only functions and basic C data types

void quake2_jni_key_event( int key, int down);

void quake2_jni_move_event( int mode, 
			int forwardmove, int sidemove, int upmove,
			float pitch, float yaw, float roll
			);

void quake2_jni_reset_framecount();

int quake2_jni_get_framecount();


#endif /* QUAKE2JNI_H_ */

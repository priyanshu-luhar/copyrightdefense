/* Features of the game */

// Start of game menu
// Game Over Screen
// Potential skin for main guy, no character select
// Potential Original Character as the main guy
// Collectible buffs on the floor 
// More than one map
// Multiple sprites and sprite sheets used
// Keyboard input, no mouse input
// Collision between the player character and the enemies
// Multiple animations implemented (Attack, hit, etc.)
// Dynamic Health bar
// Dynamic statuses that affect the player and enemies
#ifdef USE_OPENAL_SOUND
#include <stdio.h> //
#include <string.h> //
#include <math.h> //
#include <unistd.h>
#include <fcntl.h> //
#include <sys/stat.h> //
#include </usr/include/AL/alut.h>
#endif //USE_OPENAL_SOUND

#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <cstring>
#include "jhernandez2.h"
#include "header.h"

using namespace std;
/*
int total_running_time(const bool get)
{
    static int firsttime = 1;
    static int start_time;

    if (firsttime) {
        start_time = time(NULL);
        firsttime = 0;
    }
    if (get) {
        return time(NULL) - start_time;
    }
    return 0;
}
*/
int time_since_mouse_move(const bool get)
{
    static int start_time;
    static int firsttime = 1;

    if (get == false) {
        if (firsttime == 0)
            start_time = time(NULL);
    }
    if (firsttime) {
        start_time = time(NULL);
        firsttime = 0;
    }
    if (get) {
        return time(NULL) - start_time;
    }
    return 0;
}

void play_sound(int select)
{
#ifdef USE_OPENAL_SOUND
    //Get started right here.
    alutInit(0, NULL);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: alutInit()\n");
        // return 0;
    }
    //Clear error state.
    alGetError();
    //
    //Setup the listener.
    //Forward and up vectors are used.
    float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);
    //
    //Buffer holds the sound information.
    ALuint alBuffer;
    alBuffer = alutCreateBufferFromFile("./Background_Music.wav");
    //
    //Source refers to the sound.
    ALuint alSource;
    //Generate a source, and store it in a buffer.
    alGenSources(1, &alSource);
    alSourcei(alSource, AL_BUFFER, alBuffer);
    //Set volume and pitch to normal, no looping of sound.
    alSourcef(alSource, AL_GAIN, 1.0f);
    alSourcef(alSource, AL_PITCH, 1.0f);
    alSourcei(alSource, AL_LOOPING, AL_TRUE);
    if (alGetError() != AL_NO_ERROR)
        printf("ERROR: setting source\n");
    // return 0;

    if (select == 0)
        alSourcePlay(alSource);

    if (select == 1) {
        //First delete the source.
        alDeleteSources(1, &alSource);
        //Delete the buffer.
        alDeleteBuffers(1, &alBuffer);
        //Close out OpenAL itself.
        //Get active context.
        ALCcontext *Context = alcGetCurrentContext();
        //Get device for active context.
        ALCdevice *Device = alcGetContextsDevice(Context);
        //Disable context.
        alcMakeContextCurrent(NULL);
        //Release context(s).
        alcDestroyContext(Context);
        //Close device.
        alcCloseDevice(Device);
        // return 0;
    }
#endif
}

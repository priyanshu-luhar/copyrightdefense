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
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <cstring>
#include "jhernandez2.h"

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

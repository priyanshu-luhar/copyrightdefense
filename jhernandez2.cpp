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

using namespace std;
/*
void check_keys()
{
    if (gl.keys[XK_a]) {
        g.ship.angle += 4.0;               // Was 4.0
        if (g.ship.angle >= 360.0f)
            g.ship.angle -= 360.0f; 
    }
    if (gl.keys[XK_d]) {
        g.ship.angle -= 4.0;               // Was 4.0
        if (g.ship.angle < 0.0f)
            g.ship.angle += 360.0f;
    }
    if (gl.keys[XK_Left])       // From here downward edits by me: Joseph 
        g.ship.angle = 90.0;
    if (gl.keys[XK_Right]) 
        g.ship.angle = 270.0;
    if (gl.keys[XK_Up])
        g.ship.angle = 360.0;
    if (gl.keys[XK_Down])
        g.ship.angle = 180.0;
    if (gl.keys[XK_Right] && gl.keys[XK_Up])
        g.ship.angle = 315.0;
    if (gl.keys[XK_Left] && gl.keys[XK_Up])
        g.ship.angle = 45.0;
    if (gl.keys[XK_Left] && gl.keys[XK_Down])
        g.ship.angle = 135.0;
    if (gl.keys[XK_Right] && gl.keys[XK_Down])
        g.ship.angle = 235.0;
    if (gl.keys[XK_space]) {          // XK_space was XK_Up, edits stop here 
        //apply thrust
        //convert ship angle to radians
        Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
        //convert angle to a vector
        Flt xdir = cos(rad);
        Flt ydir = sin(rad);
        g.ship.vel[0] += xdir*0.02f;
        g.ship.vel[1] += ydir*0.02f;
        Flt speed = sqrt(g.ship.vel[0]*g.ship.vel[0]+
                g.ship.vel[1]*g.ship.vel[1]);
        if (speed > 10.0f) {
            speed = 10.0f;
            normalize2d(g.ship.vel);
            g.ship.vel[0] *= speed;
            g.ship.vel[1] *= speed;
        }
    }
    if (gl.keys[XK_space]) {
        //a little time between each bullet
        struct timespec bt;
        clock_gettime(CLOCK_REALTIME, &bt);
        double ts = timeDiff(&g.bulletTimer, &bt);
        if (ts > 0.1) {
            timeCopy(&g.bulletTimer, &bt);
            if (g.nbullets < MAX_BULLETS) {
                //shoot a bullet...
                //Bullet *b = new Bullet;
                Bullet *b = &g.barr[g.nbullets];
                timeCopy(&b->time, &bt);
                b->pos[0] = g.ship.pos[0];
                b->pos[1] = g.ship.pos[1];
                b->vel[0] = g.ship.vel[0];
                b->vel[1] = g.ship.vel[1];
                //convert ship angle to radians
                Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
                //convert angle to a vector
                Flt xdir = cos(rad);
                Flt ydir = sin(rad);
                b->pos[0] += xdir*20.0f;
                b->pos[1] += ydir*20.0f;
                b->vel[0] += xdir*6.0f + rnd()*0.1;
                b->vel[1] += ydir*6.0f + rnd()*0.1;
                b->color[0] = 1.0f;
                b->color[1] = 1.0f;
                b->color[2] = 1.0f;
                g.nbullets++;
            }
        }
    }
    if (g.mouseThrustOn) {
        if (gl.mouse)
            return;
        //should thrust be turned off
        struct timespec mtt;
        clock_gettime(CLOCK_REALTIME, &mtt);
        double tdif = timeDiff(&mtt, &g.mouseThrustTimer);
        //std::cout << "tdif: " << tdif << std::endl;
        if (tdif < -0.3)
            g.mouseThrustOn = false;
    }
} */

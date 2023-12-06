//This is Carlos's individual source file.

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"
#include "csandoval.h"
#include "pluhar.h"
#include "jhernandez2.h"
#include "header.h"


extern  Global gl;
extern Game g;
extern  X11_wrapper x11;

//----------------------------------------------------------------------------------------------------//
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
//----------------------------------------------------------------------------------------------------//
int total_physics_function_calls(bool temp)
{
    temp = true;
    if (temp) {
        return physics_function_counter;
    }
    return 0;
}
//----------------------------------------------------------------------------------------------------//
void handleMenu() {
    int menuChoice = 0;
    bool inMenu = true;

    while (inMenu) {
        glClear(GL_COLOR_BUFFER_BIT);
        glPushMatrix();
        glColor3f(0.1, 0.1, 0.1);
        Rect r;
        r.bot = gl.yres / 1.2;
        r.left = gl.xres / 2;
        r.center = 1;

        ggprint16(&r, 60, 0x00ffffff, "Copyright Onslaught!");
        r.bot -= 60;

        ggprint8b(&r,10, 0x00ffff00, "Press P to Pause");
        r.bot -= 5;

        ggprint8b(&r,10, 0x00ffff00, "Press R to toggle WASD");
        r.bot -= 5;

        ggprint8b(&r,10, 0x00ffff00, "Press N to toggle Night-Mode");
        r.bot -= 5;

        ggprint8b(&r,10, 0x00ffff00, "Press ctrl + S to toggle Statistics");
        r.bot -= 5;

        ggprint8b(&r,10, 0x00ffff00, "Press S to Select");
        r.bot -= 40;

        if (menuChoice == 0)
            ggprint8b(&r, 24, 0x00ff0000, "> Start Game");
        else
            ggprint8b(&r, 24, 0x00ffffff, "Start Game");
        r.bot -= 20;

        if (menuChoice == 1)
            ggprint8b(&r, 24, 0x00ff0000, "> Options");  // Increase font size to 24
        else
            ggprint8b(&r, 24, 0x00ffffff, "Options");  // Increase font size to 24

        r.bot -= 20;  // Adjust the vertical spacing

        if (menuChoice == 1)
            ggprint8b(&r, 24, 0x00ff0000, "> Quit");  // Increase font size to 24
        else
            ggprint8b(&r, 24, 0x00ffffff, "Quit");  // Increase font size to 24

        glPopMatrix();
        x11.swapBuffers();
        // Handle menu input
        XEvent e;
        while (XCheckWindowEvent(x11.dpy, x11.win, KeyPressMask, &e)) {
            if (e.type == KeyPress) {
                int key = XLookupKeysym(&e.xkey, 0);
                switch (key) {
                    case XK_s:
                        if (menuChoice == 0) {
                            inMenu = false;  // Start the game
                        } else if (menuChoice == 2) {
                            // Quit the game
                            exit(0);
                        }
                        break;
                    case XK_Up:
                        menuChoice = (menuChoice + 2) % 3;
                        break;
                    case XK_Down:
                        menuChoice = (menuChoice + 1) % 3;
                        break;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------//
void displayPauseMenu() {
    // Draw the pause menu here
    // This would be an overlay with options like "Resume" or "Quit"
    Rect r;
    r.bot = gl.yres / 2.5;
    r.left = gl.xres / 2;
    r.center = 1;
    ggprint16(&r, 16, 0x00ffffff, "PAUSED");
    ggprint16(&r, 16, 0x00ffffff, "Press P to Resume");
}
//----------------------------------------------------------------------------------------------------//
void displayGameOver() {
    // Display "Game Over" and close the game
    glClear(GL_COLOR_BUFFER_BIT);
    Rect r;
    r.bot = gl.yres / 2;
    r.left = gl.xres / 2;
    r.center = 1;
    ggprint16(&r, 50, 0xFF0000, "GAME OVER!");
    x11.swapBuffers();
    // Add a 3-second delay using usleep
    usleep(2000000); // 3 seconds
    exit(0);
}
//----------------------------------------------------------------------------------------------------//
void displayYouDied() {
    // Display "You Died" and reset the game
    glClear(GL_COLOR_BUFFER_BIT);
    Rect r;
    r.bot = gl.yres / 2;
    r.left = gl.xres / 2;
    r.center = 1;
    ggprint16(&r, 50, 0xFF0000, "YOU DIED!");
    x11.swapBuffers();
    // Add a 3-second delay using usleep
    usleep(2000000); // 3 seconds
}
//----------------------------------------------------------------------------------------------------//
void displayYouWin() {
    // Display "You Win" and reset the game
    glClear(GL_COLOR_BUFFER_BIT);
    Rect r;
    r.bot = gl.yres / 2;
    r.left = gl.xres / 2;
    r.center = 1;
    ggprint16(&r, 50, 0x00FF00, "YOU WIN!");
    x11.swapBuffers();
    // Add a 3-second delay using usleep
    usleep(2000000); // 3 seconds
    exit(0);
}
//----------------------------------------------------------------------------------------------------//
void nightmodefilter(int xres, int yres)
{
    //applies the "night-time-esque" filter on screen

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(0.5f, .5f, 1.0f, 0.3f);
    glPushMatrix();

    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(0, 0);
    glVertex2f(0, yres);
    glVertex2f(xres, 0);
    glVertex2f(xres, yres);

    glEnd();
    glPopMatrix();
}
//----------------------------------------------------------------------------------------------------//
void display_border(int xres, int yres)
{
    // draw border around game.
    int b =5;
    glColor3f(1.0f, 0.60f, 0.0f);
    glPushMatrix();

    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(0,0);
    glVertex2f(0+b, 0+b);
    glVertex2f(0, 0+yres);
    glVertex2f(0+b, 0+yres-b);
    glVertex2f(xres, 0+yres);
    glVertex2f(xres-b, 0+yres-b);
    glVertex2f(xres, 0);
    glVertex2f(xres-b, b);
    glVertex2f(0, 0);
    glVertex2f(0+b, 0+b);

    glEnd();
    glPopMatrix();
}
//----------------------------------------------------------------------------------------------------//
void display_toggle(int x, int y)
{
    Rect r;
    r.bot = y * 4;
    r.left = x;
    r.center = 0;
    ggprint8b(&r, 16, 0xFFFFFFF, "WASD Toggled");
}	
//----------------------------------------------------------------------------------------------------//
void checkShipAsteroidCollision() {
    Asteroid *a = g.ahead;
    while (a) {
        // Calculate the distance between the ship and the asteroid
        float d0 = g.ship.pos[0] - a->pos[0];
        float d1 = g.ship.pos[1] - a->pos[1];
        float distance = sqrt(d0 * d0 + d1 * d1);

        if (distance < g.ship.radius + a->radius) {
            Lives--;
            // If Lives reach 0, display Game Over and end game loop, until then, reset the game and continue
            if (Lives == 0) {
                usleep(3000000);
                play_sound(1);
                displayGameOver();
            } else {
                usleep(2000000);
                displayYouDied();
                resetGame();
            }
        }
        a = a->next;
    }
}
//----------------------------------------------------------------------------------------------------//
void moveSmallAsteroidsTowardsShip() {
    Asteroid *a = g.ahead;
    while (a) {
        // Calculate the distance between the asteroid and the ship
        Flt d0 = g.ship.pos[0] - a->pos[0];
        Flt d1 = g.ship.pos[1] - a->pos[1];
        Flt dist = sqrt(d0 * d0 + d1 * d1);

        // Define a minimum distance for small asteroids to start following the ship
        const Flt MIN_FOLLOW_DISTANCE = 130.0;

        // If the asteroid is small and close enough to the ship, move towards the ship
        if (a->radius < MINIMUM_ASTEROID_SIZE && dist < MIN_FOLLOW_DISTANCE) {
            // Normalize the direction vector towards the ship
            Flt dirX = d0 / dist;
            Flt dirY = d1 / dist;

            // Define the speed at which small asteroids follow the ship
            const Flt FOLLOW_SPEED = 0.4;

            // Update the asteroid's velocity to follow the ship
            a->vel[0] = dirX * FOLLOW_SPEED;
            a->vel[1] = dirY * FOLLOW_SPEED;
        }
        a = a->next;
    }
}
extern double speedrate;
extern bool shotgun_mode;
//----------------------------------------------------------------------------------------------------//
void resetGame() {
    g.ship = Ship(); // Reset the ship
    g.nasteroids = 0;
    g.nbullets = 0;
    g.mouseThrustOn = false;
    g.ahead = NULL;
    g.bulletTimer.tv_sec = 0;
    g.bulletTimer.tv_nsec = 0;
    gameWon = false;
    countdown = 90; // Reset the countdown to 90
    gameStartTime = time(NULL);
    g.collectedCoins = 0;
    g.score = 0;
    speedrate = 1;
    shotgun_mode = false;
    
    // Initialize the asteroids again
    while (g.ahead != NULL) {
        Asteroid *temp = g.ahead;
        g.ahead = g.ahead->next;
        delete temp;
    }
    g.ahead = NULL;
    
    for (int j = 0; j < 10; j++) {
        Asteroid *a = new Asteroid;
        a->nverts = 6;
        a->radius = rnd() * 20.0 + 40.0;
        a->angle = 0.0f;
        a->color[0] = 0.8;
        a->color[1] = 0.8;
        a->color[2] = 0.7;

        a->vel[0] = (Flt)(rnd() * 2.0 - 1.0);
        a->vel[1] = (Flt)(rnd() * 2.0 - 1.0);

        bool validPosition = false;
        while (!validPosition) {
            // Randomly select a position for the asteroid
            a->pos[0] = rnd() * (float)gl.xres;
            a->pos[1] = rnd() * (float)gl.yres;

            // Check if the asteroid is too close to the ship
            Flt d0 = a->pos[0] - g.ship.pos[0];
            Flt d1 = a->pos[1] - g.ship.pos[1];
            Flt distance = sqrt(d0 * d0 + d1 * d1);

            if (distance >= MINIMUM_ASTEROID_DISTANCE) {
                // Asteroid is far enough from the ship, consider it a valid position
                validPosition = true;
            }
        }

        // Add to front of linked list
        a->next = g.ahead;
        if (g.ahead != NULL) {
            g.ahead->prev = a;
        }
        g.ahead = a;
        ++g.nasteroids;
    }
}

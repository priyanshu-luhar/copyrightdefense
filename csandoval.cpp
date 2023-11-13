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


extern  Global gl;
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

        if (menuChoice == 2)
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
//Background will be a screenshoted grass field with some details
//Ship will be a Megaman sprite 
//Galaga Style ship will be a 2x barrel buff

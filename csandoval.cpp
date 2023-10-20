//this is carlos's individual source file.

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

extern Global gl;
extern X11_wrapper x11;


void handleMenu() {
    int menuChoice = 0;
    bool inMenu = true;

    while (inMenu) {
        glClear(GL_COLOR_BUFFER_BIT);
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        Rect r;
        r.bot = gl.yres / 1.5;
        r.left = gl.xres / 2;
        r.center = 1;

        ggprint16(&r, 60, 0x00ffffff, "Copyright Onslaught!");  // Increase font size to 60
        r.bot -= 60;  // Adjust the vertical spacing

        if (menuChoice == 0)
            ggprint8b(&r, 24, 0x00ff0000, "> Start Game");  // Increase font size to 24
        else
            ggprint8b(&r, 24, 0x00ffffff, "Start Game");  // Increase font size to 24

        r.bot -= 20;  // Adjust the vertical spacing

        if (menuChoice == 1)
            ggprint8b(&r, 24, 0x00ff0000, "> Credits");  // Increase font size to 24
        else
            ggprint8b(&r, 24, 0x00ffffff, "Credits");  // Increase font size to 24

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
                if (e.xkey.keycode == 39) {
                    // 'S' key to select menu option
                    if (menuChoice == 0) {
                        inMenu = false;  // Start the game
                    } else if (menuChoice == 1) {
                        // Example: inCredits = true;
                    } else if (menuChoice == 2) {
                        // Quit the game
                        exit(0);
                    }
                } else if (e.xkey.keycode == 111) {
                    // Up arrow key
                    menuChoice = (menuChoice + 2) % 3;
                } else if (e.xkey.keycode == 116) {
                    // Down arrow key
                    menuChoice = (menuChoice + 1) % 3;
                }
            }
        }
    }
}

void displayGameOver() {
    // Display "Game Over" and reset the game
    glClear(GL_COLOR_BUFFER_BIT);
    Rect r;
    r.bot = gl.yres / 2;
    r.left = gl.xres / 2;
    r.center = 1;
    ggprint16(&r, 50, 0xFF0000, "GAME OVER");
    x11.swapBuffers();
    // Add a 3-second delay using usleep
    usleep(3000000); // 3 seconds
	exit(0);
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
    usleep(3000000); // 3 seconds
	exit(0);
}

void nightmodefilter(int xres, int yres)
{
	//applies the "night-time-esque" filter on screen

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glColor4f(0.6f, .6f, 1.0f, 0.3f);
	glPushMatrix();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(0, 0);
    glVertex2f(0, yres);
    glVertex2f(xres, 0);
    glVertex2f(xres, yres);
	
	glEnd();
	glPopMatrix();
}

void display_border(int xres, int yres)
	{
	// draw border around game.
	int b =5;
	glColor3f(1.0f, 1.0f, 0.0f);
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

void display_name(int x, int y)
{
	// Show's the name listed in  ggprint8b on the side of the screen
	Rect r;
	r.bot = y;
	r.left = x;
	r.center = 0;
    ggprint8b(&r, 0, 0xFFFFFFF, "Player");
}	
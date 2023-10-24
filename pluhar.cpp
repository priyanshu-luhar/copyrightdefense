//This is Priyanshu's individual source file.

// Start and Pause Menu
// Potential Original Character
// Game over screen
// Potential Skin Select for maps and character
// Character Buffs
// WASD Control

//Map Ideas: 


// Player Colosion
// Asteroid to Player Agression
// 





#include "fonts.h"
#include <GL/glx.h>
#include "pluhar.h"
// #include "headers.h"

bool wasdtoggle()
{
    return !wasdvar;
}

int wasd(int key)
{
    if(wasdvar) {
        switch(key)
        {
            case XK_w:
                key = XK_Up;
                break;
            case XK_a:
                key = XK_Left;
                break;
            case XK_s:
                key = XK_Down;
                break;
            case XK_d:
                key = XK_Right;
                break;
        }
    }
    return key;
}

// void display_border(int xres, int yres)
// {
//     // draw border around game.
//     //Draw the ship
//     int b =50;
//     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//     glEnable(GL_BLEND);
//     glColor4f(1.0f, 1.0f, 0.0f, 0.1f);
//     glPushMatrix();
//     glBegin(GL_TRIANGLE_STRIP);
//     glVertex2f(0,0);
//     glVertex2f(0+b, 0+b);
//     glVertex2f(0, 0+yres);
//     glVertex2f(0+b, 0+yres-b);

//     glVertex2f(xres, 0+yres);
//     glVertex2f(xres-b, 0+yres-b);
//     glVertex2f(xres, 0);
//     glVertex2f(xres-b, b);
//     glVertex2f(0, 0);
//     glVertex2f(0+b, 0+b);


//     glEnd();
//     glPopMatrix();
// }

// void display_name(int x, int y)
// {
//     // draw border around game.
//     Rect r;
//     r.bot = y;
//     r.left = x;
//     r.center = 0;
//     ggprint8b(&r, 0, 0x00000000, "Carlos");
// }

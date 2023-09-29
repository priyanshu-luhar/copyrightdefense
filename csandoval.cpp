//this is carlos's individual source file.

// Start and Pause Menu
// Potential Original Character
// Game over screen
// Potential Skin Select for maps and character
// Character Buffs

//Map Ideas: 


// Player Colosion
// Asteroid to Player Agression

#include "fonts.h"
#include <GL/glx.h>


	


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
    ggprint8b(&r, 0, 0xFFFFFFF, "Carlos");
}
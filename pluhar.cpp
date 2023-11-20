//This is Priyanshu's individual source file.

// WASD Control
// Mouse Movement Distance
// Images

#include "fonts.h"
#include <GL/glx.h>
#include <cmath>
#include "pluhar.h"
#include "header.h"

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

/*
   int total_running_time(bool temp)
   {
   static int first_time = 1;
   static int start_time;

   if (first_time) {
   start_time = time(NULL);
   first_time = 0;
   }

   if (temp) {
   return time(NULL) - start_time;
   }

   return 0;
   }
   */

// int total_render_function_calls(bool temp)
// {
//     if (temp) {
//         return render_calls;
//     }

//     return 0;
// }

// int total_physics_function_calls(bool temp)
// {
//     if (temp) {
//         return physics_calls;
//     }

//     return 0;
// }

double mouse_movement_distance(int x, int y, bool temp)
{
    int xdist = abs(psavex - x);
    int ydist = abs(psavey - y);

    if (temp) {
        double total_distance;
        total_distance = (xdist*xdist) + (ydist*ydist);
        total_distance = sqrt(total_distance);
        return total_distance;
    } else {
        psavex = x;
        psavey = y;
    }

    return 0;
}

/*
XImage *XCreateImage(display, visual, depth, XYPixmap, 0, "images/gamebackground.png", 
                    400, 300, bitmap_pad, bytes_per_line);
*/

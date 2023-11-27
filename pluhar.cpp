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
class Image {
public:
	int width, height;
	unsigned char *data;
	~Image() { delete [] data; }
	Image(const char *fname) {
		if (fname[0] == '\0')
			return;
		//printf("fname **%s**\n", fname);
		int ppmFlag = 0;
		char name[40];
		strcpy(name, fname);
		int slen = strlen(name);
		char ppmname[80];
		if (strncmp(name+(slen-4), ".ppm", 4) == 0)
			ppmFlag = 1;
		if (ppmFlag) {
			strcpy(ppmname, name);
		} else {
			name[slen-4] = '\0';
			//printf("name **%s**\n", name);
			sprintf(ppmname,"%s.ppm", name);
			//printf("ppmname **%s**\n", ppmname);
			char ts[100];
			//system("convert eball.jpg eball.ppm");
			sprintf(ts, "convert %s %s", fname, ppmname);
			system(ts);
		}
		//sprintf(ts, "%s", name);
		FILE *fpi = fopen(ppmname, "r");
		if (fpi) {
			char line[200];
			fgets(line, 200, fpi);
			fgets(line, 200, fpi);
			//skip comments and blank lines
			while (line[0] == '#' || strlen(line) < 2)
				fgets(line, 200, fpi);
			sscanf(line, "%i %i", &width, &height);
			fgets(line, 200, fpi);
			//get pixel data
			int n = width * height * 3;			
			data = new unsigned char[n];			
			for (int i=0; i<n; i++)
				data[i] = fgetc(fpi);
			fclose(fpi);
		} else {
			printf("ERROR opening image: %s\n",ppmname);
			exit(0);
		}
		if (!ppmFlag)
			unlink(ppmname);
	}
};

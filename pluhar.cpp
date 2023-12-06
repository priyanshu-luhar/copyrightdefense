//This is Priyanshu's individual source file.

// WASD Control
// Mouse Movement Distance
// Images
// Buffbomb

#include "pluhar.h"
#include "header.h"

bool wasdtoggle()
{
        return !wasdvar;
}

int wasd(int key)
{
        if(wasdvar) {
                switch(key) {
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
void showbuffbomb()
{
        if (buffbomb) {
                ggprint16(&r, 16, 0x00ffff00, "Bomb is active, Press b to Blast!");
        }
}

void checkbuffbomb(int count)
{
        if (count != 90 && count%15 == 0)
                buffbomb = true;
}

void buffbomb()
{
                if (buffbomb) {
                        if (gl.keys[XK_b]) {
                                // Will delete almost all the asteroids
                                        while (g.nasteroids > 1) {
                                                Asteroid *savea = a->next;
                                                deleteAsteroid(&g, a);
                                                a = savea;
                                                g.nasteroids--;
                                        }
                                        buffbomb = false;
                        }
                }
}
*/
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

class MainCharacter {
        public:
} mc;
/*
   class Image {
   private:

   public:
   int width, height, imgid;
   unsigned char *data;

// Destructor
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

Image pic[2] = {"./images/menubackground.png",
"./images/luigi.png"};

unsigned char *buildAlphaData(Image *img)
{
int i;
int a,b,c;
unsigned char *newdata, *ptr;
unsigned char *data = (unsigned char *)img->data;
newdata = (unsigned char *)malloc(img->width * img->height * 4);
ptr = newdata;
for (i=0; i<img->width * img->height * 3; i+=3) {
a = *(data+0);
b = *(data+1);
c = *(data+2);
*(ptr+0) = a;
*(ptr+1) = b;
*(ptr+2) = c;
*(ptr+3) = (a|b|c);
ptr += 4;
data += 3;
}
return newdata;
}
*/

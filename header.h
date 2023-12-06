#ifndef _HEADER_H_
#define _HEADER_H_

#include <cstring>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"


typedef float Flt;
typedef float Vec[3];
extern int Lives;
extern bool gameWon;
extern int countdown;
extern time_t gameStartTime;
extern const Flt MINIMUM_ASTEROID_SIZE;
extern const float MINIMUM_ASTEROID_DISTANCE;
inline float rnd() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

class Ship {
    public:
        Vec pos;
        Vec dir;
        Vec vel;
        Vec acc;
        float angle;
        float color[3];
        float radius;
        Ship();
};

class Bullet {
    public:
        Vec pos;
        Vec vel;
        float color[6];
        struct timespec time;
        Bullet();
};

class Asteroid {
    public:
        Vec pos;
        Vec vel;
        int nverts;
        Flt radius;
        Vec vert[8];
        float angle;
        float color[3];
        Asteroid *prev;
        Asteroid *next;
        Asteroid();
};

class Coin {
    public:
        Vec pos;
        float radius;
        bool active;
        Coin();
};

class Game {
    public:
        Ship ship;
        Asteroid *ahead;
        Bullet *barr;
        int nasteroids;
        int nbullets;
        struct timespec bulletTimer;
        struct timespec mouseThrustTimer;
        bool mouseThrustOn;
        Coin coins[5]; 
        int score;
        int collectedCoins;
        Game();
        void spawnCoin();
        ~Game();
};


class Global {
    public:
        int xres, yres;
        int show_toggle;
        int nightmodefilter;
        int renderMenu;
        char keys[65536];
        int mouse;
        bool statistics;
        int allowimages;
        GLuint menubackgroundTexture;
        GLuint mcTexture;
        GLuint grassTexture;
        GLuint enemyTexture;
        GLuint purpleTexture;
        Global();
};

extern Global gl;

class X11_wrapper {
    private:
        GLXContext glc;
    public:
        Window win;
        Display *dpy;

        X11_wrapper();
        X11_wrapper(int w, int h);
        ~X11_wrapper();

        void set_title();
        void check_resize(XEvent *e);
        void reshape_window(int width, int height);
        void setup_screen_res(const int w, const int h);
        void swapBuffers();
        bool getXPending();
        XEvent getXNextEvent();
        void set_mouse_position(int x, int y);
        void show_mouse_cursor(const int onoff);
};

extern X11_wrapper x11;

#endif

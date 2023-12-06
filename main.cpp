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
#include "jhernandez2.h"
#include "pluhar.h"
#include "header.h"

using namespace std;

//defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt	Matrix[4][4];

bool buffbomb = false;
//macros
// #define rnd() (((Flt)rand())/(Flt)RAND_MAX) // Moved to headers.h
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
                             (c)[1]=(a)[1]-(b)[1]; \
(c)[2]=(a)[2]-(b)[2]
//constants
const float timeslice = 1.0f;
const float gravity = 2.2f;
#define PI 3.141592653589793
#define ALPHA 1
#define DEG2RAD (PI / 180.0)

int MAX_BULLETS = 5; //Not a const int so can be altered
bool shotgun_mode = false;
const Flt MINIMUM_ASTEROID_SIZE = 40.0;
const float MINIMUM_ASTEROID_DISTANCE = 400.0; 
// Adjust this value as needed 
//-----------------------------------------------------------------------------
//Setup Timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
extern struct timespec timeStart, timeCurrent;
extern struct timespec timePause;
extern double physicsCountdown;
extern double timeSpan;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
//-----------------------------------------------------------------------------
//Function Prototypes
void init_opengl(void);
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void physics();
void render();
void checkShipAsteroidCollision();
void displayYouDied();
void displayYouWin();
void displayGameOver();
void moveSmallAsteroidsTowardsShip();
void resetGame();
void displayPauseMenu();
//-----------------------------------------------------------------------------
//Global Variables
Global gl;
Game g;
X11_wrapper x11(gl.xres, gl.yres);
bool gameWon = false;
bool wasdvar = false;
int physics_function_counter = 0;
time_t gameStartTime;
int Lives = 3; 
bool shipFirstMove = false;
struct timespec firstMoveTime;
bool displayMessage = true;
int countdown = 90;
bool isPaused = false;

//-----------------------------------------------------------------------------

Ship::Ship() {
    pos[0] = (Flt)(gl.xres/2);
    pos[1] = (Flt)(gl.yres/2);
    pos[2] = 0.0f;
    VecZero(dir);
    VecZero(vel);
    VecZero(acc);
    angle = 0.0;
    color[0] = color[1] = color[2] = 1.0;
    radius = 6.0f;
}

Bullet::Bullet() {
}

Asteroid::Asteroid() {
    prev = NULL;
    next = NULL;

}

Coin::Coin() {
    // Initialize Coin members
    VecZero(pos);
    radius = 5.0f;  // You can set this to whatever is appropriate for your game
    active = false;
}

// Priyanshu's code for Images
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
                sprintf(ppmname,"%s.ppm", name);
                char ts[100];
                sprintf(ts, "convert %s %s", fname, ppmname);
                system(ts);
            }
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

Image pic[5] = {"./images/menubackground.png",
    "./images/luigi.png",
    "./images/grass.png",
    "./images/enemy.png",
    "./images/hollowpurple.png"};

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

//--


Game::Game() {
    ahead = NULL;
    barr = new Bullet[MAX_BULLETS];
    nasteroids = 0;
    nbullets = 0;
    mouseThrustOn = false;
    collectedCoins = 0; // Initialize collected coins count

    score = 0;
    for (int i = 0; i < 5; i++) {
        spawnCoin(); // Initially spawn 5 coins
    }

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
            // Randomly select an edge to spawn the asteroid on
            int side = random(4);
            if (side == 0) {
                // Spawn on the left edge
                a->pos[0] = 0.0f;
                a->pos[1] = rnd() * (float)gl.yres;
            } else if (side == 1) {
                // Spawn on the top edge
                a->pos[0] = rnd() * (float)gl.xres;
                a->pos[1] = (float)gl.yres;
            } else if (side == 2) {
                // Spawn on the right edge
                a->pos[0] = (float)gl.xres;
                a->pos[1] = rnd() * (float)gl.yres;
            } else {
                // Spawn on the bottom edge
                a->pos[0] = rnd() * (float)gl.xres;
                a->pos[1] = 0.0f;
            }
            // Check if the asteroid is too close to the ship
            Flt d0 = a->pos[0] - ship.pos[0];
            Flt d1 = a->pos[1] - ship.pos[1];
            Flt distance = sqrt(d0 * d0 + d1 * d1);

            if (distance >= MINIMUM_ASTEROID_DISTANCE) {
                // Asteroid is far enough from the ship, consider it a valid position
                validPosition = true;
            }
        }
        // Add to front of linked list
        a->next = ahead;
        if (ahead != NULL) {
            ahead->prev = a;
        }
        ahead = a;
        ++nasteroids;
    }
    clock_gettime(CLOCK_REALTIME, &bulletTimer);
}

void Game::spawnCoin() {
    for (int i = 0; i < 5; i++) {
        if (!g.coins[i].active) {
            // Set coin's position and make it active
            g.coins[i].pos[0] = rnd() * gl.xres;
            g.coins[i].pos[1] = rnd() * gl.yres;
            g.coins[i].active = true;
            break;
        }
    }
}


Game::~Game() {
    delete [] barr;
}

Global::Global() {
    xres = 700;
    yres = 540;
    memset(keys, 0, 65536);
    show_toggle = 0;
    nightmodefilter = 0;
    mouse = 0;
    renderMenu = 0;
    statistics = false;
    allowimages = 1;
}

X11_wrapper::X11_wrapper() {
}

X11_wrapper::X11_wrapper(int w, int h) {
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    XSetWindowAttributes swa;
    setup_screen_res(gl.xres, gl.yres);
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        std::cout << "\n\tcannot connect to X server" << std::endl;
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XWindowAttributes getWinAttr;
    XGetWindowAttributes(dpy, root, &getWinAttr);
    int fullscreen = 0;
    gl.xres = w;
    gl.yres = h;
    if (!w && !h) {
        // Go to fullscreen.
        gl.xres = getWinAttr.width;
        gl.yres = getWinAttr.height;
        XGrabKeyboard(dpy, root, False,
                GrabModeAsync, GrabModeAsync, CurrentTime);
        fullscreen = 1;
    }
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
        std::cout << "\n\tno appropriate visual found\n" << std::endl;
        exit(EXIT_FAILURE);
    } 
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
        PointerMotionMask | MotionNotify | ButtonPress | ButtonRelease |
        StructureNotifyMask | SubstructureNotifyMask;
    unsigned int winops = CWBorderPixel|CWColormap|CWEventMask;
    if (fullscreen) {
        winops |= CWOverrideRedirect;
        swa.override_redirect = True;
    }
    win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
            vi->depth, InputOutput, vi->visual, winops, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
    show_mouse_cursor(0);
}

X11_wrapper::~X11_wrapper() {
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

void X11_wrapper::set_title() {
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "Copyright Onslaught!");
}

void X11_wrapper::check_resize(XEvent *e) {
    if (e->type != ConfigureNotify)
        return;
    XConfigureEvent xce = e->xconfigure;
    if (xce.width != gl.xres || xce.height != gl.yres) {
        reshape_window(xce.width, xce.height);
    }
}

void X11_wrapper::reshape_window(int width, int height) {
    setup_screen_res(width, height);
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
    set_title();
}

void X11_wrapper::setup_screen_res(const int w, const int h) {
    gl.xres = w;
    gl.yres = h;
}

void X11_wrapper::swapBuffers() {
    glXSwapBuffers(dpy, win);
}

bool X11_wrapper::getXPending() {
    return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent() {
    XEvent e;
    XNextEvent(dpy, &e);
    return e;
}

void X11_wrapper::set_mouse_position(int x, int y) {
    XWarpPointer(dpy, None, win, 0, 0, 0, 0, x, y);
}

void X11_wrapper::show_mouse_cursor(const int onoff) {
    if (onoff) {
        XUndefineCursor(dpy, win);
        return;
    }
    Pixmap blank;
    XColor dummy;
    char data[1] = {0};
    Cursor cursor;
    blank = XCreateBitmapFromData(dpy, win, data, 1, 1);
    if (blank == None)
        std::cout << "error: out of memory." << std::endl;
    cursor = XCreatePixmapCursor(dpy, blank, blank, &dummy, &dummy, 0, 0);
    XFreePixmap(dpy, blank);
    XDefineCursor(dpy, win, cursor);
}
//X Windows variables
// ---> for fullscreen x11(0, 0);

int psavex = 0;
int psavey = 0;
//==========================================================================
// M A I N
//==========================================================================
int main(){
    play_sound(0);
    logOpen();
    init_opengl();
    srand(time(NULL));
    clock_gettime(CLOCK_REALTIME, &timePause);
    clock_gettime(CLOCK_REALTIME, &timeStart);
    x11.set_mouse_position(100,100);

    bool inMenu = true;
    int done = 0;
    gameStartTime = time(NULL);
    while (!done) {
        while (x11.getXPending()) {
            XEvent e = x11.getXNextEvent();
            x11.check_resize(&e);
            check_mouse(&e);
            done = check_keys(&e);
        }

        if (!inMenu) {
            if (!isPaused) {
                // Game is not paused, run physics and rendering
                clock_gettime(CLOCK_REALTIME, &timeCurrent);
                timeSpan = timeDiff(&timeStart, &timeCurrent);
                timeCopy(&timeStart, &timeCurrent);
                physicsCountdown += timeSpan;
                while (physicsCountdown >= physicsRate) {
                    physics();
                    physicsCountdown -= physicsRate;
                }
                render();
            } else {
                // Game is paused, display the pause menu
                displayPauseMenu();
            }
            moveSmallAsteroidsTowardsShip();
            checkShipAsteroidCollision();
            x11.swapBuffers();
        } else {
            handleMenu();
            inMenu = !inMenu;
        }
    }
    cleanup_fonts();
    logClose();
    return 0;
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, gl.xres, gl.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
    //
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    //
    //Clear the screen to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
    // This is for the player's image
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    unsigned char *mcData = buildAlphaData(&pic[1]);
    glGenTextures(1, &gl.mcTexture);

    glBindTexture(GL_TEXTURE_2D, gl.mcTexture);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, pic[1].width, pic[1].height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, pic[1].data);

    free(mcData);
    //-------------------------------------------------------------------------

    unsigned char *grassData = buildAlphaData(&pic[2]);
    glGenTextures(1, &gl.grassTexture);

    glBindTexture(GL_TEXTURE_2D, gl.grassTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, pic[2].width, pic[2].height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, pic[2].data);

    free(grassData);
    //-------------------------------------------------------------------------
    unsigned char *enemyData = buildAlphaData(&pic[3]);
    glGenTextures(1, &gl.enemyTexture);

    glBindTexture(GL_TEXTURE_2D, gl.enemyTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pic[3].width, pic[3].height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, pic[3].data);

    free(enemyData);
    //-------------------------------------------------------------------------
    unsigned char *purpleData = buildAlphaData(&pic[4]);
    glGenTextures(1, &gl.purpleTexture);

    glBindTexture(GL_TEXTURE_2D, gl.purpleTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, pic[4].width, pic[4].height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, pic[4].data);

    free(purpleData);
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

}

void normalize2d(Vec v)
{
    Flt len = v[0]*v[0] + v[1]*v[1];
    if (len == 0.0f) {
        v[0] = 1.0;
        v[1] = 0.0;
        return;
    }
    len = 1.0f / sqrt(len);
    v[0] *= len;
    v[1] *= len;
}

void check_mouse(XEvent *e)
{
    //Did the mouse move?
    //Was a mouse button clicked?
    static int savex = 0;
    static int savey = 0;
    //
    //static int ct=0;
    //std::cout << "m" << std::endl << std::flush;
    if (e->type == ButtonRelease) {
        return;
    }
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) {
            //Left button is down
            //a little time between each bullet
            struct timespec bt;
            clock_gettime(CLOCK_REALTIME, &bt);
            double ts = timeDiff(&g.bulletTimer, &bt);
            if (ts > 0.1) {
                timeCopy(&g.bulletTimer, &bt);
                //shoot a bullet...
                if (g.nbullets < MAX_BULLETS) {
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
                    ++g.nbullets;
                }
            }
        }
        if (e->xbutton.button==3) {
            //Right button is down
        }
    }
    //keys[XK_Up] = 0;
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        //Mouse moved
        mouse_movement_distance(e->xbutton.x, e->xbutton.y, false);
        time_since_mouse_move(false);
        /*
           int xdiff = savex - e->xbutton.x;
           int ydiff = savey - e->xbutton.y;
           if (++ct < 10)
           return;		
        //std::cout << "savex: " << savex << std::endl << std::flush;
        //std::cout << "e->xbutton.x: " << e->xbutton.x << std::endl <<
        //std::flush;
        if (xdiff > 0) {
        //std::cout << "xdiff: " << xdiff << std::endl << std::flush;
        g.ship.angle += 0.05f * (float)xdiff;
        if (g.ship.angle >= 360.0f)
        g.ship.angle -= 360.0f;
        }
        else if (xdiff < 0) {
        //std::cout << "xdiff: " << xdiff << std::endl << std::flush;
        g.ship.angle += 0.05f * (float)xdiff;
        if (g.ship.angle < 0.0f)
        g.ship.angle += 360.0f;
        }
        if (ydiff > 0) {
        //apply thrust
        //convert ship angle to radians
        Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
        //convert angle to a vector
        Flt xdir = cos(rad);
        Flt ydir = sin(rad);
        g.ship.vel[0] += xdir * (float)ydiff * 0.01f;
        g.ship.vel[1] += ydir * (float)ydiff * 0.01f;
        Flt speed = sqrt(g.ship.vel[0]*g.ship.vel[0]+
        g.ship.vel[1]*g.ship.vel[1]);
        if (speed > 10.0f) {
        speed = 10.0f;
        normalize2d(g.ship.vel);
        g.ship.vel[0] *= speed;
        g.ship.vel[1] *= speed;
        }
        g.mouseThrustOn = true;
        clock_gettime(CLOCK_REALTIME, &g.mouseThrustTimer);
        }
        x11.set_mouse_position(100,100);
        savex = 100;
        savey = 100;
        */
    }
}

int check_keys(XEvent *e)
{
    static int shift = 0;
    static bool controlPressed = false;
    static bool sPressed = false;

    int key = (XLookupKeysym(&e->xkey, 0));
    key = wasd(key);

    //Written by Carlos----------------------------------------------------------------------------------------------------//
    if (e->type == KeyRelease) {
        gl.keys[key] = 0;
        if (key == XK_Shift_L || key == XK_Shift_R) {
            shift = 0;
        }
        if (key == XK_Control_L) {
            controlPressed = false;
        }
        if (key == XK_s) {
            sPressed = false;
        }
        return 0;
    }
    if (e->type == KeyPress) {
        gl.keys[key] = 1;
        if (key == XK_Shift_L || key == XK_Shift_R) {
            shift = 1;
        }
        if (key == XK_s) {
            sPressed = true;
        }
        if (key == XK_Control_L) {
            controlPressed = true;
        }
    }

    if (e->type == KeyPress) {
        if (XLookupKeysym(&e->xkey, 0) == XK_p || XLookupKeysym(&e->xkey, 0) == XK_Return) {
            isPaused = !isPaused;  // Toggle the pause state
            if (isPaused) {
                clock_gettime(CLOCK_REALTIME, &timePause);  // Record the pause time
            } else {
                // Adjust the start time for the physics calculations
                struct timespec timeResume;
                clock_gettime(CLOCK_REALTIME, &timeResume);
                timeStart.tv_sec += timeResume.tv_sec - timePause.tv_sec;
                timeStart.tv_nsec += timeResume.tv_nsec - timePause.tv_nsec;
            }
        }
    }

    //Written by Carlos----------------------------------------------------------------------------------------------------//

    // Check if both Control and s keys are pressed
    if ((sPressed && controlPressed) || (controlPressed && sPressed)) {
        gl.statistics = !gl.statistics;
        controlPressed = false;
        sPressed = false;
    }

    (void)shift;
    switch (key) {
        case XK_Escape:
            return 1;

        case XK_n:
            gl.nightmodefilter = !gl.nightmodefilter;
            break;

        case XK_m:
            gl.mouse = !gl.mouse;
            if (gl.mouse)
                x11.show_mouse_cursor(1);
            else
                x11.show_mouse_cursor(0);
            break;

        case XK_r:
            wasdvar = wasdtoggle();
            gl.show_toggle = !gl.show_toggle;
            break;

        case XK_Down:
            break;
        case XK_equal:
            break;
        case XK_minus:
            break;
    }
    return 0;
}

void deleteAsteroid(Game *g, Asteroid *node)
{
    //Remove a node from doubly-linked list
    //Must look at 4 special cases below.
    if (node->prev == NULL) {
        if (node->next == NULL) {
            //only 1 item in list.
            g->ahead = NULL;
        } else {
            //at beginning of list.
            node->next->prev = NULL;
            g->ahead = node->next;
        }
    } else {
        if (node->next == NULL) {
            //at end of list.
            node->prev->next = NULL;
        } else {
            //in middle of list.
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
    }
    delete node;
    node = NULL;
}

void buildAsteroidFragment(Asteroid *ta, Asteroid *a)
{
    //build ta from a
    ta->nverts = 8;
    ta->radius = a->radius / 2.0;
    Flt r2 = ta->radius / 2.0;
    Flt angle = 0.0f;
    Flt inc = (PI * 2.0) / (Flt)ta->nverts;
    for (int i=0; i<ta->nverts; i++) {
        ta->vert[i][0] = sin(angle) * (r2 + rnd() * ta->radius);
        ta->vert[i][1] = cos(angle) * (r2 + rnd() * ta->radius);
        angle += inc;
    }
    ta->pos[0] = a->pos[0] + rnd()*2.0-5.0;
    ta->pos[1] = a->pos[1] + rnd()*2.0-5.0;
    ta->pos[2] = 0.0f;
    ta->angle = 0.0;
    //ta->rotate = a->rotate + (rnd() * 4.0 - 2.0);
    ta->vel[0] = a->vel[0] + (rnd()*1.0-0.5);
    ta->vel[1] = a->vel[1] + (rnd()*1.0-0.5);
    //std::cout << "frag" << std::endl;
}

double speedrate = 1;

void physics()
{
    physics_function_counter++;
    Flt d0,d1,dist;
    //Update ship position
    g.ship.pos[0] += g.ship.vel[0];
    g.ship.pos[1] += g.ship.vel[1];
    //Check for collision with window edges
    if (g.ship.pos[0] < 0.0) {
        g.ship.pos[0] += (float)gl.xres;
    }
    else if (g.ship.pos[0] > (float)gl.xres) {
        g.ship.pos[0] -= (float)gl.xres;
    }
    else if (g.ship.pos[1] < 0.0) {
        g.ship.pos[1] += (float)gl.yres;
    }
    else if (g.ship.pos[1] > (float)gl.yres) {
        g.ship.pos[1] -= (float)gl.yres;
    }

    if (!shipFirstMove && (g.ship.vel[0] != 0.0f || g.ship.vel[1] != 0.0f)) {
        shipFirstMove = true;
        displayMessage = true;
        clock_gettime(CLOCK_REALTIME, &firstMoveTime);
    }

    static int lastTime = -1;
    int currentTime = total_running_time(true); 
    if (currentTime != lastTime) {
        if (countdown > 0) {
            countdown--;
        } else {
            // If countdown hits 0, trigger the game won condition
            if (!gameWon) {
                gameWon = true;
                displayYouWin(); // This will display You Win and end the game.
            }
        }
        lastTime = currentTime;
    }

    //
    //
    //Update bullet positions
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    int i = 0;
    while (i < g.nbullets) {
        Bullet *b = &g.barr[i];
        //How long has bullet been alive?
        double ts = timeDiff(&b->time, &bt);
        if (ts > 2.5) {
            //time to delete the bullet.
            memcpy(&g.barr[i], &g.barr[g.nbullets-1],
                    sizeof(Bullet));
            g.nbullets--;
            //do not increment i.
            continue;
        }
        //move the bullet
        b->pos[0] += b->vel[0];
        b->pos[1] += b->vel[1];
        //Check for collision with window edges
        if (b->pos[0] < 0.0) {
            b->pos[0] += (float)gl.xres;
        }
        else if (b->pos[0] > (float)gl.xres) {
            b->pos[0] -= (float)gl.xres;
        }
        else if (b->pos[1] < 0.0) {
            b->pos[1] += (float)gl.yres;
        }
        else if (b->pos[1] > (float)gl.yres) {
            b->pos[1] -= (float)gl.yres;
        }
        ++i;
    }
    //
    //Update asteroid positions
    Asteroid *a = g.ahead;
    while (a) {
        a->pos[0] += a->vel[0];
        a->pos[1] += a->vel[1];
        //Check for collision with window edges
        if (a->pos[0] < -100.0) {
            a->pos[0] += (float)gl.xres+200;
        }
        else if (a->pos[0] > (float)gl.xres+100) {
            a->pos[0] -= (float)gl.xres+200;
        }
        else if (a->pos[1] < -100.0) {
            a->pos[1] += (float)gl.yres+200;
        }
        else if (a->pos[1] > (float)gl.yres+100) {
            a->pos[1] -= (float)gl.yres+200;
        }
        //a->angle += a->rotate;
        a = a->next;
    }
    //
    //Asteroid collision with bullets?
    //If collision detected:
    //     1. delete the bullet
    //     2. break the asteroid into pieces
    //        if asteroid small, delete it
    a = g.ahead;
    while (a) 
    {
        //is there a bullet within its radius?
        int i=0;
        while (i < g.nbullets) {
            Bullet *b = &g.barr[i];
            d0 = b->pos[0] - a->pos[0];
            d1 = b->pos[1] - a->pos[1];
            dist = (d0*d0 + d1*d1);
            if (dist < (a->radius*a->radius)) {
                //std::cout << "asteroid hit." << std::endl;
                //this asteroid is hit.
                if (a->radius > MINIMUM_ASTEROID_SIZE) {
                    //break it into pieces.
                    Asteroid *ta = a;
                    buildAsteroidFragment(ta, a);
                    int r = rand()%10+5;
                    for (int k=0; k<r; k++) {
                        //get the next asteroid position in the array
                        Asteroid *ta = new Asteroid;
                        buildAsteroidFragment(ta, a);
                        //add to front of asteroid linked list
                        ta->next = g.ahead;
                        if (g.ahead != NULL)
                            g.ahead->prev = ta;
                        g.ahead = ta;
                        g.nasteroids++;
                    }
                } else {
                    a->color[0] = 1.0;
                    a->color[1] = 0.1;
                    a->color[2] = 0.1;
                    //asteroid is too small to break up
                    //delete the asteroid and bullet
                    Asteroid *savea = a->next;
                    deleteAsteroid(&g, a);
                    a = savea;
                    g.nasteroids--;
                }
                //delete the bullet...
                memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
                g.nbullets--;
                if (a == NULL)
                    break;
            }
            i++;
        }
        if (a == NULL)
            break;
        a = a->next;
        // Priyanshu's code:~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //checkbuffbomb(countdown);
        if (countdown != 90 && countdown%15 == 0)
            buffbomb = true;

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
        //buffbomb();

        //Written by Carlos----------------------------------------------------------------------------------------------------//
        // Check if asteroid count is below 10
        if (g.nasteroids < 10) {
            // Spawn another wave of asteroids
            for (int j = 0; j < 10; j++) {
                Asteroid *a = new Asteroid;
                a->nverts = 6;
                a->radius = rnd() * 20.0 + 40.0;
                a->angle = 0.0f;
                a->color[0] = 1.0;
                a->color[1] = 1.0;
                a->color[2] = 1.0;

                a->vel[0] = (Flt)(rnd() * 2.0 - 1.0);
                a->vel[1] = (Flt)(rnd() * 2.0 - 1.0);

                int side = random(4);
                if (side == 0) {
                    a->pos[0] = 0.0f;
                    a->pos[1] = rnd() * (float)gl.yres;
                } else if (side == 1) {
                    a->pos[0] = rnd() * (float)gl.xres;
                    a->pos[1] = (float)gl.yres;
                } else if (side == 2) {
                    a->pos[0] = (float)gl.xres;
                    a->pos[1] = rnd() * (float)gl.yres;
                } else {
                    a->pos[0] = rnd() * (float)gl.xres;
                    a->pos[1] = 0.0f;
                }

                a->next = g.ahead;
                if (g.ahead != NULL) {
                    g.ahead->prev = a;
                }
                g.ahead = a;
                ++g.nasteroids;
            }
        }	
    }

    //---------------------------------------------------
    //check keys pressed now
    if (gl.keys[XK_Left]) {
        g.ship.angle = 90;
        g.ship.pos[0] = g.ship.pos[0] - speedrate;
        /*
           g.ship.angle += 8.0;
           if (g.ship.angle >= 360.0f)
           g.ship.angle -= 360.0f;
           */
    }
    if (gl.keys[XK_Right]) {
        g.ship.angle = 270;
        g.ship.pos[0] = g.ship.pos[0] + speedrate;
        /*
           g.ship.angle -= 8.0;
           if (g.ship.angle < 0.0f)
           g.ship.angle += 360.0f;
           */
    }
    if (gl.keys[XK_Up]) {
        //apply thrust
        //convert ship angle to radians
        /*
           Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
        //convert angle to a vector

        Flt xdir = cos(rad);
        Flt ydir = sin(rad);
        g.ship.pos[1] = g.ship.pos[1] + 1;

        g.ship.vel[0] += xdir*0.01f;
        g.ship.vel[1] += ydir*0.01f;
        Flt speed = sqrt(g.ship.vel[0]*g.ship.vel[0]+
        g.ship.vel[1]*g.ship.vel[1]);
        if (speed > 0.5f) {
        speed = 0.5f;
        normalize2d(g.ship.vel);
        g.ship.vel[0] *= speed;
        g.ship.vel[1] *= speed;
        }
        */
        g.ship.angle = 360;
        g.ship.pos[1] = g.ship.pos[1] + speedrate;
    }
    if (gl.keys[XK_Down]) {
        g.ship.angle = 180;
        g.ship.pos[1] = g.ship.pos[1] - speedrate;
    }
    if (gl.keys[XK_Right] && gl.keys[XK_Up])
        g.ship.angle = 315;
    if (gl.keys[XK_Left] && gl.keys[XK_Up])
        g.ship.angle = 45;
    if (gl.keys[XK_Right] && gl.keys[XK_Down])
        g.ship.angle = 225;
    if (gl.keys[XK_Left] && gl.keys[XK_Down])
        g.ship.angle = 135;
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
                if (shotgun_mode == true) {
                    Bullet *b = &g.barr[g.nbullets];
                    Bullet *b1 = &g.barr[g.nbullets];
                    Bullet *b2 = &g.barr[g.nbullets];
                    timeCopy(&b->time, &bt);
                    b->pos[0] = g.ship.pos[0];
                    b->pos[1] = g.ship.pos[1];
                    b->vel[0] = g.ship.vel[0];
                    b->vel[1] = g.ship.vel[1];
                    
                    b1->pos[0] = g.ship.pos[0] - 10;
                    b1->pos[1] = g.ship.pos[1];
                    b1->vel[0] = g.ship.vel[0];
                    b1->vel[1] = g.ship.vel[1];
                    
                    b2->pos[0] = g.ship.pos[0] + 10;
                    b2->pos[1] = g.ship.pos[1];
                    b2->vel[0] = g.ship.vel[0];
                    b2->vel[1] = g.ship.vel[1];
                    
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

                    b1->pos[0] += xdir*20.0f;
                    b1->pos[1] += ydir*20.0f;
                    b1->vel[0] += xdir*6.0f + rnd()*0.1;
                    b1->vel[1] += ydir*6.0f + rnd()*0.1;
                    b1->color[0] = 1.0f;
                    b1->color[1] = 1.0f;
                    b1->color[2] = 1.0f;

                    b2->pos[0] += xdir*20.0f;
                    b2->pos[1] += ydir*20.0f;
                    b2->vel[0] += xdir*6.0f + rnd()*0.1;
                    b2->vel[1] += ydir*6.0f + rnd()*0.1;
                    b2->color[0] = 1.0f;
                    b2->color[1] = 1.0f;
                    b2->color[2] = 1.0f;
                    g.nbullets++;
                }
                else {
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
    }
    if (g.mouseThrustOn) {
        //should thrust be turned off
        if(gl.mouse)
            return;
        //should thrust be turned off
        struct timespec mtt;
        clock_gettime(CLOCK_REALTIME, &mtt);
        double tdif = timeDiff(&mtt, &g.mouseThrustTimer);
        //std::cout << "tdif: " << tdif << std::endl;
        if (tdif < -0.3)
            g.mouseThrustOn = false;
    }

    int activeCoins = 0;
    for (int i = 0; i < 5; i++) {
        if (g.coins[i].active) {
            activeCoins++;

            // Check for collision with the ship
            Flt dx = g.ship.pos[0] - g.coins[i].pos[0];
            Flt dy = g.ship.pos[1] - g.coins[i].pos[1];
            Flt dist = sqrt(dx*dx + dy*dy);

            if (dist < (g.ship.radius + g.coins[i].radius)) {
                // Collision detected
                g.coins[i].active = false;
                g.score += 200; // Add points to score
                g.collectedCoins++; // Increment the collected coins counter
                if (g.collectedCoins == 5) {
                }
                coins_buffs(g.collectedCoins);
                // if (result = 1)
                activeCoins--;
            }
        }
    }
    // If less than 5 coins are active, spawn new ones
    while (activeCoins < 5) {
        g.spawnCoin();
        activeCoins++;
    }
}

void render()
{
    Rect r;
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, gl.grassTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex2i(0, gl.yres);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(gl.xres, gl.yres);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(gl.xres, 0);
    glEnd();
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`

    r.bot = gl.yres - 30;
    r.left = 10;
    r.center = 0;
    ggprint16(&r, 16, 0x00ff0000, "Copyright Onslaught");
    ggprint16(&r, 16, 0x00ff0000, "Number of bullets: %i", g.nbullets);
    ggprint16(&r, 16, 0x00ff0000, "Number of Enemies: %i", g.nasteroids);
    ggprint16(&r, 16, 0x00ff0000, "Number of Lives: %i", Lives);
    ggprint16(&r, 16, 0x00ff0000, "Time left: %i", countdown);
    ggprint16(&r, 16, 0x00ff0000, "Collected Coins: %i", g.collectedCoins);
    ggprint16(&r, 16, 0x00ff0000, "Score: %i", g.score);
/*
    r.bot = gl.yres / 2;
    r.left = gl.xres / 2;
    r.center = 1;
    ggprint16(&r, 16, 0xFFFFFFFF, "You have gained a life!");
*/
    ggprint16(&r, 16, 0x00ff0000, "");

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
    //showbuffbomb();
    if (buffbomb) {
        ggprint16(&r, 16, 0x00ffff00, "Bomb is active, Press b to Blast!");
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
    if (isPaused) {
        displayPauseMenu();
        return;
    }

    if (gameWon) {
        displayYouWin(); // Display "You Win" screen
        return;
    }

    if (displayMessage) {   
        struct timespec now;
        clock_gettime(CLOCK_REALTIME, &now);
        double timePassed = timeDiff(&firstMoveTime, &now);
        if (timePassed < 3.0) {
            // Display the message for 3 seconds
            Rect r;
            r.bot = gl.yres / 1.5;
            r.left = gl.xres / 2;
            r.center = 1;
            ggprint16(&r, 16, 0x00ff0000, "Survive 90 Seconds to Win!");
        } else {
            displayMessage = false;
        }
    }

    if (gl.nightmodefilter){
        nightmodefilter(gl.xres, gl.yres); // Displays a tinted filter on the screen, mimicing a "Night Time" feel
    }

    if (gl.show_toggle){
        display_border(gl.xres, gl.yres); // Displays a border around the window of the game, letting the player know the WASD toggle is "on" or "off"
        display_toggle(10, 100);
    }

    if (gl.statistics) {
        glColor3ub(100,100,100);
        glPushMatrix();
        glTranslatef(20.0, 20.0, 0.0);
        int w = 230;
        int h = 130;
        glBegin(GL_QUADS);
        glVertex2f(0,0);
        glVertex2f(0,h);
        glVertex2f(w,h);
        glVertex2f(w,0);
        glEnd();
        glPopMatrix();
        r.bot = 123;
        r.left = 28;
        r.center = 0;
        ggprint13(&r, 20, 0x0055ff55, "Statistics...");
        ggprint13(&r, 16, 0x00ffff00, "sec_running_time: %i", 
                total_running_time(true));
        ggprint13(&r, 16, 0x00ffff00, "n_physics_function_calls: %i", 
                total_physics_function_calls(true));
        ggprint13(&r, 16, 0x00ffff00, "time_since_mouse_move: %i",
                time_since_mouse_move(true));
        ggprint13(&r, 16, 0x00ffff00, "mouse distance: %lf", 
                mouse_movement_distance(-1, -1, true));
    }

    for (int i = 0; i < 5; i++) {
        if (g.coins[i].active) {
            // Draw the coin
            glColor3f(1.0, 0.84, 0); // gold color for the coin
            glBegin(GL_POLYGON);
            for (int j = 0; j < 360; j += 5) {
                float degInRad = j * DEG2RAD;
                glVertex2f(cos(degInRad) * g.coins[i].radius + g.coins[i].pos[0],
                        sin(degInRad) * g.coins[i].radius + g.coins[i].pos[1]);
            }
            glEnd();
        }
    }

    //-------------------------------------------------------------------------
    //Draw the ship
    //glColor3fv(g.ship.color);
    glPushMatrix();
    glTranslatef(g.ship.pos[0], g.ship.pos[1], g.ship.pos[2]);
    //float angle = atan2(ship.dir[1], ship.dir[0]);
    glRotatef(g.ship.angle, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    // glVertex2f(-10.0f, -10.0f);
    // glVertex2f(  0.0f, 20.0f);
    // glVertex2f( 10.0f, -10.0f);
    glVertex2f(-12.0f, -10.0f);
    glVertex2f(  0.0f,  20.0f);
    glVertex2f(  0.0f,  -6.0f);
    glVertex2f(  0.0f,  -6.0f);
    glVertex2f(  0.0f,  20.0f);
    glVertex2f( 12.0f, -10.0f);
    glEnd();
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    glVertex2f(0.0f, 0.0f);
    glEnd();
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
    float wid = 20.0f;
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, gl.mcTexture);

    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(-wid,-wid);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(-wid, wid);
    glTexCoord2f(0.0f, 0.0f); glVertex2i( wid, wid);
    glTexCoord2f(0.0f, 1.0f); glVertex2i( wid,-wid);
    glEnd();
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
    glPopMatrix();
    /*
    if (gl.keys[XK_Up] || g.mouseThrustOn) {
        int i;
        //draw thrust
        Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
        //convert angle to a vector
        Flt xdir = cos(rad);
        Flt ydir = sin(rad);
        Flt xs,ys,xe,ye,r;
        glBegin(GL_LINES);
        for (i=0; i<16; i++) {
            xs = -xdir * 11.0f + rnd() * 4.0 - 2.0;
            ys = -ydir * 11.0f + rnd() * 4.0 - 2.0;
            r = rnd()*40.0+40.0;
            xe = -xdir * r + rnd() * 18.0 - 9.0;
            ye = -ydir * r + rnd() * 18.0 - 9.0;
            glColor3f(rnd()*.3+.7, rnd()*.3+.7, 0);
            glVertex2f(g.ship.pos[0]+xs,g.ship.pos[1]+ys);
            glVertex2f(g.ship.pos[0]+xe,g.ship.pos[1]+ye);
        }
        glEnd();

    }
    */
    //-------------------------------------------------------------------------
    //Draw the asteroids
    {
        Asteroid *a = g.ahead;
        while (a) {

            glPushMatrix();
            glTranslatef(a->pos[0], a->pos[1], a->pos[2]);

            glColor3f(0.0, 1.0, 0.0);  // Orange outline color
            glLineWidth(1.0f);	

            if (a->radius > 15)
                a-> radius = 15;
            // Draw a hexagon
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
            glBegin(GL_LINE_LOOP);
            for (int j = 0; j < 4; j++) {
                float angle = 2.0f * 3.1415926 * j / 4.0;
                float x = a->radius * cos(angle);
                float y = a->radius * sin(angle);
                glVertex2f(x, y);
            }
            glEnd();

            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            wid = 0.7f * (float)a->radius;
            glColor4f(1.0, 1.0, 1.0, 1.0);

            glBindTexture(GL_TEXTURE_2D, gl.enemyTexture);
            glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 1.0f); glVertex2i(-wid,-wid);
            glTexCoord2f(1.0f, 0.0f); glVertex2i(-wid, wid);
            glTexCoord2f(0.0f, 0.0f); glVertex2i( wid, wid);
            glTexCoord2f(0.0f, 1.0f); glVertex2i( wid,-wid);
            glEnd();
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
            glPopMatrix();
            a = a->next;
        }
    }
    //-------------------------------------------------------------------------
    //Draw the bullets
    for (int i=0; i<g.nbullets; i++) {
        Bullet *b = &g.barr[i];
        //Log("draw bullet...\n");
        glColor3f(0.4, 0.2, 1.0);
        glBegin(GL_POINTS);
        glVertex2f(b->pos[0],      b->pos[1]);
        glVertex2f(b->pos[0]-1.0f, b->pos[1]);
        glVertex2f(b->pos[0]+1.0f, b->pos[1]);
        glVertex2f(b->pos[0],      b->pos[1]-1.0f);
        glVertex2f(b->pos[0],      b->pos[1]+1.0f);
        glColor3f(0.8, 0.1, 0.8);
        glVertex2f(b->pos[0]-1.0f, b->pos[1]-1.0f);
        glVertex2f(b->pos[0]-1.0f, b->pos[1]+1.0f);
        glVertex2f(b->pos[0]+1.0f, b->pos[1]-1.0f);
        glVertex2f(b->pos[0]+1.0f, b->pos[1]+1.0f);
        glEnd();
    }
}

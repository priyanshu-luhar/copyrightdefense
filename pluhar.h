#ifndef _PLUHAR_H_
#define _PLUHAR_H_

extern bool wasdvar;
extern int render_calls;
extern int physics_calls;
extern bool wasdtoggle();
extern int wasd(int key);
extern double mouse_movement_distance(int x, int y, bool temp);
extern int psavex;
extern int psavey;
/*
extern bool buffbomb;
extern void buffbomb();
extern void checkbuffbomb(int count);
extern void showbuffbomb();
*/
#endif

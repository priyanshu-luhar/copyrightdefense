#ifndef _CSANDOVAL_H_
#define _CSANDOVAL_H_


#include "header.h"

//carlos's project files

//called from main program.
extern void display_toggle(int, int);
extern void display_border(int, int);
extern void nightmodefilter(int, int);
extern void handleMenu();
extern void checkShipAsteroidCollision();

extern int total_running_time(const bool);
extern int total_physics_function_calls(bool temp);
extern int physics_function_counter;
extern bool isStopped;





#endif //_CARLOS_

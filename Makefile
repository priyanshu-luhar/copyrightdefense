CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: asteroids

asteroids: asteroids.cpp log.cpp timers.cpp csandoval.cpp pluhar.cpp jhernandez2.cpp pluhar.h jhernansez2.h csandoval.h  
	g++ $(CFLAGS) asteroids.cpp log.cpp csandoval.cpp pluhar.cpp jhernandez2.cpp timers.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -oasteroids

clean:
	rm -f asteroids
	rm -f *.o

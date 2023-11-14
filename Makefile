CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: copyright_onslaught

copyright_onslaught: copyright_onslaught.cpp log.cpp timers.cpp csandoval.cpp pluhar.cpp jhernandez2.cpp pluhar.h jhernandez2.h csandoval.h 
	g++ $(CFLAGS) copyright_onslaught.cpp log.cpp csandoval.cpp pluhar.cpp jhernandez2.cpp timers.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -ocopyright_onslaught -O4

clean:
	rm -f copyright_onslaught
	rm -f *.o

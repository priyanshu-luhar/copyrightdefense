CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr
EFLAGS = -Wall -Wextra #Error Flags

all: game

game: main.cpp log.cpp timers.cpp csandoval.cpp pluhar.cpp jhernandez2.cpp pluhar.h jhernandez2.h csandoval.h header.h 
	clear
	g++ $(CFLAGS) main.cpp log.cpp csandoval.cpp pluhar.cpp jhernandez2.cpp timers.cpp libggfonts.a $(EFLAGS) $(LFLAGS) -o game

clean:
	rm -f game
	rm -f *.o

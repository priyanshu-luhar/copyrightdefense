CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr
EFLAGS = -Wall -Wextra #Error Flags

all: game game_w_music

game: main.cpp log.cpp timers.cpp csandoval.cpp pluhar.cpp jhernandez2.cpp pluhar.h jhernandez2.h csandoval.h header.h 
	g++ $(CFLAGS) main.cpp log.cpp csandoval.cpp pluhar.cpp jhernandez2.cpp timers.cpp libggfonts.a $(EFLAGS) $(LFLAGS) -o game

game_w_music: main.cpp log.cpp timers.cpp csandoval.cpp pluhar.cpp jhernandez2.cpp pluhar.h jhernandez2.h csandoval.h header.h
    g++ $(CFLAGS) main.cpp log.cpp csandoval.cpp pluhar.cpp jhernandez2.cpp timers.cpp libggfonts.a $(EFLAGS) $(LFLAGS) \
    -D USE_OPENAL_SOUND /usr/lib/x86_64-linux-gnu/libopenal.so /usr/lib/x86_64-linux-gnu/libalut.so -o game_w_music

clean:
	rm -f game
	rm -f *.o

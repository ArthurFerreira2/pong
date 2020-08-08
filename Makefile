LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer
FLAGS = -std=c99 -pedantic -Wpedantic -Wall -Werror -O3 -Wl,-subsystem,windows
CC = gcc

pong: pong.c
	$(CC) $^ $(FLAGS) $(LIBS) -o $@

all: pong

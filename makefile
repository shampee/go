PROG = go
CC = gcc

include common.mk

CXXFLAGS += `sdl2-config --cflags`
CXXFLAGS += -g -lefence -Wall -pedantic

LDFLAGS += `sdl2-config --libs` -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lSDL2_net -lm -lpthread

# linking the program.
$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

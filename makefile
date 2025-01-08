# A simple Makefile for compiling small SDL projects

# set the compiler
CC := g++

# set the compiler flags
CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c++11 -Wall -lSDL2_image -lSDL2_ttf -lm -g

# add header files here
HDRS := Grid.h

# add source files here
SRCS := Grid.cpp Pathfinder.cpp

# generate names of object files
OBJS := $(SRCS:.cpp=.o)

# name of executable
EXEC := Pathfinder

# default recipe
all: $(EXEC)

# recipe for building the final executable
$(EXEC): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

# recipe for building object files
%.o: %.cpp $(HDRS)
	$(CC) -c $< -o $@ $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean run

run: $(EXEC)
	./$(EXEC)
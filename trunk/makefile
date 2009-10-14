CC = g++
CFLAGS = -Wall -O2
COMPILE = $(CC) $(CFLAGS) -c
LIBS = -lglut -lglui -lm

all: trabalho

trabalho: standingwaves.o circles.o fractal.o piriform.o main.o
	$(CC) -o trabalho standingwaves.o circles.o fractal.o piriform.o main.o -Wall $(LIBS)

main.o: main.cpp
	$(COMPILE) -o main.o main.cpp

circles.o: circles.cpp
	$(COMPILE) -o circles.o circles.cpp

fractal.o: fractal.cpp
	$(COMPILE) -o fractal.o fractal.cpp

piriform.o: piriform.cpp
	$(COMPILE) -o piriform.o piriform.cpp

standingwaves.o: standingwaves.cpp
	$(COMPILE) -o standingwaves.o standingwaves.cpp

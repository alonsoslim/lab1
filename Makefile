# cs335 lab1
# to compile your project, type make and press enter
CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm

all: lab1

lab1: lab1.cpp
	g++ $(CFLAGS) lab1.cpp libggfonts.a -Wall $(LFLAGS) -olab1 #-lX11 -lGL -lGLU -lm

clean:
	rm -f lab1
	rm -f *.o


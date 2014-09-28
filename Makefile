CC = g++
CFLAGS = -g -Wall -Wextra -pedantic 
IPATH = -I/usr/X11/include -I/usr/pkg/include
LPATH = -L/usr/X11/lib -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
RM = rm
all: Ass2

Ass2: world.o particle.o main.o
	$(CC) -o $@ $^ -lm -lGL -lGLU -lglut $(LPATH)
	
.cpp.o: 
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)
	
clean:
	$(RM) -f *.o *.gch Ass2

CC	= gcc
#OPT    = -g
OPT    = -O3
CFLAGS = -pedantic -Wall -c $(OPT) -I/usr/include/X11/motif
LFLAGS = $(OPT) -L/usr/lib/X11/lib
LIBS = -lXm -lXt -lX11

izzy: izzy.o
	$(CC) $(LFLAGS) -o izzy izzy.o $(LIBS)

izzy.o : izzy.c izzy.h
	$(CC) $(CFLAGS) izzy.c

clean :
	rm izzy.o izzy

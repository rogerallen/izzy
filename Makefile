CC	= gcc
#OPT    = -g
OPT    = -O3
CFLAGS = -std=c17 -pedantic -Wall -Wextra $(OPT) -I/usr/include/X11/motif
LFLAGS = -L/usr/lib/X11/lib
LIBS = -lXm -lXt -lX11

izzy : izzy.c izzy.h
	$(CC) $(CFLAGS) $(LFLAGS) izzy.c -o izzy $(LIBS)

clean :
	rm izzy

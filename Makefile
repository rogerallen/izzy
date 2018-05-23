CC	= gcc
OPT    = -g
#OPT    = -O
CFLAGS = -pedantic -Wall -c $(OPT) -I/usr/include/X11/motif
LFLAGS = $(OPT) -L/usr/lib/X11/lib
LIBS = -lXm -lXt -lX11

# for DEFINES -
# define the defines used here
DEFINES = -DANSI

OBJECTS = izzy.o xs_wprintf.o
CFILES = izzy.c xs_wprintf.c
HFILES = izzy.h xs_wprintf.h

izzy: $(OBJECTS)
	$(CC) $(LFLAGS) $(DEFINES) -o izzy $(OBJECTS) $(LIBS)

clean :
	rm $(OBJECTS) izzy

izzy.o : izzy.c $(HFILES)
	$(CC) $(CFLAGS) $(DEFINES) izzy.c

xs_wprintf.o 	: xs_wprintf.c
	$(CC) $(CFLAGS) $(DEFINES) xs_wprintf.c

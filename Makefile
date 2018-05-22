CC	= cc
OPT    = -g
#OPT    = -O
CFLAGS = -Wall -c $(OPT) -I/usr/include/X11/motif
LFLAGS = $(OPT) -L/usr/lib/X11/lib
#LIBS = -lXm -lXtm -lX11
#LIBS = -Bstatic -lXm -Bdynamic -lXt -lX11
LIBS = -lXm -lXt -lX11

# for DEFINES -
# define the defines used here
DEFINES = -DANSI

OBJECTS = imain.o iinit.o xs_wprintf.o iparse.o	ialarm.o

CFILES = imain.c iinit.c xs_wprintf.c iparse.c ialarm.c

HFILES = ialarm.h  iinit.h  imain.h  iparse.h  izzy.h  xs_wprintf.h

izzy: $(OBJECTS)
	$(CC) $(LFLAGS) $(DEFINES) -o izzy $(OBJECTS) $(LIBS)

clean :
	rm $(OBJECTS) izzy

imain.o : imain.c $(HFILES)
	$(CC) $(CFLAGS) $(DEFINES) imain.c

iinit.o : iinit.c $(HFILES)
	$(CC) $(CFLAGS) $(DEFINES) iinit.c

iparse.o : iparse.c $(HFILES)
	$(CC) $(CFLAGS) $(DEFINES) iparse.c

ialarm.o : ialarm.c $(HFILES)
	$(CC) $(CFLAGS) $(DEFINES) ialarm.c

xs_wprintf.o 	: xs_wprintf.c
	$(CC) $(CFLAGS) $(DEFINES) xs_wprintf.c
tar:
	tar cf izzy.tar $(CFILES) $(HFILES) Makefile man1/izzy.1

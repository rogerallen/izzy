CC	   = gcc
OPT    = $(if ${DEBUG},-g,-O3)    # control via DEBUG=1 on cmdline
CFLAGS = -std=c17 -pedantic -Wall -Wextra $(OPT) \
         -I/usr/include/X11/motif -L/usr/lib/X11/lib
LIBS   = -lXm -lXt -lX11

izzy : izzy.c
	$(CC) $(CFLAGS) $(LFLAGS) izzy.c -o izzy $(LIBS)

clean :
	rm -f izzy

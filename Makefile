CC=gcc
CCOPTS=--std=gnu99  -w
AR=ar

OBJS=daje.o\
	 read.o\
	 signal_sender.o\
	 terminal_control.o

HEADERS=read.h\
		signal_sender.h\
		terminal_control.h

LIBS=daje.a

BINS=daje

#daje

.phony: clean all


all:	$(BINS) $(LIBS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

daje.a:	$(OBJS) 
	$(AR) -rcs $@ $^


daje:	daje.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ 

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)

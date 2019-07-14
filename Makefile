CC = gcc
CFLAGS = -Wall -pedantic -std=c89
ifeq ($(debug), 1)
        CFLAGS = -Wall -pedantic -std=c89 -g --debug
endif
PL = parseline
PL_OBJS = main.o parseline.o
all : $(PL)

$(PL) : $(PL_OBJS) parseline.h
	$(CC) $(CFLAGS) -o $(PL) $(PL_OBJS) -lm

main.o : main.c parseline.h
	$(CC) $(CFLAGS) -c main.c

parseline.o : parseline.c parseline.h
	$(CC) $(CFLAGS) -c parseline.c

clean :
	rm *.o $(PL)

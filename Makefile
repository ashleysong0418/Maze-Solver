# Makefile for Maze Project
#
# Ashley Song, CS50, 19X
# Lab 6


PROG=client
OBJS=client.o maze.o movement.o avatar.o functions.o
LIBS=./libcs50/libcs50.a
# TEST= testing.sh
# TESTING=-DMEMTEST

CFLAGS= -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../libcs50 -lpthread
CC= gcc
MAKE= make

$(PROG): $(OBJS)
				$(CC) $(CFLAGS) $^ $(LIBS) -o $@


functions.o: functions.h
maze.o: maze.h
movement.o: maze.h movement.h
avatar.o: common.h ./libcs50/memory.h ./avatar.h ./maze.h ./movement.h
client.o: common.h ./libcs50/memory.h ./avatar.h ./maze.h ./movement.h

.PHONY: clean withGui all

withGui:
	javac GUI.java

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f *.class

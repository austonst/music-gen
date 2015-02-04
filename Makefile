# Copyright (c) 2014 Auston Sterling
# See LICENSE for copying permission.

CC = g++ -g
CFLAGS = -Wall -Wextra -c -std=c++11
LFLAGS =

TESTMOTIF_OBJ = testmotif.o motif.o libmidi.a
TESTTHEME_OBJ = testtheme.o theme.o motif.o libmidi.a
TESTPIECE_OBJ = testpiece.o piece.o theme.o motif.o libmidi.a

test: testmotif testtheme testpiece

testmotif: $(TESTMOTIF_OBJ)
	$(CC) $(TESTMOTIF_OBJ) $(LFLAGS) -o testmotif

testtheme: $(TESTTHEME_OBJ)
	$(CC) $(TESTTHEME_OBJ) $(LFLAGS) -o testtheme

testpiece: $(TESTPIECE_OBJ)
	$(CC) $(TESTPIECE_OBJ) $(LFLAGS) -o testpiece

testmotif.o: testmotif.cpp motif.o
	$(CC) testmotif.cpp $(CFLAGS)

testtheme.o: testtheme.cpp theme.o
	$(CC) testtheme.cpp $(CFLAGS)

testpiece.o: testpiece.cpp piece.o
	$(CC) testpiece.cpp $(CFLAGS)

piece.o: piece.cpp piece.hpp theme.o
	$(CC) piece.cpp $(CFLAGS)

theme.o: theme.cpp theme.hpp motif.o
	$(CC) theme.cpp $(CFLAGS)

motif.o: motif.cpp motif.hpp
	$(CC) motif.cpp $(CFLAGS)


CPP=g++

CFLAGS=-g -std=c++11

BINS=logic tstClient updateInputs

all:	$(BINS)

logic:	logic.cpp plc.o
	$(CPP) $(CFLAGS) logic.cpp plc.o -o logic -lsqlite3

plc.o:	plc.cpp plc.h
	$(CPP) $(CFLAGS) -c plc.cpp -o plc.o

tstClient:	tstClient.c
	$(CPP) $(CFLAGS) tstClient.c -o tstClient

myClientClass.o:	myClientClass.cpp myClientClass.h
	$(CPP) -c $(CFLAGS) myClientClass.cpp -o myClientClass.o

updateInputs:	updateInputs.cpp myClientClass.h myClientClass.o
	$(CPP) $(CFLAGS) updateInputs.cpp myClientClass.o -o updateInputs

clean:
	rm -f $(BINS) *.o

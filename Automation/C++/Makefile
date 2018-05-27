
CPP=g++

CFLAGS=-g -std=c++11

BINS=logic tstClient updateInputs updateOutputs

all:	$(BINS)

logic:	logic.cpp plc.o myClientClass.o semaphore.o
	$(CPP) $(CFLAGS) logic.cpp plc.o myClientClass.o semaphore.o -o logic # -lsqlite3

plc.o:	plc.cpp plc.h
	$(CPP) $(CFLAGS) -c plc.cpp -o plc.o

tstClient:	tstClient.c
	$(CPP) $(CFLAGS) tstClient.c -o tstClient

myClientClass.o:	myClientClass.cpp myClientClass.h
	$(CPP) -c $(CFLAGS) myClientClass.cpp -o myClientClass.o

updateInputs:	updateInputs.cpp myClientClass.h myClientClass.o semaphore.o
	$(CPP) $(CFLAGS) updateInputs.cpp myClientClass.o semaphore.o -o updateInputs -lmosquitto

updateOutputs:	updateOutputs.cpp myClientClass.h myClientClass.o
	$(CPP) $(CFLAGS) updateOutputs.cpp myClientClass.o -o updateOutputs -lmosquitto

semaphore.o:	semaphore.cpp
	$(CPP) -c $(CFLAGS) semaphore.cpp -o semaphore.o
clean:
	rm -f $(BINS) *.o

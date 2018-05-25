
CPP=g++

CFLAGS=-g

BINS=logic tstClient

all:	$(BINS)

logic:	logic.cpp plc.o
	$(CPP) $(CFLAGS) logic.cpp plc.o -o logic -lsqlite3

plc.o:	plc.cpp plc.h
	$(CPP) $(CFLAGS) -c plc.cpp -o plc.o

tstClient:	tstClient.c
	$(CPP) $(CFLAGS) tstClient.c -o tstClient

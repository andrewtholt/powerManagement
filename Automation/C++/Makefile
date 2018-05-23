
CPP=g++

CFLAGS=-g

BINS=logic

logic:	logic.cpp plc.o
	$(CPP) $(CFLAGS) logic.cpp plc.o -o logic -lsqlite3

plc.o:	plc.cpp plc.h
	$(CPP) $(CFLAGS) -c plc.cpp -o plc.o


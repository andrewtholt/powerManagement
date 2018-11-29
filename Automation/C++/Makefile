
CPP=g++

CFLAGS=-g -std=c++11

BINS=tstPlc # logic tstClient # updateInputs updateOutputs 
LIBS= -lpthread # -L/usr/local/lib -lMyClientSocket -lspreadhelper 
INCS=-I/usr/local/include 

all:	$(BINS)

tstPlc:	tstPlc.cpp myClientClass.o semaphore.o plcMQTT.o plcBase.o
	$(CPP) $(CFLAGS) tstPlc.cpp plcMQTT.o plcBase.o myClientClass.o semaphore.o -o tstPlc $(LIBS) # -lsqlite3

logic:	logic.cpp plc.o myClientClass.o semaphore.o 
	$(CPP) $(CFLAGS) logic.cpp plc.o myClientClass.o semaphore.o -o logic $(LIBS) # -lsqlite3

# plc.o:	plc.cpp plc.h
#	$(CPP) $(CFLAGS) -c plc.cpp -o plc.o

plcBase.o:	plcBase.cpp plcBase.h
	$(CPP) $(CFLAGS) -c plcBase.cpp -o plcBase.o

plcMQTT.o:	plcMQTT.cpp plcMQTT.h plcBase.o
	$(CPP) $(CFLAGS) -c plcMQTT.cpp -o plcMQTT.o

tstClient:	tstClient.c
	$(CPP) $(CFLAGS) tstClient.c -o tstClient

myClientClass.o:	myClientClass.cpp myClientClass.h
	$(CPP) -c $(CFLAGS) myClientClass.cpp -o myClientClass.o

updateInputs:	updateInputs.cpp myClientClass.h myClientClass.o semaphore.o 
	$(CPP) $(CFLAGS) $(INCS) updateInputs.cpp myClientClass.o semaphore.o -o updateInputs $(LIBS) -lmosquitto 

updateOutputs:	updateOutputs.cpp myClientClass.h myClientClass.o
	$(CPP) $(CFLAGS) updateOutputs.cpp myClientClass.o -o updateOutputs $(LIBS) -lmosquitto

semaphore.o:	semaphore.cpp
	$(CPP) -c $(CFLAGS) semaphore.cpp -o semaphore.o
	
clean:
	rm -f $(BINS) *.o

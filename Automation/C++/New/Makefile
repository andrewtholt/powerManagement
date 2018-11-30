
CPP=g++

CFLAGS=-g -std=c++11

BINS=tstPlc 
LIBS= -lpthread 

all:	$(BINS)

tstPlc:	tstPlc.cpp plcMQTT.o plcBase.o
	$(CPP) $(CFLAGS) tstPlc.cpp plcMQTT.o plcBase.o -o tstPlc $(LIBS) # -lsqlite3

plcBase.o:	plcBase.cpp plcBase.h
	$(CPP) $(CFLAGS) -c plcBase.cpp -o plcBase.o

plcMQTT.o:	plcMQTT.cpp plcMQTT.h plcBase.o
	$(CPP) $(CFLAGS) -c plcMQTT.cpp -o plcMQTT.o

clean:
	rm -f $(BINS) *.o

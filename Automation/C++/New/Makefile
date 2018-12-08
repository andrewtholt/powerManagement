
CPP=g++ -Wl,--no-as-needed


BINS=tstPlc mainTestAll
LIBS= -lpthread -lmosquitto -lsqlite3 -lgtest
TST_OBJS = ./Test/plcTests.o plcBase.o plcMQTT.o
INCS=-I.
CXXFLAGS = -fmax-errors=5 -g 

TEST?=1 # set thsi to 1 to enable testing

CFLAGS=-g -std=c++11 -DTEST=$(TEST)

all:	$(BINS)

tstPlc:	tstPlc.cpp plcMQTT.o plcBase.o Makefile
	$(CPP) $(CFLAGS) tstPlc.cpp plcMQTT.o plcBase.o -o tstPlc $(LIBS) $(INCS)

plcBase.o:	plcBase.cpp plcBase.h
	$(CPP) $(CFLAGS) -c plcBase.cpp -o plcBase.o

plcMQTT.o:	plcMQTT.cpp plcMQTT.h plcBase.o
	$(CPP) $(CFLAGS) -c plcMQTT.cpp -o plcMQTT.o

clean:
	rm -f $(BINS) *.o

mainTestAll:	mainTestAll.cpp plcMQTT.o plcBase.o $(TST_OBJS)
	$(CPP) $(CXXFLAGS) mainTestAll.cpp -o mainTestAll $(LIBS) $(TST_OBJS)

test:	mainTestAll
	./mainTestAll

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

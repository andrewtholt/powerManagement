
CPP=g++ -std=c++11 -Wl,--no-as-needed

BINS=tstDatabase backLight porchLight tstPlc libplc.so # mainTestAll 
LIBS= -lpthread -lrt -lmosquitto -lsqlite3 -lgtest
TST_OBJS = ./Test/plcTests.o plcBase.o plcMQTT.o
INCS=-I.
CXXFLAGS = -fmax-errors=5 -g -fPIC

TEST?=1 # set this to 1 to enable testing

CFLAGS=-g -std=c++11 -DTEST=$(TEST)  $(CXXFLAGS)

all:	$(BINS)

help:
	@echo "Help"
	@echo "make mainTestAll\tBuilds test harness"
	@echo ""

porchLight:	porchLight.cpp Makefile libplc.so
	$(CPP) $(CFLAGS) porchLight.cpp -o porchLight -L. -lplc -lpthread -lrt -lmosquitto -lsqlite3 $(INCS)

backLight:	backLight.cpp Makefile libplc.so
	$(CPP) $(CFLAGS) backLight.cpp -o backLight -L. -lplc -lpthread -lrt -lmosquitto -lsqlite3 $(INCS)

tstPlc:	tstPlc.cpp Makefile libplc.so
	$(CPP) $(CFLAGS) tstPlc.cpp -o tstPlc -L. -lplc -lpthread -lrt -lmosquitto -lsqlite3 $(INCS)

tstDatabase:	tstDatabase.cpp Makefile plcDatabase.o
	$(CPP) $(CFLAGS) tstDatabase.cpp -o tstDatabase plcBase.o plcDatabase.o -L. -lpthread -lrt -lsqlite3 $(INCS)

plcBase.o:	plcBase.cpp plcBase.h
	$(CPP) $(CFLAGS) -c plcBase.cpp -o plcBase.o

plcMQTT.o:	plcMQTT.cpp plcMQTT.h plcBase.o
	$(CPP) $(CFLAGS) -c plcMQTT.cpp -o plcMQTT.o

plcDatabase.o:	plcDatabase.cpp plcDatabase.h plcBase.o
	$(CPP) $(CFLAGS) -c plcDatabase.cpp -o plcDatabase.o

install:	libplc.so
	./install.sh

clean:
	rm -f $(BINS) *.o Test/*.o

mainTestAll:	mainTestAll.cpp plcMQTT.o plcBase.o $(TST_OBJS)
	$(CPP) $(CXXFLAGS) mainTestAll.cpp -o mainTestAll $(LIBS) $(TST_OBJS)

libplc.so:	plcBase.o plcMQTT.o
	$(CPP) -shared plcBase.o plcMQTT.o -o libplc.so

test:	mainTestAll
	./mainTestAll

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

CPP=g++

BINS = Server2 interpTest

all:	$(BINS)

Server2:	Server2.cpp interp.o utils.o
	$(CPP) -g Server2.cpp interp.o utils.o -o Server2 -lpthread

interpTest:	interpTest.cpp interp.o
	$(CPP) -g interpTest.cpp interp.o -o interpTest -lrt -lmysql

interp.o:	interp.cpp interp.h
	$(CPP) -c -g interp.cpp -o interp.o

utils.o:	utils.cpp utils.h
	$(CPP) -c -g utils.cpp -o utils.o

clean:
	rm -f $(BINS) *.o

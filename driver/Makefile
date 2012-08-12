all: canopen_internal.o canopenmsg.o libcanopen.a

canopen_internal.o: canopen_internal.h canopen_internal.cpp
	g++ -c -fPIC canopen_internal.cpp -o canopen_internal.o -std=c++0x

canopenmsg.o: canopenmsg.h canopenmsg.cpp
	g++ -c -fPIC  canopenmsg.cpp -o canopenmsg.o -std=c++0x 

canopen_highlevel.o: canopen_highlevel.h canopen_highlevel.cpp
	g++ -c -fPIC  canopen_highlevel.cpp -o canopen_highlevel.o -std=c++0x 

libcanopen.a: canopen_internal.o canopenmsg.o canopen_highlevel.o
	ar rv libcanopen.a canopen_internal.o
	ar rv libcanopen.a canopenmsg.o
	ar rv libcanopen.a canopen_highlevel.o

clean:
	\rm *.o *.a *.so *~ 

uninstall:
	rm /usr/lib/libcanopen.a
	rm /usr/include/canopenmsg.h
	rm /usr/include/canopen_internal.h

install:
	cp libcanopen.a /usr/lib
	cp canopen_highlevel.h /usr/include
	cp canopenmsg.h /usr/include
	cp canopen_internal.h /usr/include
	cp indices.csv ~/.canopen/
	cp constants.csv ~/.canopen/
	cp PDOs.csv ~/.canopen/

# cp libcanopenmsg.so /usr/lib
# cp libcanopen_internal.so /usr/lib
# g++ -shared -o libcanopenmsg.so canopenmsg.o -lpcan
# g++ -shared -o libcanopen_internal.so canopen_internal.o -lpcan
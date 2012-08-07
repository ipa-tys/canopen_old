canopenmsg.o:  canopen_internal.h canopenmsg.h canopenmsg.cpp
	g++ -c -fPIC canopenmsg.cpp -o canopenmsg.o -std=c++0x
	g++ -shared -o libcanopenmsg.so canopenmsg.o -lpcan

clean:
	\rm *.o *.so *~ 

install:
	cp libcanopenmsg.so /usr/lib
	cp canopenmsg.h /usr/include
	cp canopen_internal.h /usr/include

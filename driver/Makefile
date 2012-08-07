canopenmsg.o: canopenmsg.cpp canopenmsg.h 
	g++ -c -fPIC canopenmsg.cpp -o canopenmsg.o -std=c++0x
	g++ -shared -o libcanopenmsg.so canopenmsg.o -lpcan

clean:
	\rm *.o *.so *~ 

install:
	cp libcanopenmsg.so /usr/lib
	cp canopenmsg.h /usr/include

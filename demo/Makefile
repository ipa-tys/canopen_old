all: test_min test_min2

test_min: test_min.o
	g++ test_min.o -o test_min -lpthread -lcanopenmsg

test_min.o: test_min.cpp
	g++ -c test_min.cpp -std=c++0x

test_min2: test_min2.o
	g++ test_min2.o -o test_min2 -lpthread -lcanopenmsg

test_min2.o: test_min2.cpp
	g++ -c test_min2.cpp -std=c++0x

clean:
	\rm -f *.o *~ test_min test_min2


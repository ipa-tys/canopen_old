all: single_device multiple_devices

single_device: single_device.o
	g++ single_device.o -o single_device -lpthread -lcanopen -lpcan

single_device.o: single_device.cpp
	g++ -c single_device.cpp -std=c++0x

multiple_devices: multiple_devices.o
	g++ multiple_devices.o -o multiple_devices -lpthread -lcanopen -lpcan

multiple_devices.o: multiple_devices.cpp
	g++ -c multiple_devices.cpp -std=c++0x

test_min: test_min.o
	g++ test_min.o -o test_min -lpthread -lcanopen

test_min.o: test_min.cpp
	g++ -c test_min.cpp -std=c++0x

test_min2: test_min2.o
	g++ test_min2.o -o test_min2 -lpthread -lcanopen -lpcan

test_min2.o: test_min2.cpp
	g++ -c test_min2.cpp -std=c++0x

clean:
	\rm -f *.o *~ single_device multiple_devices


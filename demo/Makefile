all: getpos reference homing single_device multiple_devices canopenmaster_single_device canopenmaster_multiple_devices

getpos: getpos.o
	g++ getpos.o -o getpos -lpthread -lcanopen -lpcan

getpos.o: getpos.cpp
	g++ -c getpos.cpp -std=c++0x

reference: reference.o
	g++ reference.o -o reference -lpthread -lcanopen -lpcan

reference.o: reference.cpp
	g++ -c reference.cpp -std=c++0x

homing: homing.o
	g++ homing.o -o homing -lpthread -lcanopen -lpcan

homing.o: homing.cpp
	g++ -c homing.cpp -std=c++0x

single_device: single_device.o
	g++ single_device.o -o single_device -lpthread -lcanopen -lpcan

single_device.o: single_device.cpp
	g++ -c single_device.cpp -std=c++0x

multiple_devices: multiple_devices.o
	g++ multiple_devices.o -o multiple_devices -lpthread -lcanopen -lpcan

multiple_devices.o: multiple_devices.cpp
	g++ -c multiple_devices.cpp -std=c++0x

canopenmaster_single_device: canopenmaster_single_device.o
	g++ canopenmaster_single_device.o -o canopenmaster_single_device -lpthread -lcanopen -lpcan -lyaml-cpp

canopenmaster_single_device.o: canopenmaster_single_device.cpp
	g++ -c canopenmaster_single_device.cpp -std=c++0x

canopenmaster_multiple_devices: canopenmaster_multiple_devices.o
	g++ canopenmaster_multiple_devices.o -o canopenmaster_multiple_devices -lpthread -lcanopen -lpcan -lyaml-cpp

canopenmaster_multiple_devices.o: canopenmaster_multiple_devices.cpp
	g++ -c canopenmaster_multiple_devices.cpp -std=c++0x

clean:
	\rm -f *.o *~ getpos reference homing single_device multiple_devices canopenmaster_single_device canopenmaster_multiple_devices 


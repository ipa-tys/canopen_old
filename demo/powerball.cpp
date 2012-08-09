// simple demo of how to use the canopen_highlevel functionality
// with a chain of CANopen devices (tested with Schunk powerball arm).
// First, the drive is referenced (homing), then some example
// motions are performed in interpolated_position_mode

#include <thread>
#include <chrono>
#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Please call the program with the CAN deviceIDs,"
	      << "e.g. 'test_script 1 2 3 4 5 6'" << std::endl;
    return -1;
  }
  uint16_t deviceID = std::stoi(std::string(argv[1])); // todo

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
  
  // initialize listener thread:
  canopen::initListenerThread();    
    
  // put NMT and 402 state machines for device  to operational:
  if (!canopen::initDevice(deviceID)) {  // todo
    std::cout << "Device could not be initialized; aborting." << std::endl;
    return -1;
  } 

  // performing homing of device:
   if (!canopen::homing(deviceID)) {
    std::cout << "Homing was not successful; aborting." << std::endl;
    return -1;
    }  


  // put device into interpolated_position_mode:
  if (!canopen::enableIPmode(deviceID)) {
    std::cout << "Could not switch device into IP mode; aborting."
	      << std::endl;
    return -1;
  }

  // move a bit in IP mode:
  canopen::sendSync(10);
  uint32_t pos = 0;
  for (int i=0; i<100; i++) {
    canopen::sendPos(pos);
    pos += 100;
    canopen::sendSync(10);
  }
  for (int i=100; i>0; i--) {
    canopen::sendPos(pos);
    pos -= 100;
    canopen::sendSync(10);
  }



  /*  
  // performing homing of device:
  if (!canopen::homing(deviceID)) {
    std::cout << "Homing was not successful; aborting." << std::endl;
    return -1;
  } 

  // put device into interpolated_position_mode:
  if (!canopen::enableIPmode(deviceID)) {
    std::cout << "Could not switch device into IP mode; aborting."
	      << std::endl;
    return -1;
  }

  // move a bit in IP mode:
  canopen::sendSync(10);
  uint32_t pos = 0;
  for (int i=0; i<1000; i++) {
    canopen::sendPos(pos);
    pos += 100;
    canopen::sendSync(10);
  }
  for (int i=1000; i>0; i--) {
    canopen::sendPos(pos);
    pos -= 100;
    canopen::sendSync(10);
  }
  */

  // std::cout << "end" << std::endl;
  // while (true) {}

  // shutdown device and connection:
  canopen::enableBreak(deviceID);
  canopen::shutdownDevice(deviceID);
  canopen::closeConnection(); 
  return 0;
}

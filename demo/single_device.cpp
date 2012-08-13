// simple demo of how to use the canopen_highlevel functionality
// with a single CANopen motor (tested with Schunk PRH module)
// first, the drive is referenced (homing), then some example
// motions are performed in interpolated_position_mode

#include <thread>
#include <chrono>
#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Please call the program with the CAN deviceID,"
	      << "e.g. './single_device 12'" << std::endl;
    return -1;
  }
  uint16_t deviceID = std::stoi(std::string(argv[1]));

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
 
  // initialize listener thread:
  canopen::initListenerThread();    

  // put NMT and 402 state machines for device  to operational:
  std::cout << "hi1" << std::endl;
  canopen::faultReset(deviceID);
  std::cout << "hi2" << std::endl;

  canopen::initNMT();
  std::cout << "hi3" << std::endl;

  if (!canopen::initDevice(deviceID)) { 
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
  canopen::sendSync(10);
  int pos = 0;
  for (int j=0; j<2; j++) {
    for (int i=0; i<500; i++) {
      canopen::sendPos(deviceID, pos);
      pos += 100;
      canopen::sendSync(10);
    }
    for (int i=500; i>0; i--) {
      canopen::sendPos(deviceID, pos);
      pos -= 100;
      canopen::sendSync(10);
    }
  }

  // shutdown device and connection:
  canopen::enableBreak(deviceID);
  canopen::shutdownDevice(deviceID);
  canopen::closeConnection(); 
  return 0;
}

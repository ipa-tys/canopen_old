// simple demo of how to use the canopen_highlevel functionality
// with a chain of CANopen devices (tested with Schunk powerball arm).
// First, the drive is referenced (homing), then some example
// motions are performed in interpolated_position_mode

#include <thread>
#include <chrono>
#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Please call the program with the CAN deviceIDs,"
	      << "e.g. './multiple_devices 1 2 3 4 5 6'" << std::endl;
    return -1;
  }
  
  std::vector<uint16_t> deviceIDs;
  for (int i=1; i<argc; i++) deviceIDs.push_back(std::stoi(std::string(argv[i])));

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
  
  // initialize listener thread:
  canopen::initListenerThread();    
    
  // put NMT and 402 state machines for devices  to operational:
  for (auto it : deviceIDs) canopen::faultReset(it);
  canopen::initNMT();

  for (auto it : deviceIDs) 
    if (!canopen::initDevice(it)) {
      std::cout << "Device " << it << " could not be initialized; aborting." << std::endl;
      return -1;
    } 
  
  // performing homing of devices:
  for (auto it : deviceIDs)
    if (!canopen::homing(it)) {
      std::cout << "Homing was not successful; aborting." << std::endl;
      return -1;
    }  

  // put devices into interpolated_position_mode:
  for (auto it : deviceIDs)
    if (!canopen::enableIPmode(it)) {
      std::cout << "Could not switch device into IP mode; aborting."
		<< std::endl;
      return -1;
    }
  
  // move a bit in IP mode:
  canopen::sendSync(10);
  uint32_t pos = 0;
  for (int i=0; i<250; i++) {
    canopen::sendPos7(pos);
    canopen::sendPos8(pos);
    pos += 100;
    canopen::sendSync(10);
  }
  for (int i=250; i>0; i--) {
    canopen::sendPos7(pos);
    canopen::sendPos8(pos);
    pos -= 100;
    canopen::sendSync(10);
  }
  
  // shutdown devices and connection:
  for (auto it : deviceIDs) {
    canopen::enableBreak(it);
    canopen::shutdownDevice(it);
  }
  canopen::closeConnection(); 
  return 0;
}

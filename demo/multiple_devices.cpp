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
	      << "e.g. './multiple_devices 3 4 5 6 7 8'" << std::endl;
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
  for (auto it : deviceIDs) {
    std::cout << "homing device: " << it << std::endl;
    if (!canopen::homing(it)) {
      std::cout << "Homing was not successful; aborting." << std::endl;
      return -1;
    }  
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
  canopen::sendSync(10);
  int pos = 0;
  for (int j=0; j<10; j++) {
    for (int i=0; i<=300; i++) {
      for (auto it : deviceIDs) canopen::sendPos(it, pos);
      pos -= 35;
      canopen::sendSync(10);
    }
    for (int i=300; i>0; i--) {
      for (auto it : deviceIDs) canopen::sendPos(it, pos);
      pos -= 35;
      canopen::sendSync(10);
    }   
  }
  
  return -1;

  // shutdown devices and connection:
  for (auto it : deviceIDs) {
    canopen::enableBreak(it);
    canopen::shutdownDevice(it);
  }
  canopen::closeConnection(); 
  return 0;
}

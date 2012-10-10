// Copyright (c) 2012 Fraunhofer Institute
// for Manufacturing Engineering and Automation (IPA)
// See the file license.txt for copying permission.

// This program performs homing (referencing) of a device.
// See the user manual for details:
// https://github.com/ipa-tys/canopen/blob/master/doc/usermanual.pdf?raw=true

#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Arguments:" << std::endl
      	      << "(1) device file" << std::endl
	      << "(2) CAN deviceID" << std::endl
	      << "e.g. './homing /dev/pcan32 12'" << std::endl;
    return -1;
  }
  std::string deviceFile = std::string(argv[1]);
  uint16_t deviceID = std::stoi(std::string(argv[2]));

  // initialize CAN device driver:
  if (!canopen::openConnection(deviceFile)) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
    
  // initialize listener thread:
  canopen::initListenerThread();    
  canopen::initNMT();
  canopen::setMotorState(deviceID, "operation_enable");
  canopen::homing(deviceID);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  canopen::closeConnection();
}

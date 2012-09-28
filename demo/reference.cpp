#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Please call the program with the CAN deviceID"
	      << "and 1 (right) or -1 (left)"
	      << "e.g. './single_device 12 1'" << std::endl;
    return -1;
  }
  uint16_t deviceID = std::stoi(std::string(argv[1]));
  int direction = std::stoi(std::string(argv[2]));

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 

  canopen::initListenerThread();    
  canopen::initNMT();
  // canopen::setSyncInterval(deviceID, std::chrono::milliseconds(10));
  
  if (!canopen::initDevice(deviceID, std::chrono::milliseconds(10))) { 
  // if (!canopen::setMotorStateMachine(deviceID, "switched_on")) {
    std::cout << "Device could not be initialized; aborting." << std::endl;
    return -1;
  } 

  canopen::homingUntilUserInterrupt(deviceID, direction);
  // canopen::homing(deviceID);

  return 0;
}

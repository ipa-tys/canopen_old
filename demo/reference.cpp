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
 
  // initialize listener thread:
  canopen::initListenerThread();    

  // put NMT and 402 state machines for device  to operational:
  canopen::faultReset(deviceID);
  canopen::initNMT();
  if (!canopen::initDevice(deviceID)) { 
    std::cout << "Device could not be initialized; aborting." << std::endl;
    return -1;
  } 

  canopen::moveUntilUserInterrupt(deviceID, direction);

  // canopen::homing(deviceID);
  // std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

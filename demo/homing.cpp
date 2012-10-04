#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Please call the program with the CAN deviceID"
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
  canopen::initNMT();
  canopen::setSyncInterval(deviceID, std::chrono::milliseconds(10));
  canopen::setMotorState(deviceID, "operation_enable");
  canopen::homing(deviceID);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

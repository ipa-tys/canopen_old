#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cout
      << "Arguments:" << std::endl
      << "(1) CAN deviceID" << std::endl
      << "(2) SYNC interval in milliseconds" << std::endl
      << "(3) positive (right) or negative (left) speed factor" << std::endl
      << "Example: ./interactiveHoming 12 10 1" << std::endl;
    return -1;
  }
  uint16_t deviceID = std::stoi(std::string(argv[1]));
  std::chrono::milliseconds syncInterval(std::stoi(std::string(argv[2])));
  int speedFactor = std::stoi(std::string(argv[3]));

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 

  canopen::initListenerThread();    
  canopen::initNMT();
  canopen::setSyncInterval(deviceID, syncInterval);
  canopen::setMotorState(deviceID, "operation_enable");
  canopen::interactiveHoming(deviceID, speedFactor, syncInterval);

  return 0;
}

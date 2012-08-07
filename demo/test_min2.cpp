#include <thread>
#include <chrono>
#include <canopenmsg.h>

int main() {
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  }
  
  canopen::initListener();    // initialize listener thread
  
  if (!canopen::initDevice(12)) {
    std::cout << "Device could not be initialized; aborting." << std::endl;
    return -1;
  }
  
  // todo: homing
  // canopen::modesOfOperation("homing_mode");
  // canopen::controlWord("start_homing|enable_ip_mode");
  /*
  SDOreply = canopen::statusWord(12);

  if (SDOreply->checkForConstant("not_ready_to_switch_on")) {
    std::cout << "not_ready_to_switch_on" << std::endl;
  } else {
    std::cout << "other state" << std::endl;
  }

  bool deviceStatus = canopen::initDevice(12); // todo: check why this does not work yet
  std::cout << "deviceStatus: " << deviceStatus << std::endl;
  */
  
  while (true) {}
  canopen::closeConnection(); 

  return 0;
}

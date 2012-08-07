#include <thread>
#include <chrono>
#include <canopenmsg.h>

int main() {
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device" << std::endl;
    return -1;
  }
  canopen::Message* SDOreply; // SDO replies will be stored here
  canopen::initListener();    // initialize listener thread

  SDOreply = canopen::statusWord(12);

  if (SDOreply->checkForConstant("not_ready_to_switch_on")) {
    std::cout << "not_ready_to_switch_on" << std::endl;
  } else {
    std::cout << "other state" << std::endl;
  }

  bool deviceStatus = canopen::initDevice(12); // todo: check why this does not work yet
  std::cout << "deviceStatus: " << deviceStatus << std::endl;

  return 0;
  
  while (true) {}
  canopen::closeConnection(); 

  return 0;
}

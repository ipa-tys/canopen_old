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

  /*canopen::controlWord(12, "sm_shutdown");
  SDOreply = canopen::statusWord(12);
  SDOreply->debugPrint();

  canopen::controlWord(12, "sm_switch_on");
  SDOreply = canopen::statusWord(12);
  SDOreply->debugPrint();
  
  canopen::controlWord(12, "sm_enable_operation");
  SDOreply = canopen::statusWord(12);
  SDOreply->debugPrint();

  // bool state = SDOreply->checkForConstant("operation_enable");
  bool state = SDOreply->checkForConstant("sm_switch_on");
  std::cout << state << std::endl; */

  bool initOk = canopen::initDevice(12);
  std::cout << "hi: " << initOk << std::endl;

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

#include <iostream>
#include <fstream>
#include <pwd.h>
#include <queue>
#include <canopenmaster.h>

int main() {

  std::cout << "ho" << std::endl;
  canopen::using_master_thread = true;
  canopen::initChainMap("/home/tys/git/other/canopen/driver/robot1.csv");

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 

  // initialize listener thread:
  canopen::initListenerThread();

  std::cout << "outgoing queue size: " << canopen::outgoingMsgQueue.size() << std::endl;
  // canopen::sendNMT("stop_remote_node"); ok!
  canopen::initNMT(); // ok!

  canopen::Message m(12, "statusword");
  m.writeCAN();

  std::cout << "outgoing queue size: " << canopen::outgoingMsgQueue.size() << std::endl;
  // canopen::initCallback("chain1");
 
  canopen::initMasterThread();
    
  while (true) {}
  return 0;
}

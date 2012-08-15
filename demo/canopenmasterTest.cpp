#include <iostream>
#include <fstream>
#include <pwd.h>
#include <queue>
#include <canopenmaster.h>

void clientFunc() {
  // std::this_thread::sleep_for(std::chrono::milliseconds(500));
  
  std::cout << "client thread: outgoing queue size: " << canopen::outgoingMsgQueue.size() << std::endl;
  // canopen::sendNMT("stop_remote_node"); ok!
  canopen::initNMT(); // ok!

  std::cout << "AAA" << std::endl;
  canopen::Message* reply = canopen::sendSDO(12, "statusword", "", false);
  std::cout << "BBB" << std::endl;
  std::cout << "Reply:" << std::endl;
  reply->debugPrint();
  std::cout << "status check: " << reply->checkForConstant("switch_on_disabled") << std::endl;


  std::cout << "client thread: reply received!!" << std::endl;
  std::cout << "client thread: outgoing queue size: " << canopen::outgoingMsgQueue.size() << std::endl;
  
  // canopen::initCallback("chain1");

  while (true) {}
}


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

  canopen::initMasterThread();


  std::thread client_thread(clientFunc);
  client_thread.detach();

 

    
  while (true) {}
  return 0;
}

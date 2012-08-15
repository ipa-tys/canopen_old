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

  /*  std::cout << "AAA" << std::endl;
  canopen::Message* reply = canopen::sendSDO(12, "statusword");
  std::cout << "BBB" << std::endl;
  std::cout << "Reply:" << std::endl;
  reply->debugPrint();
  std::cout << "status check: " << reply->checkForConstant("switch_on_disabled") << std::endl; */

  if (!canopen::initDevice(12)) { 
    std::cout << "Device could not be initialized!" << std::endl;
  } 
 
  // performing homing of device:
  if (!canopen::homing(12)) {
    std::cout << "Homing was not successful!" << std::endl;
  } 

  if (!canopen::enableIPmode(12)) {
    std::cout << "Could not switch device into IP mode; aborting."
	      << std::endl;
  }


  // canopen::initDevice(12);
  // canopen::homing(12);


  std::cout << "client thread: reply received!!" << std::endl;
  std::cout << "client thread: outgoing queue size: " << canopen::outgoingMsgQueue.size() << std::endl;

  std::this_thread::sleep_for(std::chrono::seconds(4));

  std::cout << "NOW STARTING PDO SENDING!" << std::endl;
    
  uint32_t pos = 0;
  for (int i=0; i<500; i++) {
    std::cout << "PDO1" << std::endl;
    std::vector<int> mypos;
    mypos.push_back(pos);
    canopen::setPosCallback("chain1", mypos);
    std::cout << "PDO2" << std::endl;
    pos += 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  
  // canopen::initCallback("chain1");

  while (true) {}
}


int main() {

  std::cout << "ho" << std::endl;
  canopen::using_master_thread = true;
  canopen::initChainMap("/home/tys/git/other/canopen/driver/robot1.csv");

  /* for (auto pp : canopen::pdo.d_) {
    std::cout << pp.alias_ << std::endl;
    for (auto it : pp.components_)
  }

  return 0; */

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 

  // initialize listener thread:
  canopen::initListenerThread();
  canopen::initIncomingPDOProcessorThread();
  canopen::initMasterThread();


  std::thread client_thread(clientFunc);
  client_thread.detach();

 

    
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return 0;
}

#include <iostream>
#include <fstream>
#include <pwd.h>
#include <queue>
#include <cmath>
#include <canopenmaster.h>
#include "yaml-cpp/yaml.h"

// this shows a demo of how a client can communicate with the master by
// invoking specific callback functions. Here, this is done within the same
// process from a separate thread. The corresponding ROS demos show the same
// approach but using ROS interprocess communication via services and
// publishers/subscribers.

void clientFunc() { 
  canopen::initCallback("arm1");
  canopen::homingCallback("arm1");
  canopen::IPmodeCallback("arm1");

  for (int i=0; i<8; i++) {
    std::cout << "..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::vector<int32_t> current_pos = canopen::getCurrentPosCallback("arm1");
  std::vector<int32_t> request_pos = current_pos;
  // std::vector<int32_t> request_pos = {0,0,0,0,0,0};

  std::vector<int> incr = {20,20,20,20,20,20};

  for (int i=0; i<=10000; i++) {

    current_pos = canopen::getCurrentPosCallback("arm1");
    // std::cout << incr << std::endl;
    
    for (int k=0; k<current_pos.size(); k++) {
      if  (current_pos[k] <= -300 || current_pos[k] > 7000) {
	incr[k] = 20;
      } else  {
	incr[k] = -20;
      } 

      if (abs(request_pos[k] - current_pos[k]) > 20000) {
	request_pos[k] = current_pos[k] - incr[k]/2;
      } else if (abs(request_pos[k] - current_pos[k]) > 200) {
	request_pos[k] = request_pos[k] + incr[k]/2;
      } else {
	request_pos[k] = request_pos[k] + incr[k];
      }
    }

    std::cout << "CURRENT POS: ";
    for (auto it : current_pos) 
      std::cout << it << "  ";
    std::cout << std::endl;
    
    std::cout << "REQUEST POS: ";	
    for (int k=0; k<request_pos.size(); k++) {
      std::cout << request_pos[k] << "  ";
    }
    std::cout << std::endl;

    std::cout << "INCR: ";
    for (auto it : incr) 
      std::cout << it << "  ";
    std::cout << std::endl;

    canopen::setPosCallback("arm1", request_pos);
    // this should match the controller_cycle_duration and in practice would be
    // the feedback loop, cf. ROS demos
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  

  while (true) {}
}

int main() {
  canopen::using_master_thread = true; // todo: can get rid of this I think!
  // canopen::initChainMap("/home/tys/git/other/canopen/demo/multiple_devices.csv");
  
  auto chainDesc = canopen::parseChainDescription("multiple_devices.yaml");
  canopen::initChainMap(chainDesc);

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
  
  canopen::initNMT();
  canopen::initListenerThread();
  canopen::initIncomingPDOProcessorThread();
  
  canopen::initMasterThread();

  // std::cout << "hi" << std::endl;
  

  // client_thread simulates callback invocations from a client:
  std::thread client_thread(clientFunc);
  client_thread.detach();
    
  while (true)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  return 0;
}

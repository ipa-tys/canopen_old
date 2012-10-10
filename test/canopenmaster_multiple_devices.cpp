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

void printPositions(std::vector<double> actualPos, std::vector<double> desiredPos) {
  std::cout << "Actual: ";
  for (auto it : actualPos)
    std::cout << it << "  ";
  std::cout << std::endl;

  std::cout << "Desired: ";
  for (auto it : desiredPos)
    std::cout << it << "  ";
  std::cout << std::endl;
}

void clientFunc() { 
  canopen::initCallback("arm1");
  // canopen::homingCallback("arm1");
  canopen::IPmodeCallback("arm1");

  for (int i=0; i<8; i++) {
    std::cout << "..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::vector<double> actualPos = canopen::getActualPosCallback("arm1");
  std::vector<double> desiredPos = actualPos;

  double step_size = 2 * M_PI / 8000.0;

  //  for (int j=0; j<10; j++) {
    for (int i=0; i<=500; i++) {
      actualPos = canopen::getActualPosCallback("arm1");
      canopen::setPosCallback("arm1", desiredPos);
      printPositions(actualPos, desiredPos);
      desiredPos[0] += step_size;
      for (int k=1; k<desiredPos.size(); k++)
	desiredPos[k] = actualPos[k];
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    /* for (int i=100; i>0; i--) {
      canopen::setPosCallback("arm1", desiredPos);
      // for (int k=0; k<desiredPos.size(); k++)
	desiredPos[0] -= step_size;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }  */ 
    // }

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

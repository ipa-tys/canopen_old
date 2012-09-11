#include <iostream>
#include <fstream>
#include <pwd.h>
#include <queue>
#include <canopenmaster.h>
#include "yaml-cpp/yaml.h"

// this shows a demo of how a client can communicate with the master by
// invoking specific callback functions. Here, this is done within the same
// process from a separate thread. The corresponding ROS demos show the same
// approach but using ROS interprocess communication via services and
// publishers/subscribers.

void clientFunc() { 
  canopen::initCallback("chain1");
  // canopen::homingCallback("chain1");
  canopen::IPmodeCallback("chain1");
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
  // std::vector<double> positions = canopen::getActualPosCallback("chain1");
  
  double step_size = 2 * M_PI / 2000.0;
  std::vector<double> actualPos;
  std::vector<double> positions;

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // positions = canopen::chainMap["chain1"]->getActualPos();
  positions = canopen::getActualPosCallback("chain1");

  for (int i=0; i<2000; i++) {
    canopen::setPosCallback("chain1", positions);
    positions[0] += step_size;
    actualPos = canopen::getActualPosCallback("chain1");

    std::cout << "Positions: ";
    for (auto it : positions)
      std::cout << it << "  ";
    std::cout << std::endl;
    
    // this should match the controller_cycle_duration and in practice would be
    // the feedback loop, cf. ROS demos
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } 

  std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    for (int i=0; i<2000; i++) {
    canopen::setPosCallback("chain1", positions);
    positions[0] -= step_size;
    actualPos = canopen::getActualPosCallback("chain1");

    std::cout << "Positions: ";
    for (auto it : positions)
      std::cout << it << "  ";
    std::cout << std::endl;
    
    // this should match the controller_cycle_duration and in practice would be
    // the feedback loop, cf. ROS demos
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } 

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

  // std::this_thread::sleep_for(std::chrono::seconds(1));
  // canopen::homingCallback("chain1");
  while (true) {}
}

int main() {
  canopen::using_master_thread = true;
  // todo: canopen::parseChainDesc(filename);
  // canopen::initChainMap("/home/tys/git/other/canopen/demo/single_device.csv");

  auto chainDesc = canopen::parseChainDescription("single_device.yaml");
  canopen::initChainMap(chainDesc);

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
  canopen::initNMT();
  canopen::initListenerThread();
  std::cout << "1 - hi" << std::endl;
  canopen::initIncomingPDOProcessorThread();
  std::cout << "2 - hi" << std::endl;
  canopen::initMasterThread();
  std::cout << "3 - hi" << std::endl;

  // client_thread simulates callback invocations from a client:
  std::thread client_thread(clientFunc);
  client_thread.detach();
    
  while (true)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  return 0;
}

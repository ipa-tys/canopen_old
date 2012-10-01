#include <iostream>
#include <fstream>
#include <pwd.h>
#include <queue>
#include <canopenmaster.h>
#include "yaml-cpp/yaml.h"

void clientFunc() { 

  double step_size = 2 * M_PI / 2000.0;
  // std::vector<double> actualPos;
  // std::vector<double> positions;

  // positions = canopen::chainMap["chain1"]->getActualPos();
  // positions = canopen::getActualPosCallback("chain1");

  canopen::chainMap["chain1"]->setVel({0.1});
  std::this_thread::sleep_for(std::chrono::seconds(4));
  canopen::chainMap["chain1"]->setVel({-0.1});
  std::this_thread::sleep_for(std::chrono::seconds(4));
  canopen::chainMap["chain1"]->setVel({0.0});
  std::this_thread::sleep_for(std::chrono::seconds(1));
  canopen::chainMap["chain1"]->setVel({-0.2});
  std::this_thread::sleep_for(std::chrono::seconds(2));
  canopen::chainMap["chain1"]->setVel({0.2});
  std::this_thread::sleep_for(std::chrono::seconds(2));
  canopen::chainMap["chain1"]->setVel({0.0});
  
    // canopen::ChainState cs = canopen::chainMap["chain1"]->getChainState();
    // cs.print();
    // std::this_thread::sleep_for(std::chrono::seconds(1));
  

  /* for (int i=0; i<2000; i++) {
    canopen::setPosCallback("chain1", positions);
    positions[0] += step_size;
    actualPos = canopen::getActualPosCallback("chain1");

    std::cout << "Positions: ";
    for (auto it : positions)
      std::cout << it << "  ";
    std::cout << std::endl;
    
    // this should match the controller_cycle_duration and in practice would be
    // the feedback loop, cf. ROS demos
    std::this_thread::sleep_for(canopen::sync_deltaT_msec);
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
    std::this_thread::sleep_for(canopen::sync_deltaT_msec);
    } 

    std::this_thread::sleep_for(std::chrono::milliseconds(300)); */

  // std::this_thread::sleep_for(std::chrono::seconds(1));
  // canopen::homingCallback("chain1");
  while (true) {}
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cout << "sync interval [msec] must be given as argument!" << std::endl;
    return -1;
  }

  canopen::using_master_thread = true;
  canopen::syncInterval = std::chrono::milliseconds(std::stoi(std::string(argv[1])));
  auto chainDesc = canopen::parseChainDescription("single_device.yaml");
  canopen::initChainMap(chainDesc);

  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
  canopen::initListenerThread();
  canopen::initIncomingPDOProcessorThread();
  canopen::initMasterThread();
  canopen::initNMT();
  for (auto it : canopen::chainMap) 
    it.second->CANopenInit();

  // client_thread simulates callback invocations from a client:
  std::thread client_thread(clientFunc);
  client_thread.detach();
    
  while (true)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  return 0;
}

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

  // canopen::initCallback("chain1", canopen::sync_deltaT_msec);
  // canopen::homingCallback("chain1");
  // canopen::IPmodeCallback("chain1");
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
  // std::vector<double> positions = canopen::getActualPosCallback("chain1");
  
  double step_size = 2 * M_PI / 1000.0;
  std::vector<double> actualPos;
  std::vector<double> positions;

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // positions = canopen::chainMap["chain1"]->getActualPos();
  positions = canopen::getActualPosCallback("chain1");

  for (int i=0; i<250; i++) {
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

  std::cout << "final command" << std::endl;

  //std::this_thread::sleep_for(std::chrono::milliseconds(4000));

  for (int i=0; i<250; i++) {
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

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

  // std::this_thread::sleep_for(std::chrono::seconds(1));
  // canopen::homingCallback("chain1");
  while (true) {}
}

int main(int argc, char *argv[]) {

  canopen::using_master_thread = true; // todo: hack

  if (argc != 2) {
    std::cout << "sync rate must be given as argument!" << std::endl;
    return -1;
  }
  
  int sync_deltaT_msec_int = std::stoi(std::string(argv[1]));
  canopen::sync_deltaT_msec = std::chrono::milliseconds(sync_deltaT_msec_int);
  std::cout << "sync rate: " << sync_deltaT_msec_int << std::endl;
  // todo: canopen::parseChainDesc(filename);
  // canopen::initChainMap("/home/tys/git/other/canopen/demo/single_device.csv");

  auto chainDesc = canopen::parseChainDescription("single_device.yaml");
  canopen::initChainMap(chainDesc);

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 

  std::cout << "test chain map!" << std::endl;
  for (auto it : canopen::chainMap) {
    std::cout << it.second->alias_ << std::endl;
    std::cout << "Devices:" << std::endl;
    for (auto it1: it.second->devices_) {
      std::cout << it1.CANid_ << std::endl;
    }
  }

  std::cout << std::endl;


  canopen::initListenerThread();
  std::cout << "0 - hi" << std::endl;

  canopen::initMasterThread();
  std::cout << "3 - hi" << std::endl;


  canopen::faultReset(10); // hack
  canopen::initNMT();
  std::cout << "0.5 - hi" << std::endl;
  canopen::initDevice(10, canopen::sync_deltaT_msec); // hack
  std::cout << "0.6 - hi" << std::endl;
  
  if (!canopen::enableIPmode(10)) { // hack
    std::cout << "Could not switch device into IP mode; aborting."
	      << std::endl;
    return -1;
  }

  std::cout << "0.7 - hi" << std::endl;


  
  std::cout << "1 - hi" << std::endl;
  // canopen::initIncomingPDOProcessorThread();
  std::cout << "2 - hi" << std::endl;
  /*
  // client_thread simulates callback invocations from a client:
  std::thread client_thread(clientFunc);
  client_thread.detach(); */
    
  while (true) {
    std::cout << "PDO size: " << std::dec << canopen::incomingPDOs.size() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return 0;
}

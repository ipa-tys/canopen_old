#include <iostream>
#include <fstream>
#include <pwd.h>
#include <queue>
#include <canopenmaster.h>

// this shows a demo of how a client can communicate with the master by
// invoking specific callback functions. Here, this is done within the same
// process from a separate thread. The corresponding ROS demos show the same
// approach but using ROS interprocess communication via services and
// publishers/subscribers.

void clientFunc() { 
  canopen::initCallback("chain1");
  canopen::homingCallback("chain1");
  canopen::IPmodeCallback("chain1");
  std::this_thread::sleep_for(std::chrono::seconds(4));
    
  uint32_t pos = 0;
  for (int i=0; i<500; i++) {
    canopen::setPosCallback("chain1", {pos});
    pos += 100;
    // this should match the controller_cycle_duration and in practice would be
    // the feedback loop, cf. ROS demos
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  while (true) {}
}

int main() {
  canopen::using_master_thread = true;
  canopen::initChainMap("/home/tys/git/other/canopen/demo/single_device.csv");

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
  canopen::initNMT();
  canopen::initListenerThread();
  canopen::initIncomingPDOProcessorThread();
  canopen::initMasterThread();

  // client_thread simulates callback invocations from a client:
  std::thread client_thread(clientFunc);
  client_thread.detach();
    
  while (true)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  return 0;
}

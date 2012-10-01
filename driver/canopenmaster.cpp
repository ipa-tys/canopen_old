#include <queue>
#include <map>
#include <inttypes.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "canopenmaster.h"

namespace canopen {
 
  void masterFunc() {
    auto tic = std::chrono::high_resolution_clock::now();
    sendSync(syncInterval); // todo: check if this pre-sync is necessary
    sendSync(syncInterval);

    while (true) {
      tic = std::chrono::high_resolution_clock::now();
      sendSync();

      // PDOs are sent to all chains every SYNC cycle:
      for (auto chain : chainMap)
	chain.second->sendPos();

      // SDOs are only sent in the remaining time of a SYNC cycle:
      while (std::chrono::high_resolution_clock::now() < tic + syncInterval) {
	// fetch a message from the outgoingMsgQueue and send to CAN bus:
	if (outgoingMsgQueue.size() > 0) {
	  outgoingMsgQueue.front().writeCAN(true); 
	  outgoingMsgQueue.pop();
	}
	std::this_thread::sleep_for(std::chrono::microseconds(1)); 
      }
    }
  }

  void initMasterThread() {
    using_master_thread = true;
    std::thread master_thread(masterFunc);
    master_thread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  void incomingPDOProcessorFunc() {
    // from chainMap build a map id2chain:
    std::map<uint16_t, std::string> id2chain;
    for (auto chain : chainMap)
      for (auto device : chain.second->devices_)
	id2chain[device->CANid_] = chain.second->alias_;

    std::cout << "MAP: " << std::endl;
    for (auto it : id2chain)
      std::cout << it.first << " ---> " << it.second << std::endl;
    
    while (true) {
      if (incomingPDOs.size()>1) { // todo: change to ">0" as soon as queue is thread-safe
	Message m = incomingPDOs.front();
	incomingPDOs.pop();
	chainMap[ id2chain[m.nodeID_] ]->update(m);
	std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
      }
    }
  }

  void initIncomingPDOProcessorThread() {
    std::thread incomingPDOProcessor_thread(incomingPDOProcessorFunc);
    incomingPDOProcessor_thread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

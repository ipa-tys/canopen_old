#include <queue>
#include <map>
#include <inttypes.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "canopenmaster.h"

namespace canopen {

  std::chrono::milliseconds sync_deltaT_msec;
  std::map<std::string, Chain*> chainMap;

  void initChainMap(std::vector<ChainDescription> chainDesc) {
    for (auto c : chainDesc) {
      std::cout << "init chain: " << c.name << std::endl;
      chainMap[c.name] = new Chain(c, sync_deltaT_msec);
    }
  }

  void masterFunc() {
    
    // send out to CAN bus at specified rate:
    auto tic = std::chrono::high_resolution_clock::now();
    bool any_SendPosActive;
    bool noSyncYet = true;

    std::cout << "anysendposactive? " << any_SendPosActive << std::endl;

    while (true) {
      tic = std::chrono::high_resolution_clock::now();


      // std::cout << "SYNC active? " << any_SendPosActive << std::endl;

      // any_SendPosActive = false;
      any_SendPosActive = true;
      for (auto chain : chainMap) 
	if (chain.second->sendPosActive_)
	  any_SendPosActive = true;

      if (any_SendPosActive && noSyncYet) {
	noSyncYet = false;
	for (int i=0; i<2; i++) {
	  sendSync();
	  std::this_thread::sleep_for(sync_deltaT_msec);
	}
      }

      // std::cout << "hi A" << std::endl;

      // send PDOs to CAN bus (if chain has sendPosActive_==true):
      for (auto chain : chainMap) 
	if (chain.second->sendPosActive_) 
	  chain.second->sendPos();
      
      if (any_SendPosActive) {
	std::cout << "SYNC" << std::endl;
	sendSync();
      }

      // std::cout << "hi B" << std::endl;

      while (std::chrono::high_resolution_clock::now() < tic + sync_deltaT_msec) {
	// fetch a message from the outgoingMsgQueue and send to CAN bus:
	if (outgoingMsgQueue.size() > 1) { // todo - check
	  // std::cout << "write" << std::endl;

	  // outgoingMsgQueue.front().writeCAN(true); 

	  std::cout << "write B" << std::endl;
	  outgoingMsgQueue.pop();
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10)); 
      }

      // std::cout << "hi C" << std::endl;

      // std::cout << ((std::chrono::high_resolution_clock::now()-tic)-sync_deltaT_msec).count() << std::endl;
      tic = std::chrono::high_resolution_clock::now();
    }
  }

  void initMasterThread() {
    using_master_thread = true;
    std::thread master_thread(masterFunc);
    master_thread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  void incomingPDOProcessorFunc() {
    std::map<uint16_t, std::string> id2chain;
    for (auto chain : chainMap)
      for (auto device : chain.second->devices_)
	id2chain[device.CANid_] = chain.second->alias_;

    std::cout << "MAP: " << std::endl;
    for (auto it : id2chain)
      std::cout << std::dec << it.first << " ---> " << it.second << std::endl;
    std::cout << "ENDMAP: " << std::endl;
    while (true) {
      std::cout << "incomingPDOprocessor" << std::endl;
      if (incomingPDOs.size()>3) { // todo: change to ">0" as soon as queue is thread-safe
	std::cout << "processing a PDO" << std::endl;
	Message m = incomingPDOs.front();
	std::cout << "processing a PDO1" << std::endl;
	incomingPDOs.pop();
	std::cout << "processing a PDO2" << std::endl;
	std::cout << id2chain[ m.nodeID_] << std::endl;
	std::cout << m.nodeID_ << std::endl;
	chainMap[ id2chain[m.nodeID_] ]->updateStatusWithIncomingPDO(m);
	std::cout << "processing a PDO3" << std::endl;
	// fetching can be arbitrarily fast:
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

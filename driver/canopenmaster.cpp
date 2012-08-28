#include <queue>
#include <map>
#include <inttypes.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "canopenmaster.h"

namespace canopen {
  std::chrono::milliseconds controllerCycleDuration_msec(10);
  std::map<std::string, Chain*> chainMap;

  void homingCallback(std::string chainName) { 
    chainMap[chainName]->chainHoming();
  }
  
  void initCallback(std::string chainName) { 
    chainMap[chainName]->chainInit();
  }

  void IPmodeCallback(std::string chainName) { 
    chainMap[chainName]->chainIPmode();
  }

  void setPosCallback(std::string chainName, std::vector<int> positions) {
    chainMap[chainName]->setPos(positions);
  }

  std::vector<int> getCurrentPosCallback(std::string chainName) {
    return chainMap[chainName]->getCurrentPos();
  }

  void initChainMap(std::string robotDescFilename) {

    // parse description file: // todo: make this more flexible: YAML etc.
    std::ifstream fin(robotDescFilename);
    std::string ll;

    std::vector<std::string> all_chainNames;
    std::vector< std::vector<std::string> > all_deviceNames;
    std::vector< std::vector<std::string> > all_CANbuses;
    std::vector< std::vector<uint16_t> > all_CANids;

    while (std::getline(fin, ll)) {
      std::istringstream x(ll);
      std::string chainName;
      std::vector<std::string> deviceNames;
      std::vector<std::string> CANbuses;
      std::vector<uint16_t> CANids;
      std::string temp;
      x >> chainName;
      while (x >> temp) {
	deviceNames.push_back(temp);
	x >> temp;
	CANbuses.push_back(temp);
	x >> temp;
	CANids.push_back(std::stoi(temp));
      }

      all_chainNames.push_back(chainName);
      all_deviceNames.push_back(deviceNames);
      all_CANbuses.push_back(CANbuses);
      all_CANids.push_back(CANids);
    }

    // construct chainMap:
    for (int i=0; i<all_chainNames.size(); i++) {
      chainMap[all_chainNames[i]] = new Chain(all_chainNames[i], all_deviceNames[i],
					      all_CANbuses[i], all_CANids[i]);
    }
  }

  void masterFunc() {
    
    // send out to CAN bus at specified rate:
    auto tic = std::chrono::high_resolution_clock::now();
    bool any_SendPosActive;
    bool noSyncYet = true;
    while (true) {
      tic = std::chrono::high_resolution_clock::now();

      any_SendPosActive = false;
      for (auto chain : chainMap) 
	if (chain.second->sendPosActive_)
	  any_SendPosActive = true;

      if (any_SendPosActive && noSyncYet) {
	noSyncYet = false;
	for (int i=0; i<2; i++) {
	  sendSync();
	  std::this_thread::sleep_for(controllerCycleDuration_msec);
	}
      }

      // send PDOs to CAN bus (if chain has sendPosActive_==true):
      for (auto chain : chainMap) 
	if (chain.second->sendPosActive_) 
	  chain.second->sendPos();
      
      if (any_SendPosActive) {
	std::cout << "SYNC" << std::endl;
	sendSync();
      }

      while (std::chrono::high_resolution_clock::now() < tic + controllerCycleDuration_msec) {
	// fetch a message from the outgoingMsgQueue and send to CAN bus:
	if (outgoingMsgQueue.size() > 0) {
	  outgoingMsgQueue.front().writeCAN(true); 
	  outgoingMsgQueue.pop();
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10)); 
      }

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
      std::cout << it.first << " ---> " << it.second << std::endl;
    
    while (true) {
      if (incomingPDOs.size()>1) { // todo: change to ">0" as soon as queue is thread-safe
	Message m = incomingPDOs.front();
	incomingPDOs.pop();
	chainMap[ id2chain[m.nodeID_] ]->updateStatusWithIncomingPDO(m);
	// fetching can be arbitrarily fast:
	std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
      }
    }

  }


  void initIncomingPDOProcessorThread() {
    std::thread incomingPDOProcessor_thread(incomingPDOProcessorFunc);
    incomingPDOProcessor_thread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }




}




  
// std::queue<std::vector<uint32_t> > outgoingPosQueue;
// std::queue<std::vector<uint32_t> > incomingPosQueue;





/*
  int main() {
  
  std::vector<uint32_t> x {1,2,3};
  std::vector<uint32_t> y {1,2,3,4};

  outgoingPosQueue.push(x);
  outgoingPosQueue.push(y);


  while (outgoingPosQueue.size() > 0) {
  std::cout << "hi" << std::endl;
  std::vector<uint32_t> z = outgoingPosQueue.front();
  outgoingPosQueue.pop();
  std::cout << z.size() << std::endl;

    

  }

  }
  }
*/

#ifndef CHAIN_H
#define CHAIN_H

#include <iostream>
#include <string>
#include <libpcan.h>
#include <cmath>
#include <map>
#include <queue>
#include <cassert>
#include <regex>
#include "canopen_highlevel.h"
#include "chain_description.h"

#define _USE_MATH_DEFINES

namespace canopen {
  extern std::chrono::milliseconds syncInterval;

  class Device {  // [positions]=rad, [velocities]=rad/sec
  public:

    inline Device(std::string alias, std::string CANbus,
		  uint16_t CANid):
    alias_(alias), CANbus_(CANbus), CANid_(CANid),
      initialized_(false), timeStamp_( std::chrono::microseconds(0) ) {}
    
    void CANopenInit();
    void update(Message m);
    inline void setVel(double vel) {
      desiredVel_ = vel;
      desiredPos_ = desiredPos_ + vel * (syncInterval.count() / 1000.0);
    }

    double actualPos_; // unit = rad
    double desiredPos_; // unit = rad
    double actualVel_; // unit = rad/sec
    double desiredVel_; // unit = rad/sec
    std::chrono::microseconds timeStamp_; 

    std::string alias_;
    std::string CANbus_;
    uint16_t CANid_;
    bool initialized_;
  };

  struct ChainState {
    std::vector<double> actualPos;
    std::vector<double> desiredPos;
    std::vector<double> actualVel;
    std::vector<double> desiredVel;
    bool fault;
  };

  class Chain { // [positions]=rad, [velocities]=rad/sec
  public:
    inline Chain(ChainDescription chainDesc):
    alias_(chainDesc.name), initialized_(false), fault_(false) {
      for (auto d : chainDesc.devices) {
	Device* dev = new Device(d.name, d.bus, d.id);
	deviceMap_[d.id] = dev;
	devices_.push_back(dev);
      }
    }
    
    inline void CANopenInit() {
      for (auto device : devices_)
	device->CANopenInit();  }

    inline void sendPos() {
      if (initialized_ & !fault_)
	for (auto device : devices_)
	  canopen::sendPos(device->CANid_, device->desiredPos_);  }

    inline void setVel(std::vector<double> velocities) {
      for (int i=0; i<velocities.size(); i++)
	devices_[i]->setVel(velocities[i]);  }

    inline ChainState getChainState() {
      ChainState cs;
      for (auto device : devices_) {
	cs.actualPos.push_back(device->actualPos_);
	cs.desiredPos.push_back(device->desiredPos_);
	cs.actualVel.push_back(device->actualVel_);
	cs.desiredVel.push_back(device->desiredVel_);
      }
      cs.fault = fault_;
      return cs;
    }

    inline void update(Message m) {
      deviceMap_[m.nodeID_]->update(m);
      if (!initialized_) {
	bool allInitialized = true;
	for (auto device : deviceMap_)
	  if (! device.second->initialized_)
	    allInitialized = false;
	if (allInitialized) initialized_ = true;
      }
    }

    std::string alias_;
    // two alternative access modes for the devices in a chain:
    std::map<uint16_t, Device*> deviceMap_;
    std::vector<Device*> devices_;
    bool initialized_;
    bool fault_;
  };

  extern std::map<std::string, Chain*> chainMap;

  inline void initChainMap(std::vector<ChainDescription> chainDesc) {
    for (auto c : chainDesc) 
      chainMap[c.name] = new Chain(c);   }
  

}
#endif

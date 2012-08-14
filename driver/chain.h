#ifndef CHAIN_H
#define CHAIN_H

#include <iostream>
#include <string>
#include <libpcan.h>
#include <set>
#include <map>
#include <queue>
#include <regex>
#include <pwd.h>
#include <fstream>
#include "canopen_highlevel.h"

namespace canopen {

  class Device {
  public:

  Device(std::string alias, std::string CANbus, uint16_t CANid):
    alias_(alias), CANbus_(CANbus), CANid_(CANid) {
      // todo: only for testing!!
      // init currentVel to fixed value
    }

    void deviceInit() { initDevice(CANid_); } // could have bool return value
    void deviceHoming() { homing(CANid_); }
    // void pushVel(double vel);
    // void sendVel();
    // void sendSDO();
    // inline bool sendVelQueueEmpty() { return(sendVel_queue_.empty()); }

    // private:
    std::string alias_;
    std::string CANbus_;
    uint16_t CANid_;
    // double latestVelSent_; // most recent velocity sent
    // std::queue<double> sendVel_queue_;
    // std::queue<TPCANMsg> sendSDO_queue_;
  };

  class Chain {
  public:

    Chain(std::string chainName, std::vector<std::string> deviceNames,
	  std::vector<std::string> CANbuses, std::vector<uint16_t> CANids);

    void chainInit();
    void chainHoming();
    // void setPos(std::vector<int> positions);
    // std::vector<int> getPos();

    bool sendPosActive_;
    // private:
    std::string alias_;
    std::vector<Device> devices_;

  };

}

#endif

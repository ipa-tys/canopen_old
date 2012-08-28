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
    void deviceIPmode() { enableIPmode(CANid_); }
    void setPos(int position) { requested_position_ = position; }
    int getCurrentPos() { return current_position_;  }
    int getRequestedPos() { return requested_position_;  }
    void updateStatusWithIncomingPDO(Message m);
    // void pushVel(double vel);
    // void sendVel();
    // void sendSDO();
    // inline bool sendVelQueueEmpty() { return(sendVel_queue_.empty()); }

    // private:
    std::string alias_;
    std::string CANbus_;
    uint16_t CANid_;
    int requested_position_;
    int current_position_;
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
    void chainIPmode();
    std::vector<uint16_t> getDeviceIDs();
    void setPos(std::vector<int> positions);
    void sendPos();
    void updateStatusWithIncomingPDO(Message m);                              // todo: save deviceID lookup by using a map

    std::vector<int> getRequestedPos();
    std::vector<int> getCurrentPos();

    bool sendPosActive_;
                                                                                  // private:
    std::string alias_;
    std::vector<Device> devices_;                                              // todo: this should be a map: int CANid -> device object

  };

}

#endif

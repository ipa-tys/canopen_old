#ifndef CHAIN_H
#define CHAIN_H

#include <iostream>
#include <string>
#include <libpcan.h>
#include <cmath>
#include <set>
#include <map>
#include <queue>
#include <cassert>
#include <regex>
#include <pwd.h>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include "canopen_highlevel.h"

#define _USE_MATH_DEFINES

namespace canopen {
  
  // parsing chain and device descriptions:
  struct DeviceDescription {
    std::string name;
    int id;
    std::string bus;
  };
  struct ChainDescription {
    std::string name;
    std::vector<DeviceDescription> devices;
  };
  void operator>> (const YAML::Node& node, DeviceDescription& d);
  void operator>> (const YAML::Node& node, ChainDescription& c);
  std::vector<ChainDescription> parseChainDescription(std::string filename);

  class Device {
  public:
    
  Device(std::string alias, std::string CANbus, uint16_t CANid, uint32_t sync_deltaT_msec):
    alias_(alias), CANbus_(CANbus), CANid_(CANid), sync_deltaT_msec_(sync_deltaT_msec),
      actualPos_(0), actualVel_(0), desiredPos_(0), desiredVel_(0), 
      timeStamp_( std::chrono::microseconds(0) ) {
    }

    void deviceInit() { initDevice(CANid_); }
    // todo (optional): deviceInit could have bool return value
    void deviceHoming() { homing(CANid_); }
    void deviceIPmode() { enableIPmode(CANid_); }
    
    // Note: in IP mode, a desired velocity determines the desired position
    // and vice versa, because the sync frequency is considered fixed
    // and the device tries to reach a desired position within a
    // given SYNC cycle
    void setPos(double pos);
    void setVel(double vel);

    double getActualPos() { return actualPos_; } 
    double getDesiredPos() { return desiredPos_; } 
    double getActualVel() { return actualVel_; } 
    double getDesiredVel() { return desiredVel_; } 

    void updateStatusWithIncomingPDO(Message m);

    // todo: private:
    void updateActualPosAndVel(double newPos, std::chrono::microseconds newTimeStamp);

    std::string alias_;
    std::string CANbus_;
    uint16_t CANid_;
    uint32_t sync_deltaT_msec_; // time between two SYNCs in msec

    double actualPos_; // unit = rad
    double desiredPos_; // unit = rad
    double actualVel_; // unit = rad/sec
    double desiredVel_; // unit = rad/sec
    // timestamp of latest received CAN message (unit = microsec):
    std::chrono::microseconds timeStamp_; 
  };

  class Chain {
  public:
    Chain(ChainDescription chainDesc, uint32_t sync_deltaT_msec);
    void chainInit();
    void chainHoming();
    void chainIPmode();
    std::vector<uint16_t> getDeviceIDs();
    void sendPos();
    void updateStatusWithIncomingPDO(Message m); 
    // ↑ todo: save deviceID lookup by using a map

    std::vector<double> getDesiredPos();  // [rad]
    std::vector<double> getActualPos();  // [rad]
    std::vector<double> getDesiredVel(); // [rad/sec]
    std::vector<double> getActualVel(); // [rad/sec]
    void setPos(std::vector<double> positions); // [rad]
    void setVel(std::vector<double> velocities); // [rad/sec]
    bool sendPosActive_;
    // private:
    std::string alias_;
    std::vector<Device> devices_; 
    // ↑ todo: this should be a map: int CANid -> device object
  };

}

#endif

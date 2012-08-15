// API functions for CANopen communication

#ifndef CANOPENHIGHLEVEL_H
#define CANOPENHIGHLEVEL

#include "canopenmsg.h"

namespace canopen {
  extern EDSDict eds;
  extern PDODict pdo;

  // general communication commands:
  bool openConnection(std::string devName); // initialize device driver
  void closeConnection();
  void initListenerThread(); // initialize listener thread
  void faultReset(uint16_t deviceID);
  void initNMT();
  bool initDevice(uint16_t deviceID); // init NMT and 402 state machines
  bool shutdownDevice(uint16_t deviceID); // init NMT and 402 state machines
  void sendSync(uint32_t sleepTime_msec=0); // sends a SYNC (must be done in regular time intervals)

  // motor functions:
  // homing: perform device homing; returns "true" if "drive_referenced" appears in statusword
  // after homing; return "false" otherwise:
  bool homing(uint16_t deviceID); 

  // put device into specific mode of operation
  // "homing_mode", "profile_position_mode", "profile_velocity_mode", 
  // "torque_profile_mode", "interpolated_position_mode":
  bool driveMode(uint16_t deviceID, std::string mode);
  bool enableIPmode(uint16_t deviceID);

  // before drive moves (and PDOs can be received), "releaseBreak" must be invoked
  // to stop moving, "fastenBreak" must be invoked
  // these commands trigger the 402 state machine between "operation_enable"
  // and "ready_to_switch_on"
  bool releaseBreak(uint16_t deviceID); 
  bool enableBreak(uint16_t deviceID);

  void sendPos(uint16_t deviceID, int pos); // send position command as PDO (for IP mode)
}
#endif

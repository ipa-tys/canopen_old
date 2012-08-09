// API functions for CANopen communication

#ifndef CANOPENHIGHLEVEL_H
#define CANOPENHIGHLEVEL

#include "canopenmsg.h"

namespace canopen {
  extern EDSDict eds;

  // general communication commands:
  bool openConnection(std::string devName); // initialize device driver
  void closeConnection();
  void initListenerThread(); // initialize listener thread
  // bool initAll(); // todo??
  bool initDevice(uint16_t deviceID); // init NMT and 402 state machines
  bool shutdownDevice(uint16_t deviceID); // init NMT and 402 state machines
  void sendSync(uint32_t sleepTime_msec); // sends a SYNC (must be done in regular time intervals)

  // motor functions:

  // perform device homing; returns "true" if "drive_referenced" appears in statusword
  // after homing; return "false" otherwise
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

  // void ipMode(uint16_t deviceID); // put device into IP mode, ready to receive PDOs
  void sendPos(uint32_t pos); // send position command as PDO (for IP mode)
}
#endif

// API functions for CANopen communication

#ifndef CANOPENHIGHLEVEL_H
#define CANOPENHIGHLEVEL

#include "canopenmsg.h"

namespace canopen {

  inline uint32_t rad2mdeg(double phi) {
    return static_cast<uint32_t>( round( phi/(2*M_PI)*360000.0 ) ); }
  
  /* std::vector<uint32_t> rad2mdeg(std::vector<double> phis) {
    std::vector<uint32_t> alphas;
    for (auto phi : phis)
      alphas.push_back( rad2mdeg( phi ) );
    return alphas;
    }*/

  inline double mdeg2rad(uint32_t alpha) {
    return static_cast<double>( static_cast<double>(alpha)/360000.0*2*M_PI ); }

  extern EDSDict eds;
  extern PDODict pdo;

  // general communication commands:
  bool openConnection(std::string devName); // initialize device driver
  void closeConnection();

  void initListenerThread(); // initialize listener thread
  void listenerFunc();

  void faultReset(uint16_t deviceID);
  void initNMT();
  void setSyncInterval(uint16_t deviceID, std::chrono::milliseconds sync_deltaT_msec);
  bool initDevice(uint16_t deviceID, std::chrono::milliseconds sync_deltaT_msec); // init NMT and 402 state machines
  bool shutdownDevice(uint16_t deviceID); // init NMT and 402 state machines
  void sendSync(uint32_t sleepTime_msec=0); // sends a SYNC (must be done in regular time intervals)

  // motor functions:
  // homing: perform device homing; returns "true" if "drive_referenced" appears in statusword
  // after homing; return "false" otherwise:
  bool homing(uint16_t deviceID); 
  void moveUntilUserInterrupt(uint16_t deviceID, int direction);

  // put device into specific mode of operation
  // "homing_mode", "profile_position_mode", "profile_velocity_mode", 
  // "torque_profile_mode", "interpolated_position_mode":
  bool driveMode(uint16_t deviceID, std::string mode);
  bool enableIPmode(uint16_t deviceID);

  double getPos(uint16_t deviceID);

  // before drive moves (and PDOs can be received), "releaseBreak" must be invoked
  // to stop moving, "fastenBreak" must be invoked
  // these commands trigger the 402 state machine between "operation_enable"
  // and "ready_to_switch_on"
  bool releaseBreak(uint16_t deviceID); 
  bool enableBreak(uint16_t deviceID);

  void sendPos(uint16_t deviceID, double pos_rad); // send position (rad) command as PDO (for IP mode)
}
#endif

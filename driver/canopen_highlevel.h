// API functions for CANopen communication

#ifndef CANOPENHIGHLEVEL_H
#define CANOPENHIGHLEVEL

#include "canopenmsg.h"

namespace canopen {

  inline int32_t rad2mdeg(double phi) {
    return static_cast<int32_t>( round( phi/(2*M_PI)*360000.0 ) ); }
  inline double mdeg2rad(int32_t alpha) {
    return static_cast<double>( static_cast<double>(alpha)/360000.0*2*M_PI ); }

  extern EDSDict eds;
  extern PDODict pdo;

  // general init and communication commands:
  bool openConnection(std::string devName); // initialize device driver
  void closeConnection();
  void listenerFunc();
  void initListenerThread(); // initialize listener thread
  void initNMT();
  void setSyncInterval(uint16_t deviceID,
		       std::chrono::milliseconds syncInterval);
  void sendSync(std::chrono::milliseconds syncInterval); 
  inline Message* getStatus(uint16_t deviceID) {
    return sendSDO(deviceID, "statusword"); }
  inline bool getStatus(uint16_t deviceID, std::string statusName) {
    return sendSDO(deviceID, "statusword")->checkForConstant(statusName); }
  bool waitForStatus
    (uint16_t deviceID, std::string statusName,
     std::chrono::milliseconds timeout=std::chrono::milliseconds(100));

  // motor functions:
  bool setMotorState(uint16_t deviceID, std::string targetState);
  bool homing(uint16_t deviceID); 
  void interactiveHoming
    (uint16_t deviceID,
     int speedFactor,
     std::chrono::milliseconds syncInterval=std::chrono::milliseconds(0));
  bool driveMode(uint16_t deviceID, std::string mode);
  // send target pos (rad) as PDO (for IP mode):
  void sendPos(uint16_t deviceID, double pos_rad); 
  double getPos(uint16_t deviceID);

}
#endif

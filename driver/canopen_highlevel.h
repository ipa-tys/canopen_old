#ifndef CANOPENHIGHLEVEL_H
#define CANOPENHIGHLEVEL

#include "canopenmsg.h"

namespace canopen {

  extern EDSDict eds;

  void debug_show_pendingSDOReplies();

  // composite of multiple CANopen commands
  bool initDevice(uint16_t deviceID);
  void initListener();
  void listener_func();
  void homing(uint16_t deviceID, uint32_t sleep_ms=0);
  
  // wrapper for single CANopen commands
  
  void stopRemoteNode();
  void sendPos(std::string alias, uint32_t pos);
  bool openConnection(std::string devName);
  void closeConnection();
  void ipMode(uint16_t deviceID);
  void sendSync();
  void modesOfOperationDisplay(uint16_t deviceID);
  Message* sendSDO(uint16_t deviceID, std::string alias, std::string param="", bool writeMode=true);
  void sendNMT(std::string param);
  Message* statusWord(uint16_t deviceID);
  Message* controlWord(uint16_t deviceID, std::string mode);

}

#endif

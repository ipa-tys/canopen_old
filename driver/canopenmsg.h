#ifndef CANOPENMSG_H
#define CANOPENMSG_H

#include <boost/bimap.hpp>
#include <string>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <fcntl.h>    // O_RDWR
#include <libpcan.h>
#include <vector>
#include <stdint.h>
#include <inttypes.h>
#include <thread>
#include "canopen_internal.h"


namespace canopen {
  // extern HANDLE h;
  // uint32_t getEDSconst(std::string alias, std::string constname);

  class EDSDict {
  public:
    EDSDict();
    uint32_t getConst(std::string alias, std::string constname);
    uint32_t getMask(std::string alias, std::string constname);
    uint8_t getLen(std::string alias);
    uint16_t getIndex(std::string alias);
    uint8_t getSubindex(std::string alias);
    std::string getAlias(uint16_t index, uint8_t subindex);

    // private:
    EDSClassSet d_;
  };

  class PDODict {
  public:
    PDODict();
    std::vector<std::string> getComponents(std::string alias);
    uint16_t getCobID(std::string alias); // todo: still needed?
    std::string getAlias(uint16_t cobID);
    uint16_t getNodeID(std::string alias);
    bool cobIDexists(uint16_t cobID);
  private:
    PDOClassSet d_;
  };

  class Message {  // abstract representation of CANopen messages
  public:
    Message(TPCANRdMsg m); // construct message from raw CAN message (for messages coming in from bus)
    Message(uint8_t nodeID, std::string alias, uint32_t value=0); // user-constructed non-PDO message
    Message(uint8_t nodeID, std::string alias, std::vector<uint32_t> values); // user-constructed PDO message
    void writeCAN(bool writeMode=true);
    static Message* readCAN(bool blocking=true);
    // std::vector<std::string> parse(); // todo: not needed? / translate message from device into human-readable 
    Message* waitForSDOAnswer();
    void debugPrint();
    void debugPrintFlags();
    bool checkForConstant(std::string constName); // only for SDOs so far
    std::vector<std::string> parseFlags();
  private:
    uint8_t nodeID_; 
    std::string alias_; // entry into EDSDict, or PDODict, depending on message
    std::vector<uint32_t> values_;

    std::string createMsgHash();
    std::string createMsgHash(TPCANMsg m);
  };
  
  // todo: add a flag to openConnection + static member variable in Message that performs logging of in- and outgoing messages with timestamp
  
  // wrapper functions for sending SDO, PDO, and NMT messages
  // sendSDO calls deliver the device response as return value: 
  Message* sendSDO(uint16_t deviceID, std::string alias, std::string param="", bool writeMode=true);
  void sendNMT(std::string param);
  // todo: generic sendPDO

  // only for testing purposes, print map of sent SDOs waiting for reply:
  void debug_show_pendingSDOReplies(); 
}

// todo (possibly): message caching; data changing of existing messages (for max. efficiency)
#endif

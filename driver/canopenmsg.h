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

#include <iostream>
#include <string>
#include <libpcan.h>
#include <set>
#include <map>
#include <queue>
#include <regex>
#include <pwd.h>
#include <fstream>


#include "canopen_internal.h"


namespace canopen {

  extern bool using_master_thread;
 

  class EDSDict {
  public:
    EDSDict();
    uint32_t getConst(std::string alias, std::string constname);
    uint32_t getMask(std::string alias, std::string constname);
    uint8_t getLen(std::string alias);
    std::string getAttr(std::string alias);
    uint16_t getIndex(std::string alias);
    uint8_t getSubindex(std::string alias);
    std::string getAlias(uint16_t index, uint8_t subindex);
    EDSClassSet d_;
  };

  class PDODict {
  public:
    PDODict();
    std::vector<std::string> getComponents(std::string alias);
    uint16_t getCobID(std::string alias); // todo: still needed?
    std::string getAlias(uint16_t cobID);
    // private:
    PDOClassSet d_;
  };

  class Message {  // abstract representation of CANopen messages
  public:
    Message(TPCANRdMsg m);
    // constructor for messages coming in from bus (TPCANRdMsg)
    
    Message(uint8_t nodeID, std::string alias, uint32_t value=0);
    // user-constructed non-PDO message
    
    Message(uint8_t nodeID, std::string alias, std::vector<int32_t> values);
    // user-constructed PDO message
    
    void writeCAN(bool directlyToCANBus=false); 
    // if toOutgoingMsgQueue==true, messages are not written directly on the
    // CAN bus, but instead are put into the queue "outgoingMsgQueue" which
    // is a global variable within the "canopen" namespace

    static Message* readCAN(bool blocking=true);
    Message* waitForSDOAnswer();
    bool checkForConstant(std::string constName); // only for SDOs so far
    std::vector<std::string> parseFlags();

    bool contains(std::string indexAlias); 
    // check if PDO contains a given index from indices.csv
    // (e.g. "position_actual_values")

    uint32_t get(std::string indexAlias);
    // return the value of a given index contained in a PDO
    // (use Message::contains before to check if index is present in PDO)

    // private:
    uint8_t nodeID_; 
    std::string alias_; // entry into EDSDict, or PDODict, depending on message
    std::vector<int32_t> values_;

    std::chrono::microseconds timeStamp_;

    std::string createMsgHash();
    std::string createMsgHash(TPCANMsg m);
    
    // todo (optional): message caching; data changing of existing messages (for max. efficiency)
    void debugPrint(); // only for debug/testing
    void debugPrintFlags();
  };

  
  // wrapper functions for sending SDO, PDO, and NMT messages
  // sendSDO calls deliver the device response as return value: 
  Message* sendSDO(uint16_t deviceID, std::string alias, std::string param="");
  Message* sendSDO(uint16_t deviceID, std::string alias, uint32_t value);

  void sendNMT(std::string param);
  void sendPDO(uint16_t deviceID, std::string alias, std::vector<int32_t> data);

  // only for debug/testing purposes:
  void debug_show_pendingSDOReplies(); 
  void debugPrint_TPCAN(TPCANMsg msg);

  extern std::queue<Message> outgoingMsgQueue; // todo: only for debugging
  extern std::queue<Message> incomingPDOs;

}



#endif

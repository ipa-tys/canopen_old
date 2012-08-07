#include "canopen_highlevel.h"

namespace canopen {

   bool openConnection(std::string devName) {
    h = LINUX_CAN_Open(devName.c_str(), O_RDWR);
    if (!h) {
      // std::cout << "Cannot open CAN device" << std::endl;
      return false;
    }
    errno = CAN_Init(h, CAN_BAUD_500K, CAN_INIT_TYPE_ST);
    return true;
  }

  void listener_func() {
    TPCANRdMsg m;
    Message* msg;
    
    while (true) {
      // std::cout << "hi, listener" << std::endl;
      msg = Message::readCAN(true);
      // std::cout << "message received listener" << std::endl;
    }
  }


  void initListener() {
    std::thread listener_thread(listener_func);
    listener_thread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }


  void ipMode(uint16_t deviceID) {
    Message(deviceID, "modes_of_operation",
	    eds.getConst("modes_of_operation", "interpolated_position_mode")).writeCAN();
  }
  
  Message* controlWord(uint16_t deviceID, std::string mode) { // todo: remove
    Message m(deviceID, "controlword", eds.getConst("controlword", mode));
    m.writeCAN();
    Message* statusMsg = m.waitForSDOAnswer();
    std::cout << "controlword answer:" << std::endl;
    statusMsg->debugPrint();
    return statusMsg;
  }

    Message* sendSDO(uint16_t deviceID, std::string alias, std::string param, bool writeMode) {
    Message* m;
    if (param != "") 
      m = new Message(deviceID, alias, eds.getConst(alias, param));
    else {
      m = new Message(deviceID, alias);
      std::cout << "HERE!!!!! " << alias << "    " << static_cast<int>(writeMode) << std::endl;
    }
    m->writeCAN(writeMode);
    Message* reply = m->waitForSDOAnswer();
    delete m;
    return reply;
  }

  void sendNMT(std::string param) {
    Message(0, "NMT", eds.getConst("NMT", param)).writeCAN();
    std::this_thread::sleep_for(std::chrono::milliseconds(20)); 
  }

  void sendSync() { Message(0, "Sync").writeCAN(); }
  void sendPos(std::string alias, uint32_t pos) {
    std::vector<uint32_t> v;
    v.push_back(eds.getConst("controlword", "start_homing|enable_ip_mode"));
    v.push_back(0);
    v.push_back(pos);
    Message(alias, v).writeCAN();
  }
  void closeConnection() { CAN_Close(h); }
  void stopRemoteNode() { // only for testing
    std::cout << "stop remote node" << std::endl;
    Message(0, "NMT", eds.getConst("NMT", "stop_remote_node")).writeCAN();
  }

  bool initDevice(uint16_t deviceID) {
    sendNMT("stop_remote_node");
    sendNMT("start_remote_node");
    sendSDO(12, "controlword", "sm_shutdown");
    sendSDO(12, "controlword", "sm_switch_on");
    sendSDO(12, "controlword", "sm_enable_operation");
    Message* SDOreply = sendSDO(12, "statusword", "", false);
    return SDOreply->checkForConstant("operation_enable"); // return "true" if device is indeed operational
  }

  void homing(uint16_t deviceID, uint32_t sleep_ms) {
    Message(deviceID, "modes_of_operation", eds.getConst("modes_of_operation", "homing_mode")).writeCAN();
    Message(deviceID, "controlword", eds.getConst("controlword", "start_homing|enable_ip_mode")).writeCAN();
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms)); 
  }
  Message* statusWord(uint16_t deviceID) {  // todo: remove
    Message m(deviceID, "statusword");
    m.writeCAN(false);
    std::cout << "statusword waiting for answer...." << std::endl;
    // TPCANMsg msg = 
    return m.waitForSDOAnswer();
    // std::cout << "statusword received answer." << std::endl;
  }  // todo check
  
  void modesOfOperationDisplay(uint16_t deviceID) { Message(deviceID, "statusword").writeCAN(false); }





}

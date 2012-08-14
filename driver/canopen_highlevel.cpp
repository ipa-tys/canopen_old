#include "canopen_highlevel.h"

namespace canopen {
  // todo: canopen::waitForConstant (like checkForConstant, but with loop)

  // ---------------------- general communication commands: -------------------

  bool openConnection(std::string devName) {
    h = LINUX_CAN_Open(devName.c_str(), O_RDWR);
    if (!h) return false;
    errno = CAN_Init(h, CAN_BAUD_500K, CAN_INIT_TYPE_ST);
    return true;
  }
  
  void closeConnection() { CAN_Close(h); }
  
  void listenerFunc() {
    TPCANRdMsg m;
    Message* msg;
    while (true) msg = Message::readCAN(true);
  }

  void initListenerThread() {
    std::thread listener_thread(listenerFunc);
    listener_thread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  void faultReset(uint16_t deviceID) {
    // sendSDO(deviceID, "controlword", "fault_reset");
    sendSDO(deviceID, "controlword", "reset_fault_0");
    sendSDO(deviceID, "controlword", "reset_fault_1");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  void initNMT() {
    sendNMT("stop_remote_node");   // todo: change this!, check NMT state
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    sendNMT("start_remote_node");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  bool initDevice(uint16_t deviceID) {
    sendSDO(deviceID, "controlword", "sm_shutdown");
    while (!sendSDO(deviceID, "statusword", "", false)->checkForConstant("ready_to_switch_on")) {
      std::cout << "waiting.............." << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    sendSDO(deviceID, "controlword", "sm_switch_on");
    while (!sendSDO(deviceID, "statusword", "", false)->checkForConstant("switched_on")) {
      std::cout << "waiting.............." << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }


    // sendSDO(deviceID, "controlword", "sm_enable_operation");
    // Message* SDOreply = sendSDO(deviceID, "statusword", "", false);
    // return "true" if device is indeed operational ("false" otherwise):
    // return SDOreply->checkForConstant("operation_enable");
    return true;  // todo: implement time out
  }

  bool shutdownDevice(uint16_t deviceID) {
    sendSDO(deviceID, "controlword", "sm_shutdown");
    return true; // todo: check why this does not always return a value
    // return sendSDO(deviceID, "statusword", "", false)->checkForConstant("ready_to_switch_on");
  }

  void sendSync(uint32_t sleepTime_msec) {
    Message(0, "Sync").writeCAN(); 
    if (sleepTime_msec > 0)
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime_msec));
  }
  
  // ------------- motor functions: -------------------------------------------

  bool homing(uint16_t deviceID) {
    sendSDO(deviceID, "modes_of_operation", "homing_mode");
    sendSDO(deviceID, "controlword", "start_homing|enable_ip_mode");

    // wait for drive to start moving:
    while (!sendSDO(deviceID, "statusword", "", false)->checkForConstant("drive_is_moving"))
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
    
    // wait for drive to stop moving:
    while (sendSDO(deviceID, "statusword", "", false)->checkForConstant("drive_is_moving")) 
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
    
    // return true if drive signals it is referenced; false otherwise:
    return sendSDO(deviceID, "statusword", "", false)->checkForConstant("drive_referenced");
  }

  bool driveMode(uint16_t deviceID, std::string mode) {
    sendSDO(deviceID, "modes_of_operation", mode);
    return sendSDO(deviceID, "modes_of_operation_display", "", false)->checkForConstant(mode);
  }
  
  bool releaseBreak(uint16_t deviceID) {
    sendSDO(deviceID, "controlword", "sm_enable_operation");
    while (!sendSDO(deviceID, "statusword", "", false)->checkForConstant("operation_enable")) {
      std::cout << "waiting.............." << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return true; // todo timeout
  }

  bool enableBreak(uint16_t deviceID) {
    sendSDO(deviceID, "controlword", "sm_switch_on");
    /* while (!sendSDO(deviceID, "statusword", "", false)->checkForConstant("switched_on")) {
       std::cout << "waiting.............." << std::endl;
       std::this_thread::sleep_for(std::chrono::milliseconds(10));
       }*/
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    return true; // todo timeout
  }

  bool enableIPmode(uint16_t deviceID) {
    bool ok = true;
    ok = ok & enableBreak(deviceID);
    ok = ok & driveMode(deviceID, "interpolated_position_mode");
    ok = ok & releaseBreak(deviceID);
    return ok; // todo
  }

  void sendPos(uint16_t deviceID, uint32_t pos) {  
    std::vector<uint32_t> v;
    v.push_back(eds.getConst("controlword", "start_homing|enable_ip_mode"));
    v.push_back(0);
    v.push_back(pos);
    // Message("schunk_default_rPDO_12", v).writeCAN();  // works for PRH module
    Message(deviceID, "schunk_default_rPDO", v).writeCAN();
  }

}

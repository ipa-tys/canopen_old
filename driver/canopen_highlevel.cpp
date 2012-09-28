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
    while (true) {
      msg = Message::readCAN(true);
    }
  }

  void initListenerThread() {
    std::thread listener_thread(listenerFunc);
    listener_thread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  double getPos(uint16_t deviceID) {
    Message* m = sendSDO(deviceID, "position_actual_value");
      return mdeg2rad( m->values_[0] );
  }

  void faultReset(uint16_t deviceID) {
    // sendSDO(deviceID, "controlword", "fault_reset");
    sendSDO(deviceID, "controlword", "reset_fault_0");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    sendSDO(deviceID, "controlword", "reset_fault_1");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  void initNMT() {
    sendNMT("stop_remote_node");   // todo: change this!, check NMT state
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    sendNMT("start_remote_node");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  void setSyncInterval(uint16_t deviceID,
		       std::chrono::milliseconds sync_deltaT_msec) {
    uint32_t dt = static_cast<uint32_t>( sync_deltaT_msec.count() );
    std::cout << "Sync interval: " << dt << std::endl;
    sendSDO(deviceID, "ip_time_units", dt);

    sendSDO(deviceID, "ip_time_index", "milliseconds");

    sendSDO(deviceID, "sync_timeout_factor", 0);
  }

  bool initDevice(uint16_t deviceID, std::chrono::milliseconds sync_deltaT_msec) {
    sendSDO(deviceID, "controlword", "sm_shutdown");
    auto tic = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds timeoutDuration(500);
    bool timeout = false;
    while (!timeout && !sendSDO(deviceID, "statusword")->checkForConstant("ready_to_switch_on")) {
      // std::cout << "waiting.............." << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      if (std::chrono::high_resolution_clock::now() > tic + timeoutDuration) {
	timeout = true;
	std::cout << "Device " << deviceID << " initDevice timeout" << std::endl;
      }
    }

    // setSyncInterval(deviceID, sync_deltaT_msec);

    sendSDO(deviceID, "controlword", "sm_switch_on");
    timeout = false;
    while (!timeout && !sendSDO(deviceID, "statusword")->checkForConstant("switched_on")) {
      // std::cout << "waiting.............." << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      if (std::chrono::high_resolution_clock::now() > tic + timeoutDuration) {
	timeout = true;
	std::cout << "Device " << deviceID << " initDevice timeout" << std::endl;
      }
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
    while (!sendSDO(deviceID, "statusword")->checkForConstant("drive_is_moving"))
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
    
    // wait for drive to stop moving:
    while (sendSDO(deviceID, "statusword")->checkForConstant("drive_is_moving")) 
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
    
    // return true if drive signals it is referenced; false otherwise:
    return sendSDO(deviceID, "statusword")->checkForConstant("drive_referenced");
  }

  void moveUntilUserInterrupt(uint16_t deviceID, int direction=1) { // todo: direction->speed
    bool pressed = false;
    std::thread keyThread([&]() { std::string tt; std::getline(std::cin, tt); pressed=true; });
    keyThread.detach();
    canopen::homing(deviceID);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    canopen::enableIPmode(deviceID);
    double pos = 0;
    canopen::sendSync(10);
    canopen::sendSync(10);
    while (!pressed) {
      canopen::sendPos(deviceID, pos);
      pos += direction * M_PI / 3600.0;
      canopen::sendSync(10);
    }
    // canopen::homing(deviceID);
    // canopen::enableBreak(deviceID);
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  bool driveMode(uint16_t deviceID, std::string mode) {
    sendSDO(deviceID, "modes_of_operation", mode);
    return sendSDO(deviceID, "modes_of_operation_display")->checkForConstant(mode);
  }
  
  bool releaseBreak(uint16_t deviceID) {
    sendSDO(deviceID, "controlword", "sm_enable_operation");
    bool timeout = false;
    auto tic = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds timeoutDuration(1500);
    while (!timeout && !sendSDO(deviceID, "statusword")->checkForConstant("operation_enable")) {
      // std::cout << "waiting.............." << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      if (std::chrono::high_resolution_clock::now() > tic + timeoutDuration) {
	timeout = true;
	std::cout << "Device " << deviceID << " releaseBreak timeout" << std::endl;
      }
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
    // ok = ok & enableBreak(deviceID);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ok = ok & driveMode(deviceID, "interpolated_position_mode");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ok = ok & releaseBreak(deviceID);
    std::cout << "IP mode enabled? " << ok << std::endl;
    return ok; // todo
  }

  /* void sendPos(uint16_t deviceID, double pos) {  
    std::vector<uint32_t> v;
    v.push_back(eds.getConst("controlword", "start_homing|enable_ip_mode"));
    v.push_back(0);
    v.push_back(pos);
    Message(deviceID, "schunk_default_rPDO", v).writeCAN();
    } */

  void sendPos(uint16_t deviceID, double pos_rad) {
    std::vector<int32_t> data =
      {eds.getConst("controlword", "start_homing|enable_ip_mode"),
       0, 
       rad2mdeg(pos_rad) }; // Schunk has millidegrees as unit instead of rad
    sendPDO(deviceID, "schunk_default_rPDO", data);
  }

}

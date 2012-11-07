// Copyright (c) 2012 Fraunhofer Institute
// for Manufacturing Engineering and Automation (IPA)
// See the file license.txt for copying permission.

#include "canopen_highlevel.h"

namespace canopen {

  // general communication commands:

  bool openConnection(std::string devName) {
    h = LINUX_CAN_Open(devName.c_str(), O_RDWR);
    if (!h) return false;
    errno = CAN_Init(h, CAN_BAUD_500K, CAN_INIT_TYPE_ST);
    return true;
  }
  
  void closeConnection() { CAN_Close(h); }
  


  void initNMT() {
    // todo: check NMT state; allow device-specific NMT
    sendNMT("stop_remote_node");   
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sendNMT("start_remote_node");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  void setSyncInterval(uint16_t deviceID,
		       std::chrono::milliseconds syncInterval) {
    sendSDO(deviceID, "ip_time_units",
	    static_cast<uint32_t>( syncInterval.count() ));
    sendSDO(deviceID, "ip_time_index", "milliseconds");
    sendSDO(deviceID, "sync_timeout_factor", 0); // 0 = disable sync timeout
  }

  void sendSync(std::chrono::milliseconds syncInterval) {
    Message(0, "Sync").writeCAN(); 
    if (syncInterval > std::chrono::milliseconds(0))
      std::this_thread::sleep_for(syncInterval);
  }

  bool waitForStatus(uint16_t deviceID, std::string statusName,
		     std::chrono::milliseconds timeout) {
    bool isTimeout = false;
    auto tic = std::chrono::high_resolution_clock::now();
    while (!isTimeout && !getStatus(deviceID, statusName)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      if (std::chrono::high_resolution_clock::now() > tic + timeout)
	isTimeout = true;
    }
    return !isTimeout;
  }

  // motor functions:

  bool setMotorState(uint16_t deviceID, std::string targetState) {
    // goes shortest way in the motor state machine to the desired target state
    // todo: correct function of fault reset has not been tested yet
    Message m;

    if (targetState == "operation_enable") {
      
      m = getStatus(deviceID);
      if (m.checkForConstant("operation_enable"))
	return true;
      if (m.checkForConstant("fault")) {
	setStatus(deviceID, "reset_fault_1");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

      if (getStatus(deviceID, "switch_on_disabled"))
	setStatus(deviceID, "sm_shutdown");
      if (getStatus(deviceID, "ready_to_switch_on"))
	setStatus(deviceID, "sm_switch_on");
      if (getStatus(deviceID, "switched_on"))
	setStatus(deviceID, "sm_enable_operation");

      return getStatus(deviceID, "operation_enable");

    } else if (targetState == "switched_on") {

      m = getStatus(deviceID);
      if (m.checkForConstant("switched_on")) 
	return true;
      if (m.checkForConstant("operation_enable")) 
	setStatus(deviceID, "sm_switch_on");

      m = getStatus(deviceID);
      if (m.checkForConstant("switched_on"))
	return true;
      if (m.checkForConstant("fault")) {
	setStatus(deviceID, "reset_fault_1");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

      if (getStatus(deviceID, "switch_on_disabled"))
	setStatus(deviceID, "sm_shutdown");
      
      if (getStatus(deviceID, "ready_to_switch_on"))
	setStatus(deviceID, "sm_switch_on");
      
      return getStatus(deviceID, "switched_on");

    }  else if (targetState == "ready_to_switch_on") {

      m = getStatus(deviceID);
      if (m.checkForConstant("ready_to_switch_on")) 
	return true;
      if (m.checkForConstant("operation_enable")) 
	setStatus(deviceID, "sm_switch_on");

      if (getStatus(deviceID, "switched_on"))
	setStatus(deviceID, "sm_shutdown");
     
      m = getStatus(deviceID);
      if (m.checkForConstant("ready_to_switch_on"))
	return true;
      if (m.checkForConstant("fault")) {
	setStatus(deviceID, "reset_fault_1");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
      
      if (getStatus(deviceID, "switch_on_disabled"))
	setStatus(deviceID, "sm_shutdown");
      
      return getStatus(deviceID, "ready_to_switch_on");

    }  else if (targetState == "switch_on_disabled") {
      
      m = getStatus(deviceID);
      if (m.checkForConstant("switch_on_disabled")) 
	return true;
      if (m.checkForConstant("fault")) {
	setStatus(deviceID, "reset_fault_1");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

      if (getStatus(deviceID, "operation_enable"))
	setStatus(deviceID, "sm_switch_on");
      setStatus(deviceID, "disable_voltage");
      return getStatus(deviceID, "switch_on_disabled");

    } else {
      std::cout << "This is not a supported motor state." << std::endl;
      return false;
    }
  }

  bool homing(uint16_t deviceID) {
    // set current position as device 0 position
    // returns "true" if "drive_referenced" has appeared in device statusword
    canopen::setMotorState(deviceID, "operation_enable");

    sendSDO(deviceID, "modes_of_operation", "homing_mode");
    sendSDO(deviceID, "controlword", "start_homing|enable_ip_mode");

    // wait for drive to start moving:
    while (!sendSDO(deviceID, "statusword").checkForConstant("drive_is_moving"))
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
    
    // wait for drive to stop moving:
    while (sendSDO(deviceID, "statusword").checkForConstant("drive_is_moving")) 
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); 

    while (!sendSDO(deviceID, "statusword").checkForConstant("drive_referenced")) 
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); 

    // return true if drive signals it is referenced; false otherwise:
    return sendSDO(deviceID, "statusword").checkForConstant("drive_referenced");
  }

  void interactiveHoming(uint16_t deviceID, int speedFactor=1,
		  std::chrono::milliseconds syncInterval) {
    // the device moves until user interrupts with <Return>
    // the stop position will be the new 0 (home/reference) position
    canopen::setMotorState(deviceID, "operation_enable");
    
    bool pressed = false;
    std::thread keyThread([&]() { 
	std::string tt; 
	std::getline(std::cin, tt);
	pressed=true;
      });
    keyThread.detach();
    
    canopen::homing(deviceID); // ensures that start position = 0
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    canopen::driveMode(deviceID, "interpolated_position_mode");
    double pos = 0;
    while (!pressed) {
      canopen::sendPos(deviceID, pos);
      pos += speedFactor * 0.1 * M_PI / 3600.0;
      canopen::sendSync(syncInterval);
    }
    canopen::homing(deviceID); // ensures that end position = 0
  }

  bool driveMode(uint16_t deviceID, std::string mode) {
    sendSDO(deviceID, "modes_of_operation", mode);
    // todo: waitForConstant
    return 
      sendSDO(deviceID, "modes_of_operation_display").checkForConstant(mode);
  }
  
  void sendPos(uint16_t deviceID, double pos_rad) {
    std::vector<int32_t> data =
      {eds.getConst("controlword", "start_homing|enable_ip_mode"),
       0, 
       rad2mdeg(pos_rad) }; // Schunk has millidegrees as unit instead of rad
    sendPDO(deviceID, "schunk_default_rPDO", data);
  }
  
  double getPos(uint16_t deviceID) {
    Message m( sendSDO(deviceID, "position_actual_value") );
    return mdeg2rad( m.values_[0] );
  }
  
}

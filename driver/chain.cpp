#include "chain.h"

namespace canopen {
  std::chrono::milliseconds syncInterval; // todo: necessary?
  std::map<std::string, Chain*> chainMap; // todo: necessary?

  void Device::CANopenInit() {
    setSyncInterval(CANid_, canopen::syncInterval); 
    setMotorState(CANid_, "operation_enable");
    driveMode(CANid_, "interpolated_position_mode");
  }

  void Device::update(Message m) {
    assert(timeStamp_ < m.timeStamp_ && 
	   "Incoming message timeStamp overflow (std::chrono::microseconds); " 
	   "adapt timeStamp_ member variable in class canopen::Message!");

    if (m.contains("position_actual_value")) {
      double newPos = mdeg2rad( m.get("position_actual_value") );
      // only update velocity if there is already a previous pos and timeStamp stored:
      if (timeStamp_ != std::chrono::microseconds(0)) {
	std::chrono::microseconds deltaTime( m.timeStamp_ - timeStamp_ );
	double deltaTime_double = static_cast<double>(deltaTime.count()) * 0.000001;
	actualVel_ = (newPos - actualPos_) / deltaTime_double;
	if (!initialized_) {
	  initialized_ = true;
	  desiredPos_ = actualPos_;
	  desiredVel_ = 0;
	}
      }
      actualPos_ = newPos;
      timeStamp_ = m.timeStamp_;
    }

    // Note: for any other canopen::Device member variables that should
    // updated continuously you can add the code here
  }

}

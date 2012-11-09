// Copyright (c) 2012 Fraunhofer Institute
// for Manufacturing Engineering and Automation (IPA)
// See the file license.txt for copying permission.

// This program moves a device with a given target velocity
// which is reached by a velocity ramp of constant acceleration
// See the user manual for details:
// https://github.com/ipa-tys/canopen/blob/master/doc/usermanual.pdf?raw=true

#include <chrono>
#include <listener.h>
#include <canopenmaster.h>

int main(int argc, char *argv[]) {

  if (argc != 6) {
    std::cout << "Arguments:" << std::endl
      	      << "(1) device file" << std::endl
	      << "(2) CAN deviceID" << std::endl
      	      << "(3) sync rate [msec]" << std::endl
	      << "(4) target velocity [rad/sec]" << std::endl
      	      << "(5) acceleration [rad/sec^2]" << std::endl
	      << "Example 1: ./move_device /dev/pcan32 12 10 0.2 0.05" << std::endl
      	      << "Example 2 (reverse direction): "
	      << "./move_device /dev/pcan32 12 10 -0.2 -0.05" << std::endl;
    return -1;
  }
  std::cout << "Interrupt motion with Ctrl-C" << std::endl;
  std::string deviceFile = std::string(argv[1]);
  uint16_t deviceID = std::stoi(std::string(argv[2]));
  canopen::syncInterval = std::chrono::milliseconds(std::stoi(std::string(argv[3])));
  double targetVel = std::stod(std::string(argv[4]));
  double accel = std::stod(std::string(argv[5]));

  // In this example, the canopen::chainMap is constructed manually,
  // but there are also functions to construct it from a YAML file,
  // or (in ros_canopen) from the ROS parameter server, cf. user manual.
  canopen::using_master_thread = true;
  canopen::DeviceDescription CANdevice;
  CANdevice.name = "device1";
  CANdevice.id = deviceID;
  CANdevice.bus = deviceFile;
  canopen::ChainDescription CANchain;
  CANchain.name = "chain1";
  CANchain.devices.push_back(CANdevice);
  canopen::initChainMap( { CANchain } );

  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 

  canopen::initListenerThread(canopen::smartListener);
  // canopen::initIncomingPDOProcessorThread();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  canopen::initMasterThread();

  canopen::initNMT();
  for (auto it : canopen::chainMap) 
    it.second->CANopenInit();

  // send velocity commands; these could also come from
  // a different thread, or even a different process
  // as in ros_canopen
  //std::thread client_thread(clientFunc);
  //client_thread.detach();
  std::chrono::milliseconds accelerationTime
    ( static_cast<int>(round( 1000.0 * targetVel / accel)) );
  double vel = 0;
  auto startTime = std::chrono::high_resolution_clock::now();
  auto tic = std::chrono::high_resolution_clock::now();

  // increasing velocity ramp up to target velocity:
  std::cout << "Accelerating to target velocity" << std::endl;
  while (tic < startTime + accelerationTime) {
    tic = std::chrono::high_resolution_clock::now();
    vel = accel * 0.000001 *
      std::chrono::duration_cast<std::chrono::microseconds>(tic-startTime).count();
    canopen::chainMap["chain1"]->setVel({vel});
    std::this_thread::sleep_for
      (canopen::syncInterval - (std::chrono::high_resolution_clock::now() - tic));
  }

  // constant velocity when target vel has been reached:
  std::cout << "Target velocity reached!" << std::endl;
  canopen::chainMap["chain1"]->setVel({targetVel});
    
  while (true)
    std::this_thread::sleep_for(std::chrono::seconds(1));

  return 0;
}

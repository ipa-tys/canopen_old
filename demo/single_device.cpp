// simple demo of how to use the canopen_highlevel functionality
// with a single CANopen motor (tested with Schunk PRH module)
// first, the drive is referenced (homing), then some example
// motions are performed in interpolated_position_mode

#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cout << "Please call the program with the CAN deviceID,"
	      << "followed by the syncTime in msec"
	      << "e.g. './single_device 12 10'" << std::endl;
    return -1;
  }
  uint16_t deviceID = std::stoi(std::string(argv[1]));

  uint32_t sync_deltaT_msec_int = std::stoi(argv[2]);
  std::cout << sync_deltaT_msec_int << std::endl;
  std::chrono::milliseconds sync_deltaT_msec(sync_deltaT_msec_int);

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
 
  // initialize listener thread:
  canopen::initListenerThread();    

  // put NMT and 402 state machines for device  to operational:
  // canopen::faultReset(deviceID);
  canopen::initNMT();
  canopen::setSyncInterval(deviceID, sync_deltaT_msec);

  if (!canopen::initDevice(deviceID, sync_deltaT_msec)) { 
    std::cout << "Device could not be initialized; aborting." << std::endl;
    return -1;
  } 

  // performing homing of device:
  /*  if (!canopen::homing(deviceID)) {
    std::cout << "Homing was not successful; aborting." << std::endl;
    return -1;
    } */

  // put device into interpolated_position_mode:
  if (!canopen::enableIPmode(deviceID)) {
    std::cout << "Could not switch device into IP mode; aborting."
	      << std::endl;
    return -1;
  }

  // move a bit in IP mode:
  
  double step_size = 2*M_PI / 1000.0;
  // double step_size = 2*M_PI / 20000.0;

  // std::cout << canopen::getPos(4) << std::endl;
  // return -1;

  double pos = 0;
  /* for (int i=0; i<10; i++) {
    canopen::sendPos(deviceID, pos);
    canopen::sendSync(10);
    } */

  for (int i=0; i<1000; i++) {
    canopen::sendPos(deviceID, pos);
    pos += step_size;
    canopen::sendSync(sync_deltaT_msec_int);
  }
  std::cout << pos << std::endl;

  for (int i=1000; i>0; i--) {
    canopen::sendPos(deviceID, pos);
    pos -= step_size;
    canopen::sendSync(sync_deltaT_msec_int);
  }

  std::cout << pos << std::endl;
  
  // shutdown device and connection:
  canopen::enableBreak(deviceID);
  canopen::shutdownDevice(deviceID);
  canopen::closeConnection(); 
  return 0;
}

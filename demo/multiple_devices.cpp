// simple demo of how to use the canopen_highlevel functionality
// with a chain of CANopen devices (tested with Schunk powerball arm).
// First, the drive is referenced (homing), then some example
// motions are performed in interpolated_position_mode

#include <thread>
#include <chrono>
#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Please call the program with the syncTime in msec,"
	      << "followed by the CAN deviceIDs,"
	      << "e.g. './multiple_devices 10 3 4 5 6 7 8'" << std::endl;
    return -1;
  }
  
  uint32_t sync_deltaT_msec = std::stoi(argv[1]);

  std::vector<uint16_t> deviceIDs;
  for (int i=2; i<argc; i++) deviceIDs.push_back(std::stoi(std::string(argv[i])));

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
  
  // initialize listener thread:
  canopen::initListenerThread();    
    
  // put NMT and 402 state machines for devices  to operational:
  // for (auto it : deviceIDs) canopen::faultReset(it);
  canopen::initNMT();
  for (auto it : deviceIDs) 
    if (!canopen::initDevice(it, std::chrono::milliseconds(sync_deltaT_msec) )) {
      std::cout << "Device " << it << " could not be initialized; aborting." << std::endl;
      return -1;
    } 
  
  // performing homing of devices:
  for (auto it : deviceIDs) {
    std::cout << "homing device: " << it << std::endl;
    if (!canopen::homing(it)) {
      std::cout << "Homing was not successful; aborting." << std::endl;
      return -1;
    }  
  }  

  
  // move a bit in IP mode:
  
  // double step_size = 2 * M_PI / 8000.0;

  // double pos = 0;
  
  // std::vector<double> positions;
  // for (auto it : deviceIDs) positions.push_back( canopen::getPos(it) );

  /* for (auto it : positions)
    std::cout << it << "   ";
    std::cout << std::endl; */


  // put devices into interpolated_position_mode:
  for (auto it : deviceIDs)
    if (!canopen::enableIPmode(it)) {
      std::cout << "Could not switch device into IP mode; aborting."
		<< std::endl;
      return -1;
    }

  std::cout << "now moving" << std::endl;
  
  canopen::sendSync(sync_deltaT_msec);
  canopen::sendSync(sync_deltaT_msec);


  double mypos = 0.0;
  double step_size = 2*M_PI / 12000.0;

  for (int j=0; j<5; j++) {
    for (int i=0; i<=300; i++) {
      // for (int i=0; i<3; i++) {
      for (int i=0; i<deviceIDs.size(); i++) {
	// canopen::sendPos(deviceIDs[i], positions[i]);
	canopen::sendPos(deviceIDs[i], mypos);
	mypos = mypos + step_size;
	// positions[i] += step_size;
      }
      canopen::sendSync(sync_deltaT_msec);
    }
    
    //for (int i=0; i<3; i++)
    //  canopen::sendSync(sync_deltaT_msec);


    for (int i=300; i>0; i--) {
      for (int i=0; i<deviceIDs.size(); i++) {
	// canopen::sendPos(deviceIDs[i], positions[i]);
	// positions[i] -= step_size;
	canopen::sendPos(deviceIDs[i], mypos);
	mypos = mypos - step_size;

      }
      canopen::sendSync(sync_deltaT_msec);
    }   
  }
  
  return -1;

  // shutdown devices and connection:
  for (auto it : deviceIDs) {
    canopen::enableBreak(it);
    canopen::shutdownDevice(it);
  }
  canopen::closeConnection(); 
  return 0;
}

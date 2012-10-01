#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {

  if (argc != 4) {
    std::cout
      << "Arguments:" 
      << "(1) CAN deviceID" << std::endl
      << "(2) SYNC interval in milliseconds" << std::endl
      << "(3) positive (right) or negative (left) speed factor" << std::endl
      << "Example: ./move_single_device 12 10 1" << std::endl;
    return -1;
  }
  uint16_t deviceID = std::stoi(std::string(argv[1]));
  std::chrono::milliseconds syncInterval(std::stoi(std::string(argv[2])));
  int speedFactor = std::stoi(std::string(argv[3]));

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 

  canopen::initListenerThread();    
  canopen::initNMT();
  canopen::setSyncInterval(deviceID, syncInterval);
  canopen::setMotorState(deviceID, "operation_enable");

  std::cout << "Drive referenced? " << canopen::getStatus(deviceID, "drive_referenced") << std::endl;
  double pos = fmod(canopen::getPos(deviceID), M_PI);
    std::cout << "Current position: " << pos << std::endl;

  bool ended = false;
  std::thread keyThread([&]() { 
      std::string tt; 
      while (!ended) {
	std::getline(std::cin, tt);
	if (tt=="q") 
	  ended = true;
	else if (tt=="+") {
	  speedFactor += 1;
	  std::cout << "Speed factor: " << speedFactor << std::endl;
	} else if (tt=="-") {
	  speedFactor -= 1;
	  std::cout << "Speed factor: " << speedFactor << std::endl;
	} 
      }
    });
  keyThread.detach();

  canopen::driveMode(deviceID, "interpolated_position_mode");
  while (!ended) {
    canopen::sendSync(syncInterval);
    if (speedFactor != 0) {
      canopen::sendPos(deviceID, pos);
      pos += speedFactor * M_PI / 3600.0;
    }
  }
   
  canopen::setMotorState(deviceID, "ready_to_switch_on");
  canopen::closeConnection();
  return 0;
}

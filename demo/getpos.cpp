#include <canopen_highlevel.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Please call the program with the CAN deviceID"
	      << "e.g. './single_device 12'" << std::endl;
    return -1;
  }
  uint16_t deviceID = std::stoi(std::string(argv[1]));

  // initialize CAN device driver:
  if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device; aborting." << std::endl;
    return -1;
  } 
 
  // initialize listener thread:
  canopen::initListenerThread();    

  // put NMT and 402 state machines for device  to operational:
  canopen::faultReset(deviceID);
  canopen::initNMT();
  if (!canopen::initDevice(deviceID)) { 
    std::cout << "Device could not be initialized; aborting." << std::endl;
    return -1;
  } 

  // canopen::homing(deviceID);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  double pos = canopen::getPos(deviceID);
  std::cout << "Pos: " << pos << std::endl;

}

#include "chain.h"

namespace canopen {

  // ---------- Device: everything inline so far

  // ---------- Chain:

  Chain::Chain(std::string chainName, 	std::vector<std::string> deviceNames,
	       std::vector<std::string> CANbuses, std::vector<uint16_t> CANids):
    alias_(chainName), sendPosActive_(false)
  {
    for (int i=0; i<deviceNames.size(); i++)
      devices_.push_back( Device(deviceNames[i], CANbuses[i], CANids[i]) );
  }

  void Chain::chainInit() {
    for (auto device : devices_)
      device.deviceInit();
  }

  void Chain::chainHoming() {
    for (auto device : devices_)
      device.deviceHoming();
  }

  std::vector<uint16_t> Chain::getDeviceIDs() {
    std::vector<uint16_t> deviceIDs;
    for (auto device : devices_)
      deviceIDs.push_back(device.CANid_);
    return deviceIDs;
  }

  void Chain::setPos(std::vector<int> positions) {
    if (!sendPosActive_)
      sendPosActive_ = true;
    for (int i=0; i<positions.size(); i++)
      devices_[i].setPos(positions[i]);
  }

  void Chain::sendPos() {
    std::vector<int> requestedPositions = getRequestedPos();
    std::vector<uint16_t> deviceIDs = getDeviceIDs();
    std::cout << "Chain: " << requestedPositions.size() << "   " << deviceIDs.size() << std::endl;
    for (int i=0; i<deviceIDs.size(); i++)
      canopen::sendPos(deviceIDs[i], requestedPositions[i]);
  }

  std::vector<int> Chain::getRequestedPos() {
    std::vector<int> requestedPositions;
    for (auto device : devices_)
      requestedPositions.push_back(device.getRequestedPos());
    std::cout << "POS: " << requestedPositions[0] << std::endl;
    return requestedPositions;
  }

}

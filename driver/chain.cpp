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


}

#include "chain.h"

namespace canopen {
  // parsing chain and device descriptions:
  void operator>> (const YAML::Node& node, DeviceDescription& d) {
    node["name"] >> d.name;
    node["id"] >> d.id;
    node["bus"] >> d.bus;
  }

  void operator>> (const YAML::Node& node, ChainDescription& c) {
    node["name"] >> c.name;
    const YAML::Node& devices = node["devices"];
    for (int i=0; i<devices.size(); i++) {
      DeviceDescription d;
      devices[i] >> d;
      c.devices.push_back(d);
    }
  }

  std::vector<ChainDescription> parseChainDescription(std::string filename) {
    std::ifstream fin(filename);
    YAML::Parser parser(fin);
    YAML::Node doc;
    parser.GetNextDocument(doc);
    
    std::vector<canopen::ChainDescription> chainDesc;
  
    for (int i=0; i<doc.size(); i++) {
      canopen::ChainDescription chain;
      doc[i] >> chain;
      chainDesc.push_back(chain);
    }
    return chainDesc;
  }



  // ---------- Device: everything inline so far

  void Device::updateStatusWithIncomingPDO(Message m) {
    // step 1: get components of message:
    std::vector<std::string> components = pdo.getComponents(m.alias_); // todo: implement Message::getComponents()
    // e.g. if m contains position, update position...
    // so far, only position_actual_value is implemented, todo: statusword etc.

    auto it = std::find(components.begin(), components.end(), "position_actual_value");
    if (it != components.end()) { 
      size_t ind = it - components.begin();
      // std::cout << "FOUND POS_ACT_VAL at index " << ind << std::endl;
      // std::cout << "UPDATING POS to: " << m.values_[ind] << std::endl;
      std::cout << "hi" << std::endl;
      current_position_ = 2* M_PI * m.values_[ind] / 360000.0;
      std::cout << "POS_ACT_VAL: " << m.values_[ind] << "   " << current_position_ << std::endl;
    }
  }

  // ---------- Chain:

  Chain::Chain(ChainDescription chainDesc):
    alias_(chainDesc.name), sendPosActive_(false) {
    for (auto d : chainDesc.devices)
      devices_.push_back( Device(d.name, d.bus, d.id) );
  }

  void Chain::chainInit() {
    for (auto device : devices_)
      device.deviceInit();
  }

  void Chain::chainHoming() {
    for (auto device : devices_)
      device.deviceHoming();
  }

  void Chain::chainIPmode() {
    for (auto device : devices_)
      device.deviceIPmode();
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
    // std::cout << "Chain: " << requestedPositions.size() << "   " << deviceIDs.size() << std::endl;
    for (int i=0; i<deviceIDs.size(); i++)
      canopen::sendPos(deviceIDs[i], requestedPositions[i]);
  }

  std::vector<int> Chain::getRequestedPos() {
    std::vector<int> requestedPositions;
    for (auto device : devices_)
      requestedPositions.push_back(device.getRequestedPos());
    return requestedPositions;
  }

  std::vector<int> Chain::getCurrentPos() {
    std::vector<int> currentPositions;
    for (auto device : devices_)
      currentPositions.push_back(device.getCurrentPos());
    return currentPositions;
  }

  void Chain::updateStatusWithIncomingPDO(Message m) { // todo save device lookup by using a map
    std::vector<uint16_t> deviceIDs = getDeviceIDs();
    int i=0;
    while (deviceIDs[i] != m.nodeID_) i++;
    devices_[i].updateStatusWithIncomingPDO(m);
  }


}

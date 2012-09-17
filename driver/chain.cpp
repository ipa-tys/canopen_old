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

  // ---------- Device: (also see inline definitions in chain.h)

  void Device::updateActualPosAndVel(double newPos, std::chrono::microseconds newTimeStamp) {
    assert(timeStamp_ < newTimeStamp && 
	   "Incoming message timeStamp overflow (std::chrono::microseconds); " 
	   "adapt timeStamp_ member variable in class canopen::Message!");

    // only update velocity if there is already a previous pos and timeStamp stored:
    if (timeStamp_ != std::chrono::microseconds(0)) {
      std::chrono::microseconds deltaTime( newTimeStamp - timeStamp_ );
      double deltaTime_double = static_cast<double>(deltaTime.count()) * 0.000001;

      std::cout << "Deltatime: " << deltaTime_double << std::endl;

      actualVel_ = (newPos - actualPos_) / deltaTime_double;
      std::cout << deltaTime_double << "   " << newPos << "  " << actualVel_ << std::endl;
    }
    actualPos_ = newPos;
    timeStamp_ = newTimeStamp;
  }

  void Device::updateStatusWithIncomingPDO(Message m) {
    if (m.contains("position_actual_value")) 
      updateActualPosAndVel( mdeg2rad( m.get("position_actual_value") ),
			     m.timeStamp_);

    // Note: you can add any other CANopen indices here that should be
    // continuously updated in canopen::Device member variables
    // (you can use Device::updateActualPosAndVel as an example)
  }

  void Device::setPos(double pos) {
    desiredVel_ = (pos - actualPos_) / (sync_deltaT_msec_.count() / 1000.0);
    desiredPos_ = pos;
  }

  void Device::setVel(double vel) {
    desiredVel_ = vel;
    desiredPos_ = desiredPos_ + vel * (sync_deltaT_msec_.count() / 1000.0);
  }

  // ---------- Chain:

  Chain::Chain(ChainDescription chainDesc, std::chrono::milliseconds sync_deltaT_msec):
    alias_(chainDesc.name), sendPosActive_(false) {
    for (auto d : chainDesc.devices)
      devices_.push_back( Device(d.name, d.bus, d.id, sync_deltaT_msec) );
  }

  void Chain::chainInit(std::chrono::milliseconds sync_deltaT_msec) {
    for (auto device : devices_)
      device.deviceInit(sync_deltaT_msec);
    sendPosActive_ = true;
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

  void Chain::setPos(std::vector<double> positions) {
    if (!sendPosActive_)
      sendPosActive_ = true;
    for (int i=0; i<positions.size(); i++)
      devices_[i].setPos(positions[i]);
  }

  void Chain::setVel(std::vector<double> velocities) {
    if (!sendPosActive_)
      sendPosActive_ = true;
    for (int i=0; i<velocities.size(); i++)
      devices_[i].setVel(velocities[i]);
  }

  void Chain::sendPos() {
    std::vector<double> desiredPositions = getDesiredPos();
    std::vector<uint16_t> deviceIDs = getDeviceIDs();
    for (int i=0; i<deviceIDs.size(); i++)
      canopen::sendPos(deviceIDs[i], desiredPositions[i]);
  }

  std::vector<double> Chain::getDesiredPos() {
    std::vector<double> desiredPositions;
    for (auto device : devices_)
      desiredPositions.push_back(device.getDesiredPos());
    return desiredPositions;
  }

  std::vector<double> Chain::getActualPos() {
    std::vector<double> actualPositions;
    for (auto device : devices_)
      actualPositions.push_back(device.getActualPos());
    return actualPositions;
  }

  std::vector<double> Chain::getDesiredVel() {
    std::vector<double> desiredVelocities;
    for (auto device : devices_)
      desiredVelocities.push_back(device.getDesiredVel());
    return desiredVelocities;
  }
  
  std::vector<double> Chain::getActualVel() {
    std::vector<double> actualVelocities;
    for (auto device : devices_)
      actualVelocities.push_back(device.getActualVel());
    return actualVelocities;
  }

  void Chain::updateStatusWithIncomingPDO(Message m) { // todo save device lookup by using a map
    std::vector<uint16_t> deviceIDs = getDeviceIDs();
    int i=0;
    while (deviceIDs[i] != m.nodeID_) i++;
    devices_[i].updateStatusWithIncomingPDO(m);
  }


}

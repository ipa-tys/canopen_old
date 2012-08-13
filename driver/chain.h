#ifndef CHAIN_H
#define CHAIN_H

class Device {
 
 public:
  Device(uint8_t devID, std::string devName, HANDLE h);
  void initDevice();
  void homing();
  void pushVel(double vel);
  void sendVel();
  void sendSDO();
  inline bool sendVelQueueEmpty() { return(sendVel_queue_.empty()); }
 private:
  uint8_t devID_;
  std::string devName_;
  double latestVelSent_; // most recent velocity sent
  std::queue<double> sendVel_queue_;
  
  std::queue<TPCANMsg> sendSDO_queue_;
  bool waiting_for_SDO_reply_; // not implemented yet (maybe we don't need it at all)
};




#endif

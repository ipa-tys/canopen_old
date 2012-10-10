// Copyright (c) 2012 Fraunhofer Institute
// for Manufacturing Engineering and Automation (IPA)
// See the file license.txt for copying permission.

#ifndef CANOPENMASTER_H
#define CANOPENMASTER_H
#include "chain.h"


namespace canopen {

  void initMasterThread();
  void masterFunc();

  void initIncomingPDOProcessorThread();
  void incomingPDOProcessorFunc();

  /*
  inline void initCallback(std::string chainName,
			   std::chrono::milliseconds sync_deltaT_msec) {
    chainMap[chainName]->chainInit(sync_deltaT_msec); }

  inline void homingCallback(std::string chainName) { chainMap[chainName]->chainHoming(); }
  inline void IPmodeCallback(std::string chainName) { chainMap[chainName]->chainIPmode(); }

  inline void setPosCallback(std::string chainName, std::vector<double> positions) {
    chainMap[chainName]->setPos(positions); }

  inline void setVelCallback(std::string chainName, std::vector<double> velocities) {
    chainMap[chainName]->setVel(velocities); }

  inline std::vector<double> getActualPosCallback(std::string chainName) {
    return chainMap[chainName]->getActualPos(); }

  inline std::vector<double> getDesiredPosCallback(std::string chainName) {
    return chainMap[chainName]->getDesiredPos(); }

  inline std::vector<double> getActualVelCallback(std::string chainName) {
    return chainMap[chainName]->getActualVel(); }
  
  inline std::vector<double> getDesiredVelCallback(std::string chainName) {
    return chainMap[chainName]->getDesiredVel(); }
  */
    

  // void jointVelocitiesCallback(std::string chainName, std::vector<int> velocities);
  /* void jointVelocitiesCallback(std::string chainName, std::vector<int> velocities) {
    std::cout << "jointvelocitiescallback: " << chainName << std::endl;
    // chainMap[chainName]->setPos(positions);  todo!
    }  */


}

#endif


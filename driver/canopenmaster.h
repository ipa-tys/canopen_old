#ifndef CANOPENMASTER_H
#define CANOPENMASTER_H
#include "chain.h"


namespace canopen {
  extern std::map<std::string, Chain*> chainMap;

  void initChainMap(std::vector<ChainDescription> chainDesc);
  void initMasterThread();
  void masterFunc();
  void initIncomingPDOProcessorThread();
  void incomingPDOProcessorFunc();

  inline void initCallback(std::string chainName) { chainMap[chainName]->chainInit(); }
  inline void homingCallback(std::string chainName) { chainMap[chainName]->chainHoming(); }
  inline void IPmodeCallback(std::string chainName) { chainMap[chainName]->chainIPmode(); }

  inline void setPosCallback(std::string chainName, std::vector<double> positions) {
    chainMap[chainName]->setPos(positions); }

  inline void setVelCallback(std::string chainName, std::vector<double> velocities) {
    chainMap[chainName]->setVel(velocities); }

  inline std::vector<double> getActualPosCallback(std::string chainName) {
    return chainMap[chainName]->getActualPos(); }

  // void jointVelocitiesCallback(std::string chainName, std::vector<int> velocities);
  /* void jointVelocitiesCallback(std::string chainName, std::vector<int> velocities) {
    std::cout << "jointvelocitiescallback: " << chainName << std::endl;
    // chainMap[chainName]->setPos(positions);  todo!
    }  */


}

#endif


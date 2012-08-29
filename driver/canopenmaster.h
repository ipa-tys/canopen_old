#ifndef CANOPENMASTER_H
#define CANOPENMASTER_H
#include "chain.h"


namespace canopen {
  void initChainMap(std::vector<ChainDescription> chainDesc);
  void initMasterThread();
  void masterFunc();
  void initIncomingPDOProcessorThread();
  void incomingPDOProcessorFunc();

  void initCallback(std::string chainName);
  void homingCallback(std::string chainName);
  void IPmodeCallback(std::string chainName);

  void setPosCallback(std::string chainName, std::vector<int> positions);
  std::vector<int> getCurrentPosCallback(std::string chainName);

  extern std::map<std::string, Chain*> chainMap;
}

#endif


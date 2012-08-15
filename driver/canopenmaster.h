#ifndef CANOPENMASTER_H
#define CANOPENMASTER_H

#include "chain.h"

namespace canopen {

  void initChainMap(std::string robotDescFilename);
  void initMasterThread();
  void initIncomingPDOProcessorThread();

  void initCallback(std::string chainName);
  void homingCallback(std::string chainName);
  void IPmodeCallback(std::string chainName);

  void setPosCallback(std::string chainName, std::vector<int> positions);
  std::vector<int> getCurrentPosCallback(std::string chainName);

}

#endif


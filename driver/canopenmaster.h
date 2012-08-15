#ifndef CANOPENMASTER_H
#define CANOPENMASTER_H

#include "chain.h"

namespace canopen {

  void initChainMap(std::string robotDescFilename);
  void initMasterThread();
  void initIncomingPDOProcessorThread();

  void homingCallback(std::string chainName);
  void initCallback(std::string chainName);
  void setPosCallback(std::string chainName, std::vector<int> positions);
  

}

#endif


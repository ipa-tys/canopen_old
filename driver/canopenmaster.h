#ifndef CANOPENMASTER_H
#define CANOPENMASTER_H

#include "chain.h"

namespace canopen {

  void initChainMap(std::string robotDescFilename);
  void initMasterThread();

  void homingCallback(std::string chainName);
  void initCallback(std::string chainName);

}

#endif


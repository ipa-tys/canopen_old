#ifndef CHAIN_DESCRIPTION_H
#define CHAIN_DESCRIPTION_H

#include <string>
#include <vector>
#include <fstream>
#include "yaml-cpp/yaml.h"

namespace canopen {

  struct DeviceDescription {
    std::string name;
    int id;
    std::string bus;
  };

  struct ChainDescription {
    std::string name;
    std::vector<DeviceDescription> devices;
  };

  void operator>> (const YAML::Node& node, DeviceDescription& d);
  void operator>> (const YAML::Node& node, ChainDescription& c);
  std::vector<ChainDescription> parseChainDescription(std::string filename);

}
#endif

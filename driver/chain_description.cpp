#include "chain_description.h"

namespace canopen {

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

}

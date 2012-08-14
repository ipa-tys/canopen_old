#include <iostream>
#include <fstream>
#include <pwd.h>
#include <chain.h> // todo: rename to canopen_chain.h

namespace canopen {

  std::map<std::string, Chain*> chainMap; // todo: Chain instead of Chain*?

  void initRobot(std::string robotDescFilename) {

    // parse description file: // todo: make this more flexible: YAML etc.
    std::ifstream fin(robotDescFilename);
    std::string ll;

    std::vector<std::string> all_chainNames;
    std::vector< std::vector<std::string> > all_deviceNames;
    std::vector< std::vector<std::string> > all_CANbuses;
    std::vector< std::vector<uint16_t> > all_CANids;

    while (std::getline(fin, ll)) {
      std::istringstream x(ll);
      std::string chainName;
      std::vector<std::string> deviceNames;
      std::vector<std::string> CANbuses;
      std::vector<uint16_t> CANids;
      std::string temp;
      x >> chainName;
      while (x >> temp) {
	deviceNames.push_back(temp);
	x >> temp;
	CANbuses.push_back(temp);
	x >> temp;
	CANids.push_back(std::stoi(temp));
      }

      all_chainNames.push_back(chainName);
      all_deviceNames.push_back(deviceNames);
      all_CANbuses.push_back(CANbuses);
      all_CANids.push_back(CANids);
    }

    // construct chainMap:
    for (int i=0; i<all_chainNames.size(); i++) {
      chainMap[all_chainNames[i]] = new Chain(all_chainNames[i], all_deviceNames[i],
					      all_CANbuses[i], all_CANids[i]);
    }
  }
}


int main() {
  
  canopen::initRobot("/home/tys/git/other/canopen/driver/robot1.csv");
  std::cout << canopen::chainMap["chain1"]->alias_ << std::endl;
  for (auto it : canopen::chainMap["chain1"]->devices_) {
    std::cout << it.alias_ << "  " << it.CANbus_ << "  " << it.CANid_ << std::endl;
  }
  return 0;
}

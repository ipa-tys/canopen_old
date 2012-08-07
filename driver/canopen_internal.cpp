#include "canopen_internal.h"

namespace canopen {


  // Constants
  uint64_t Constants::getMask(std::string name) {
    typedef ConstantContainer::nth_index<1>::type ConstantContainer_by_name;
    ConstantContainer_by_name::iterator it=constants_.get<1>().find(name);
    return it->mask_;
  }

  uint64_t Constants::getValue(std::string name) {
    typedef ConstantContainer::nth_index<1>::type ConstantContainer_by_name;
    ConstantContainer_by_name::iterator it=constants_.get<1>().find(name);
    return it->value_;
  }

  std::string Constants::getName(uint64_t mask, uint64_t value) {
    return "test";
  }

  void Constants::insert(std::string name, uint64_t mask, uint64_t value) {
    constants_.insert(Constant(name, mask, value));
  }



} 

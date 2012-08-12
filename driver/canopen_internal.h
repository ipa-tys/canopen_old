#ifndef CANOPENINTERNAL_H
#define CANOPENINTERNAL_H

#include <boost/bimap.hpp>
#include <string>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <fcntl.h>    // for O_RDWR
#include <libpcan.h>
#include <vector>
#include <stdint.h>
#include <inttypes.h>
#include <thread>

namespace canopen {
  extern HANDLE h;
  
  namespace bmi = boost::multi_index;
  typedef boost::bimap<std::string, uint32_t> bmtype;

  struct Constant {
  Constant(std::string constName, uint64_t mask, uint64_t value):
    constName_(constName), mask_(mask), value_(value) {};
    
    std::string constName_;
    uint64_t mask_;
    uint64_t value_;
  };
  
  struct mask_value_key:bmi::composite_key<
    Constant,
    BOOST_MULTI_INDEX_MEMBER(Constant, uint64_t, mask_),
    BOOST_MULTI_INDEX_MEMBER(Constant, uint64_t, value_)
    >{};

  typedef bmi::multi_index_container<Constant,
    bmi::indexed_by<
    bmi::ordered_unique<mask_value_key>,
    bmi::ordered_unique<bmi::member<Constant, std::string, &Constant::constName_> >
    >
    > ConstantContainer;

  class Constants {
  public:
    Constants() {};
    uint64_t getMask(std::string name);
    uint64_t getValue(std::string name);
    std::string getName(uint64_t mask, uint64_t value);
    void insert(std::string name, uint64_t mask, uint64_t value);
  private:
    ConstantContainer constants_;
  };


  class EDSClass {
  public:
    std::string alias_;
    uint16_t index_;
    uint8_t subindex_;
    uint8_t length_;
    std::string attr_;
    std::vector<uint64_t> masks_;  // todo:
    Constants constants_;

    EDSClass(std::string alias, uint16_t index, uint8_t subindex,
	   uint8_t length, std::string attr):
    alias_(alias), index_(index), subindex_(subindex),
      length_(length), attr_(attr) {}
  };

  struct index_subindex_key : bmi::composite_key<
    EDSClass,
    BOOST_MULTI_INDEX_MEMBER(EDSClass, uint16_t, index_),
    BOOST_MULTI_INDEX_MEMBER(EDSClass, uint8_t, subindex_)
    >{};

  struct byAlias;
  struct byIndex;
  struct byCobID;
  typedef bmi::multi_index_container<EDSClass,
    bmi::indexed_by<
    bmi::ordered_unique<index_subindex_key>,
    bmi::ordered_unique<bmi::tag<byAlias>, bmi::member<EDSClass, std::string, &EDSClass::alias_> >
    >
    > EDSClassSet;


  struct PDOClass { 
    std::string alias_;
    uint16_t cobID_;
    std::vector<std::string> components_; // entries into EDSDict

  PDOClass(std::string alias, uint16_t cobID, std::vector<std::string> components):
    alias_(alias), cobID_(cobID), components_(components) {}
  };
  typedef bmi::multi_index_container<PDOClass,
    bmi::indexed_by<
    bmi::ordered_unique<bmi::tag<byAlias>, bmi::member<PDOClass, std::string, &PDOClass::alias_> >,
    bmi::ordered_unique<bmi::tag<byCobID>, bmi::member<PDOClass, std::uint16_t, &PDOClass::cobID_> >
    >
    > PDOClassSet;

}

#endif

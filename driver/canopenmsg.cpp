#include "canopenmsg.h"

namespace canopen {
  // -------------- namespace-global variables:
  bool using_master_thread = false;
  std::queue<Message> outgoingMsgQueue; // only used if using_master_thread==true

  HANDLE h;
  EDSDict eds;
  PDODict pdo;
  std::map<std::string, Message* > pendingSDOReplies; // todo: make thread-safe

  bool queue_incoming_PDOs = true; // todo: implement its use; only queue PDOs when true
  std::queue<Message> incomingPDOs; // todo: make thread_safe

  // -------------- EDSDict member functions:

  EDSDict::EDSDict() { // constructor parses indices.csv, constants.csv, PDOs.csv
    int uid = getuid();
    passwd * pw = getpwuid(uid);
    std::string homeDir = pw->pw_dir;
    std::ifstream fin;
    std::string ll;

    typedef struct {
      std::string constname;
      uint64_t mask;
      uint64_t value;
    } consts;

    // parse constants:
    std::multimap<std::string, consts > constants;
    std::string constFileName = homeDir + "/.canopen/constants.csv";
    fin.open(constFileName);
    while (std::getline(fin, ll)) {
      std::istringstream x(ll);
      std::string alias;
      consts c;
      x >> alias;
      x >> c.constname;
      x >> std::hex >> c.mask;
      x >> std::hex >> c.value;
      constants.insert(std::make_pair(alias, c));
    }
    fin.close();
    
    // parse indices into eds data structure:
    std::string indexFileName = homeDir + "/.canopen/indices.csv";
    fin.open(indexFileName);
    while(std::getline(fin, ll)) {
      std::istringstream x(ll); 
      std::string alias;
      uint16_t index;
      uint16_t subindex; // had to use 16 instead of 8 here for >> to work
      uint16_t length; // had to use 16 instead of 8 here for >> to work
      std::string attr;
      x >> alias;
      x >> std::hex >> index;
      x >> std::hex >> subindex;
      x >> length;
      x >> attr;
      // std::cout << "index: " << index << std::endl;
      // std::cout << "LENGTH: " << length << std::endl;
      // std::cout << "ATTR: " << attr << std::endl;
      EDSClass* xx = new EDSClass(alias, index, subindex, length, attr);
      
      // now insert constants:
      auto it = constants.find(alias);
      if (it != constants.end()) {
	auto ret = constants.equal_range(alias);
	for (auto it1=ret.first; it1!=ret.second; it1++)
	  xx->constants_.insert(it1->second.constname, it1->second.mask, it1->second.value);
      }

      d_.insert(*xx);
    }
  }

  uint32_t EDSDict::getConst(std::string alias, std::string constname) {
    typedef EDSClassSet::nth_index<1>::type EDSClassSet_by_alias;
    EDSClassSet_by_alias::iterator it=d_.get<1>().find(alias);
    Constants cc = it->constants_;
    return cc.getValue(constname);
  }

  uint32_t EDSDict::getMask(std::string alias, std::string constname) {
    typedef EDSClassSet::nth_index<1>::type EDSClassSet_by_alias;
    EDSClassSet_by_alias::iterator it=d_.get<1>().find(alias);
    Constants cc = it->constants_;
    return cc.getMask(constname);
  }

  uint8_t EDSDict::getLen(std::string alias) {
    bmtype bm;
    typedef EDSClassSet::nth_index<1>::type EDSClassSet_by_alias;
    EDSClassSet_by_alias::iterator it=d_.get<1>().find(alias);
    return it->length_;
  }

  uint16_t EDSDict::getIndex(std::string alias) {
    bmtype bm;
    typedef EDSClassSet::nth_index<1>::type EDSClassSet_by_alias;
    EDSClassSet_by_alias::iterator it=d_.get<1>().find(alias);
    return it->index_;
  }

  uint8_t EDSDict::getSubindex(std::string alias) {
    bmtype bm;
    typedef EDSClassSet::nth_index<1>::type EDSClassSet_by_alias;
    EDSClassSet_by_alias::iterator it=d_.get<1>().find(alias);
    return it->subindex_;
  }

  std::string EDSDict::getAlias(uint16_t index, uint8_t subindex) {
    EDSClassSet::iterator it = d_.find(boost::make_tuple(index, subindex));
    return it->alias_;
  }

  // -------------- PDODict member functions:

  PDODict::PDODict() { // todo: assume all devices have same tPDO mapping -- ok?
    int uid = getuid();
    passwd * pw = getpwuid(uid);
    std::string homeDir = pw->pw_dir;
    std::ifstream fin;
    std::string ll;
    std::string PDOfileName = homeDir + "/.canopen/PDOs.csv";
    fin.open(PDOfileName);
    
    while(std::getline(fin, ll)) {
      std::istringstream x(ll); 
      std::string alias;
      uint16_t index;
      std::string temp;
      std::vector<std::string> components;
      x >> alias;
      x >> std::hex >> index;
      while (x >> temp) 
	components.push_back(temp);
      PDOClass* xx = new PDOClass(alias, index, components);
      d_.insert(*xx);
    }
  }

  std::vector<std::string> PDODict::getComponents(std::string alias) {
    typedef PDOClassSet::nth_index<0>::type PDOClassSet_by_alias;
    PDOClassSet_by_alias::iterator it=d_.get<0>().find(alias);
    return it->components_;
  }

  uint16_t PDODict::getCobID(std::string alias) {
    typedef PDOClassSet::nth_index<0>::type PDOClassSet_by_alias;
    PDOClassSet_by_alias::iterator it=d_.get<0>().find(alias);
    return it->cobID_;
  }

  std::string PDODict::getAlias(uint16_t cobID) { // only used for incoming PDOs
    uint16_t index;
    std::vector<uint16_t> indices = {0x180, 0x280, 0x380, 0x480};
    int i=0;
    while (i<3 && indices[i+1] < cobID) i++;
    index = indices[i];
    typedef PDOClassSet::nth_index<1>::type PDOClassSet_by_cobID;
    PDOClassSet_by_cobID::iterator it=d_.get<1>().find(index); 
    return it->alias_;
  }

  // -------------- Message member functions:

  // user-constructed non-PDO message:
  Message::Message(uint8_t nodeID, std::string alias, uint32_t value):
    nodeID_(nodeID), alias_(alias) {
    values_.push_back(value);
  }

  // user-constructed PDO message:
  Message::Message(uint8_t nodeID, std::string alias, std::vector<uint32_t> values):
    nodeID_(nodeID), alias_(alias), values_(values) {}

  // constructor for messages coming in from bus (TPCANRdMsg):
  Message::Message(TPCANRdMsg m) {  
    if (m.Msg.ID >= 0x180 && m.Msg.ID <= 0x4ff) { // incoming PDOs
      alias_ = pdo.getAlias(m.Msg.ID);

      std::vector<uint16_t> indices = {0x180, 0x280, 0x380, 0x480};
      int i=0;
      while (i<3 && indices[i+1] < m.Msg.ID) i++;
      nodeID_ = m.Msg.ID - indices[i];

      std::vector<std::string> components = pdo.getComponents(alias_);

      std::vector<uint32_t> lengths;
      for (auto comp:components) lengths.push_back(eds.getLen(comp));
      int pos = 0;
      for (int i=0; i<components.size(); i++) {
	values_.push_back(0);
	for (int j=0; j<lengths[i]; j++) {
	  values_[i] += (static_cast<uint32_t>(m.Msg.DATA[pos]) << (j*8));
	  pos++;
	}
      }
      incomingPDOs.push(*this);
      std::cout << "incomingPDOs queue size: " << incomingPDOs.size() << std::endl;

    } else if (m.Msg.ID >= 0x580 && m.Msg.ID <= 0x5ff) { // SDO replies
      uint16_t index = m.Msg.DATA[1] + (m.Msg.DATA[2]<<8);
      uint8_t subindex = m.Msg.DATA[3];
      nodeID_ = m.Msg.ID - 0x580;
      alias_ =  eds.getAlias(index, subindex);
      values_.push_back(m.Msg.DATA[4] + (m.Msg.DATA[5]<<8) + (m.Msg.DATA[6]<<8) + (m.Msg.DATA[7]<<8));
      std::string ss = createMsgHash();
      pendingSDOReplies[ss] = this;
    }
  }

  std::string Message::createMsgHash(TPCANMsg m) { 
    std::string ss = std::to_string(nodeID_) + "_" +
      std::to_string(m.DATA[1] + (m.DATA[2]<<8)) + "_" +
      std::to_string(m.DATA[3]);
    return ss;
  }

  std::string Message::createMsgHash() { // COBID_index_subindex (decimal)
    uint16_t index = eds.getIndex(alias_);
    uint8_t subindex = eds.getSubindex(alias_);
    std::string ss = std::to_string(nodeID_) + "_" +
      std::to_string(index) + "_" +
      std::to_string(subindex);
    return ss;
  }

  bool Message::checkForConstant(std::string constName) {
    uint32_t value = values_[0];
    uint32_t constValue = eds.getConst(alias_, constName);
    uint32_t mask = eds.getMask(alias_, constName);
    std::cout << "constValue: " << constValue << std::endl; // todo: remove output
    std::cout << "constMask: " << mask << std::endl;
    std::cout << "value: " << value << std::endl;
    std::cout << "value & constMask: " << (value & mask) << std::endl;
    return (value & mask) == constValue;
  }

  void Message::writeCAN(bool writeMode, bool directlyToCanBus) {
    if (using_master_thread && !directlyToCanBus && alias_ != "Sync") {
      // don't write to CAN bus, just put in queue
      // note: SYNC messages are never queued
      outgoingMsgQueue.push(*this);
    } else {
      TPCANMsg msg;
      for (int i=0; i<8; i++) msg.DATA[i]=0;

      if (values_.size() > 1) {  // PDO
	std::vector<std::string> components = pdo.getComponents(alias_);
	std::vector<uint32_t> lengths;
	for (auto comp:components) lengths.push_back(eds.getLen(comp));
	int pos = 0;
	for (int i=0; i<components.size(); i++) {
	  for (int j=0; j<lengths[i]; j++) {
	    msg.DATA[pos] = static_cast<uint8_t>(values_[i] >> (j*8) & 0xFF);
	    pos++;
	  }
	}
	msg.ID = pdo.getCobID(alias_) + nodeID_;
	msg.MSGTYPE = 0x00;
	msg.LEN = 8;
	CAN_Write(h, &msg);
    
	printf("%02x %d %d\n", msg.ID, msg.MSGTYPE, msg.LEN);
	for (int i=0; i<8; i++) printf("%02x ", msg.DATA[i]);
	printf("\n");
      } else if (alias_ == "NMT") {
	std::cout << "hi, NMT" << std::endl;
	msg.ID = 0;
	msg.MSGTYPE = 0x00;  // standard message
	msg.LEN = 2;
	msg.DATA[0] = values_[0];
	msg.DATA[1] = nodeID_;
	CAN_Write(h, &msg);
      
      } else if (alias_ == "Sync") {
	msg.ID = 0x80;
	msg.MSGTYPE = 0x00;
	msg.LEN = 0;
	CAN_Write(h, &msg);
    
      } else { // SDO
	std::cout << "hi, SDO" << std::endl;
	msg.ID = 0x600 + nodeID_;
	msg.MSGTYPE = 0x00;
	uint8_t len = eds.getLen(alias_);
	std::cout << "LEN: " << static_cast<int>(len) << std::endl;
	if (writeMode == true) {
	  msg.LEN = 4 + len; // 0x2F(or 0x2b or 0x23) / index / subindex / actual data
	  if (len==1) {
	    msg.DATA[0] = 0x2F;
	  } else if (len==2) {
	    msg.DATA[0] = 0x2B;
	  } else { // len==4
	    msg.DATA[0] = 0x23;
	  }
	} else { // writeMode==false
	  msg.LEN = 4; // only 0x40 / index / subindex
	  msg.DATA[0] = 0x40;
	}
	uint16_t index = eds.getIndex(alias_);
    
	msg.DATA[1] = static_cast<uint8_t>(index & 0xFF);
	msg.DATA[2] = static_cast<uint8_t>((index >> 8) & 0xFF);
	msg.DATA[3] = eds.getSubindex(alias_);
	if (writeMode == true) {
	  uint32_t v = values_[0];
	  for (int i=0; i<len; i++) msg.DATA[4+i] = static_cast<uint8_t>( (v >> (8*i)) & 0xFF );
	} 

	// put on multiset
	std::string ss = createMsgHash(msg);
	pendingSDOReplies.insert(std::make_pair(ss, nullptr));
	std::cout << "Message hash: " << ss << std::endl;

	CAN_Write(h, &msg);
      }  // end SDO

    }
  }

  void debug_show_pendingSDOReplies() {
    std::cout << "DEBUG. Pending_queue_size = " << pendingSDOReplies.size() << std::endl;
    
    for (auto it : pendingSDOReplies) {
      std::cout << it.first;
      if (it.second == nullptr) std::cout << "(nullptr)";
      std::cout << ", ";
    }
    std::cout << std::endl;
  }

  void Message::debugPrint() {
    std::cout << "MESSAGE debugPrint:" << std::endl;
    std::cout << "nodeID: " << static_cast<int>(nodeID_) << ", alias: " << alias_ 
	      << ", value: " << values_[0] << std::endl;
  }

  Message* Message::readCAN(bool blocking) { // todo: different blocking modes
    TPCANRdMsg m;
    if ((errno = LINUX_CAN_Read(canopen::h, &m))) {
      perror("receivetest: LINUX_CAN_Read()");
      // return errno;
    }
    Message* msg = new Message(m);
    return msg;
  }

  Message* Message::waitForSDOAnswer() { 
    std::string ss = createMsgHash();
    while (pendingSDOReplies[ss] == nullptr)
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    Message* m = pendingSDOReplies[ss];
    pendingSDOReplies.erase(ss);
    // delete m;  todo
    return m;
  } 

  // ------------- wrapper functions for sending SDO, PDO, and NMT messages: --

  Message* sendSDO(uint16_t deviceID, std::string alias,
		   std::string param, bool writeMode) {
    Message* m;
    if (param != "") // for SDOs that don't take parameter (e.g. statusword)
      m = new Message(deviceID, alias, eds.getConst(alias, param));
    else 
      m = new Message(deviceID, alias);
    m->writeCAN(writeMode);
    Message* reply = m->waitForSDOAnswer();
    delete m;
    return reply;
  }

  void sendNMT(std::string param) {
    Message(0, "NMT", eds.getConst("NMT", param)).writeCAN();
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
  }
}

// todo: PDOs
// todo: sleep_for in writeCAN instead of in functions

#include <queue>
#include <map>
#include <inttypes.h>
#include <iostream>

namespace canopen {

  typedef struct {
    std::vector<std::string 
  } chaindesc_type;

  std::map<std::string, Chain*> chainMap;

  void masterFunc() {
    std::map<std::string, Chain*> chains;
    
    std::vector<std::thread> SDOthreadPool;
    // SDO calls block until return or timeout, hence the thread pool
    // todo: currently, SDO sending is directly to bus

    // todo: init chains from csv files; for now chain init is hard-coded:
    

  }

  void initMasterThread() {
    using_master_thread = true;
    std::thread listener_thread(masterFunc);
    master_thread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }


  
  // std::queue<std::vector<uint32_t> > outgoingPosQueue;
  // std::queue<std::vector<uint32_t> > incomingPosQueue;


}


/*
int main() {
  
  std::vector<uint32_t> x {1,2,3};
  std::vector<uint32_t> y {1,2,3,4};

  outgoingPosQueue.push(x);
  outgoingPosQueue.push(y);


  while (outgoingPosQueue.size() > 0) {
    std::cout << "hi" << std::endl;
    std::vector<uint32_t> z = outgoingPosQueue.front();
    outgoingPosQueue.pop();
    std::cout << z.size() << std::endl;

    

  }

}
}
*/

#include <queue>
#include <inttypes.h>
#include <iostream>

namespace canopen {

  void masterFunc() {


  }

  void initMasterThread() {
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

#include <thread>
#include <chrono>
#include <iostream>
#include <canopenmsg.h>

int main() {
    if (!canopen::openConnection("/dev/pcan32")) {
    std::cout << "Cannot open CAN device" << std::endl;
    return -1;
    } 
  canopen::Message* SDOreply1; // SDO replies will be stored here 
  std::cout << "hi" << std::endl;
  return 0;
}

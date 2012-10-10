add_executable(homing homing.cpp)
# SET_TARGET_PROPERTIES(homing PROPERTIES LINK_FLAGS -fPIC)
target_link_libraries(homing pthread canopen pcan)
#  gcov yaml-cpp)
# target_link_libraries(homing canopen)

add_executable(move_single_device move_single_device.cpp)
target_link_libraries(move_single_device pthread canopen pcan yaml-cpp)

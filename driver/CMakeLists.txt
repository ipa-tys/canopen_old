
ADD_LIBRARY( canopen STATIC
  canopen_internal.cpp
  canopenmsg.cpp
  canopen_highlevel.cpp
  chain_description.cpp
  chain.cpp
  listener.cpp
  canopenmaster.cpp
  )

install (TARGETS canopen 
#  LIBRARY DESTINATION lib)
  ARCHIVE DESTINATION lib)

install (FILES 
  canopen_internal.h
  canopenmsg.h
  canopen_highlevel.h
  chain_description.h
  chain.h
  listener.h
  canopenmaster.h
  DESTINATION include)

install (CODE "FILE(MAKE_DIRECTORY ~/.canopen)")
install (FILES
  constants.csv
  indices.csv
  PDOs.csv
  DESTINATION ~/.canopen)

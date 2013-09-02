

set(COMMON_HEADERS Camera.h RefCounting.h)

set(COMMON_SOURCE Camera.cpp RefCounting.cpp)
    
# build the game engine
add_library(common SHARED ${COMMON_HEADERS} ${COMMON_SOURCE})
add_definitions(-DCOMMON_EXPORT)
  
  
  
  
  

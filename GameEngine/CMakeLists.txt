

set(ENGINE_HEADERS Camera.h RefCounting.h Timer.h)

set(ENGINE_SOURCE Camera.cpp RefCounting.cpp Timer.cpp)
    
# build the game engine
add_library(GameEngine STATIC ${ENGINE_HEADERS} ${ENGINE_SOURCE})
  
  
  
  
  

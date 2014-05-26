

set(MEMORY_MANAGER_HEADERS BNew.h Heap.h MemoryPool.h)

set(MEMORY_MANAGER_SOURCE BNew.cpp Heap.cpp MemoryPool.cpp)
    
# build the game engine
add_library(MemoryManager STATIC ${MEMORY_MANAGER_HEADERS} ${MEMORY_MANAGER_SOURCE})
  
  
  
  
  
  

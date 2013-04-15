cmake_minimum_required (VERSION 2.8)
project (GameEngine CXX)

# The version number.
set (ENGINE_VERSION_MAJOR 1)
set (ENGINE_VERSION_MINOR 0)

set(ENGINE_SOURCE
        GameEngine/asVM.cpp
        GameEngine/BNew.cpp
        GameEngine/Heap.cpp
        GameEngine/MemoryPool.cpp
        GameEngine/Camera.cpp
        GameEngine/QuadTree.cpp
        GameEngine/RefCounting.cpp
        GameEngine/VecMath.cpp
        GameEngine/PluginManager.cpp
        GameEngine/Game.cpp
        GameEngine/StringAlgorithms.cpp
        GameEngine/GameStateMachine.cpp
        GameEngine/GameStateScript.cpp
        )
        
set(ENGINE_HEADERS
       GameEngine/asVM.h
       GameEngine/BNew.h
       GameEngine/Heap.h
       GameEngine/MemoryPool.h
       GameEngine/Singleton.h
       GameEngine/Camera.h
       GameEngine/QuadTree.h
       GameEngine/RefCounting.h
       GameEngine/Delegates.h
       GameEngine/VecMath.h
       GameEngine/PluginManager.h
       GameEngine/Game.h
       GameEngine/StringAlgorithms.h
       GameEngine/GameStateMachine.h
       GameEngine/GameStateScript.h
    )

set(TEST_SOURCE
    ../testApp/main.cpp
    )
# testApp
  
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
#include_directories(${PROJECT_BINARY_DIR})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}

include_directories("${CMAKE_SOURCE_DIR}")
#include_directories("${CMAKE_SOURCE_DIR}/../testApp")

add_definitions( "-Wall -std=c++0x -fno-rtti" )

add_library(GameEngine STATIC ${ENGINE_HEADERS} ${ENGINE_SOURCE})
#add_executable(Test ${TEST_SOURCE} ${TEST_HEADERS})

target_link_libraries(GameEngine dl)
target_link_libraries(GameEngine glfw)
#target_link_libraries(Test GameEngine)

#target_link_libraries(Test AngelScript)

#target_link_libraries (Tutorial test)

cmake_minimum_required (VERSION 2.8)
project (GameEngine CXX)

# The version number.
set (ENGINE_VERSION_MAJOR 1)
set (ENGINE_VERSION_MINOR 0)

if( MSVC )
    if(NOT ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11")
        message( SEND_ERROR "Visual Studio 11 is needed to compile the code" )
    endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules")
include("${CMAKE_SOURCE_DIR}/CMake/CompileFlags.cmake")

option(BUILD_MEMORY_MANAGER "Build Memory Manager" OFF)
option(BUILD_GUI "Build GUI" OFF)

find_package (GLEW)
if (GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIR})
endif (GLEW_FOUND)

find_package (GLFW)
if (GLFW_FOUND)
  include_directories(${GLFW_INCLUDE_DIR})
endif(GLFW_FOUND)

find_package (STB_IMAGE)
if (STB_IMAGE_FOUND)
    include_directories(${STB_IMAGE_INCLUDE_DIR})
endif (STB_IMAGE_FOUND)

find_package (GLM)
if (GLM_FOUND)
  include_directories(${GLM_INCLUDE_DIR})
endif (GLM_FOUND)

find_package (ANGELSCRIPT)
if (ANGELSCRIPT_FOUND)
  include_directories(${ANGELSCRIPT_INCLUDE_DIR})
  include_directories(${ANGELSCRIPT_ADDON_INCLUDE_DIR})
endif (ANGELSCRIPT_FOUND)

include_directories("${CMAKE_SOURCE_DIR}/GameEngine")
include_directories("${CMAKE_SOURCE_DIR}/GameLauncher/interfaces")
include_directories("${CMAKE_SOURCE_DIR}/GameLauncher")

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)

if(MSVC)
    # visual studio compile flags

    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /arch:SSE2 /RTC1 /W3 /MP /GR-")
    set(CMAKE_CXX_FLAGS_RELEASE "/W3 /MP /O2 /arch:SSE2 /GR- /MD")
else()

    #MinGW and g++ compile flags
    set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -fno-rtti -pipe")
    set(CMAKE_CXX_FLAGS_DEBUG "-Wall -std=c++11 -g -fno-rtti -pipe")
    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -std=c++11 -O3 -fno-rtti -pipe")

    if(UNIX)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC")

    endif(UNIX)

endif()

if(BUILD_MEMORY_MANAGER)
    add_subdirectory(MemoryManager)
endif(BUILD_MEMORY_MANAGER)

if(BUILD_GUI)
    add_subdirectory(GUI)
endif(BUILD_GUI)

add_subdirectory(GameEngine)
add_subdirectory(OpenGLRenderer)
add_subdirectory(DXInput)
add_subdirectory(AdventureGame)
#add_subdirectory(MineSweeper)
#add_subdirectory(FlockingAlgorithm)

set(GAME_LAUNCHER_HEADERS
        GameLauncher/asVM.h
        GameLauncher/FileManager.h
        GameLauncher/Game.h
        GameLauncher/GameConstants.h
        GameLauncher/GameStateMachine.h
        #GameStateScript.h
        GameLauncher/interfaces/IGameState.h
        GameLauncher/interfaces/IGameState.h
        #IGrid.h
        GameLauncher/interfaces/IGame.h
        GameLauncher/interfaces/IInput.h
        GameLauncher/interfaces/IKMInput.h
        GameLauncher/interfaces/IPlugin.h
        GameLauncher/interfaces/IRenderer.h
        GameLauncher/interfaces/IResourceManager.h
        GameLauncher/PluginManager.h
        #QuadTree.h
        GameLauncher/RTTI.h
        #SerializedState.h
        GameLauncher/Singleton.h
        #StringAlgorithms.h
        #GameLauncher/Timer.h
        GameLauncher/VecMath.h
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptbuilder/scriptbuilder.h
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptstdstring/scriptstdstring.h
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scripthelper/scripthelper.h
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptmath/scriptmath.h
    )

set(GAME_LAUNCHER_SOURCE
        GameLauncher/main.cpp
        GameLauncher/asVM.cpp
        GameLauncher/FileManager.cpp
        GameLauncher/Game.cpp
        GameLauncher/GameConstants.cpp
        GameLauncher/GameStateMachine.cpp
        #GameStateScript.cpp
        #IGrid.inl
        GameLauncher/PluginManager.cpp
        #QuadTree.cpp
        #SerializedState.cpp
        #StringAlgorithms.cpp
        #GameLauncher/Timer.cpp
        GameLauncher/VecMath.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptbuilder/scriptbuilder.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptstdstring/scriptstdstring.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scripthelper/scripthelper.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptmath/scriptmath.cpp
        )


add_executable(GameLauncher ${GAME_LAUNCHER_HEADERS} ${GAME_LAUNCHER_SOURCE})

target_link_libraries(GameLauncher GameEngine ${GLFW_LIBRARY})
target_link_libraries(GameLauncher ${ANGELSCRIPT_LIBRARY_RELEASE})




cmake_minimum_required (VERSION 2.8)
project (GameEngine CXX)

# The version number.
set (ENGINE_VERSION_MAJOR 1)
set (ENGINE_VERSION_MINOR 0)

set(INPUT_SOURCE
    DXInput/DXInput.cpp
    )
set(INPUT_HEADERS
    DXInput/DXInput.h
    )

set(OPENGL_RENDERER_SOURCE
    OpenGLRenderer/oglRenderer.cpp
    OpenGLRenderer/LineEngine.cpp
    OpenGLRenderer/ResourceManager.cpp
    OpenGLRenderer/FontEngine.cpp
    OpenGLRenderer/SpriteEngine.cpp
    OpenGLRenderer/GenerateBuffers.cpp
    )
set(OPENGL_RENDERER_HEADERS
    OpenGLRenderer/oglRenderer.h
    OpenGLRenderer/LineEngine.h
    OpenGLRenderer/ResourceManager.h
    OpenGLRenderer/FontEngine.h
    OpenGLRenderer/SpriteEngine.h
    OpenGLRenderer/GenerateBuffers.h
    )

set(FLOCKING_ALGORITHM_SOURCE
    FlockingAlgorithm/FlockingAlgorithm.cpp
    )

set(FLOCKING_ALGORITHM_HEADERS
    FlockingAlgorithm/FlockingAlgorithm.h
    )

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
        GameEngine/Timer.cpp
        GameEngine/Menu.cpp
        GameEngine/FileManager.cpp
        GameEngine/GameConstants.cpp
        ../angelscript/scriptbuilder/scriptbuilder.cpp
        ../angelscript/scriptstdstring/scriptstdstring.cpp
        ../angelscript/scripthelper/scripthelper.cpp
        ../angelscript/scriptmath/scriptmath.cpp
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
       GameEngine/Timer.h
       GameEngine/Menu.h
       GameEngine/FileManager.h
       GameEngine/GameConstants.h
       ../angelscript/scriptbuilder/scriptbuilder.h
       ../angelscript/scriptstdstring/scriptstdstring.h
       ../angelscript/scripthelper/scripthelper.h
       ../angelscript/scriptmath/scriptmath.h

    )

set(GAMELAUNCHER_SOURCE
    GameLauncher/main.cpp
    )
# testApp
  
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
#include_directories(${PROJECT_BINARY_DIR})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}

include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/GameEngine/")
include_directories("${CMAKE_SOURCE_DIR}/../")
#include_directories("${CMAKE_SOURCE_DIR}/../testApp")

# build the game engine
set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/../lib/)
add_library(GameEngine STATIC ${ENGINE_HEADERS} ${ENGINE_SOURCE})
target_link_libraries(GameEngine dl)
target_link_libraries(GameEngine glfw)
target_link_libraries(GameEngine angelscript)

# build the plugins
set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/../)
add_library(renderer MODULE ${OPENGL_RENDERER_HEADERS} ${OPENGL_RENDERER_SOURCE})
target_link_libraries(renderer GLEW)
target_link_libraries(renderer GameEngine)

add_library(input MODULE ${INPUT_HEADERS} ${INPUT_SOURCE})

set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/../)
add_library(FlockingAlgorithm MODULE ${FLOCKING_ALGORITHM_HEADERS} ${FLOCKING_ALGORITHM_SOURCE})
target_link_libraries(FlockingAlgorithm GameEngine)


# build the game launcher
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/../)
add_executable(GameLauncher ${GAMELAUNCHER_SOURCE})


target_link_libraries(GameLauncher GameEngine)

if(WIN32)
    if(MSVC)
            set(VISUAL_STUDIO_FLAGS
                    /MP
                    /Od
                    /arch:SSE2
                    /GR-
                    /MTd
                )

            set_target_properties(GameEngine PROPERTIES COMPILE_FLAGS VISUAL_STUDIO_FLAGS)
            set_target_properties(renderer PROPERTIES COMPILE_FLAGS VISUAL_STUDIO_FLAGS)
            set_target_properties(input PROPERTIES COMPILE_FLAGS VISUAL_STUDIO_FLAGS)
            set_target_properties(FlockingAlgorithm PROPERTIES COMPILE_FLAGS VISUAL_STUDIO_FLAGS)
            set_target_properties(GameEngine PROPERTIES COMPILE_FLAGS VISUAL_STUDIO_FLAGS)
    endif(MSVC)
else()
    add_definitions( "-Wall -std=c++0x -fno-rtti -g -fPIC" )
    #add_definitions( "-Wall -std=c++0x -fno-rtti -O2 -fPIC" )
endif()

#target_link_libraries (Tutorial test)

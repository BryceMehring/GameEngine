cmake_minimum_required (VERSION 2.8)
project (GameEngine CXX)

if( MSVC )
	if(NOT ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11")
		message( SEND_ERROR "Visual Studio 11 is needed to compile the code" )
	endif()
endif()

# The version number.
set (ENGINE_VERSION_MAJOR 1)
set (ENGINE_VERSION_MINOR 0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules/")

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

set(ADVENTURE_GAME_SOURCE
	AdventureGame/AdventureGame.cpp
	)

set(ADVENTURE_GAME_HEADERS
	AdventureGame/AdventureGame.h
	)

set(MINE_SWEEPER_HEADERS
	MineSweeper/MineSweeper.cpp
	)

set(MINE_SWEEPER_SOURCE
	MineSweeper/MineSweeper.h
	)
	
find_package (ANGELSCRIPT)
if (ANGELSCRIPT_FOUND)
  include_directories(${ANGELSCRIPT_INCLUDE_DIR})
  include_directories(${ANGELSCRIPT_ADDON_INCLUDE_DIR})
else(ANGELSCRIPT_FOUND)
	message(SEND_ERROR "Angelscript is required")
endif (ANGELSCRIPT_FOUND)

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
        GameEngine/SerializedState.cpp
		GameEngine/IGrid.inl
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptbuilder/scriptbuilder.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptstdstring/scriptstdstring.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scripthelper/scripthelper.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptmath/scriptmath.cpp
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
       GameEngine/SerializedState.h
       GameEngine/RTTI.h
       GameEngine/IGameState.h
       GameEngine/IInput.h
       GameEngine/IKMInput.h
       GameEngine/SerializedState.h
       GameEngine/IGameState.h
       GameEngine/IPlugin.h
       GameEngine/IRenderer.h
       GameEngine/IResourceManager.h
	   GameEngine/IGrid.h
       ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptbuilder/scriptbuilder.h
       ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptstdstring/scriptstdstring.h
       ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scripthelper/scripthelper.h
       ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptmath/scriptmath.h

    )

set(GAMELAUNCHER_SOURCE
    GameLauncher/main.cpp
    )

include_directories("${CMAKE_SOURCE_DIR}/GameEngine/")

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin/)
set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin/)

# build the game engine
add_library(GameEngine STATIC ${ENGINE_HEADERS} ${ENGINE_SOURCE})

# build the plugins
add_library(renderer MODULE ${OPENGL_RENDERER_HEADERS} ${OPENGL_RENDERER_SOURCE})
target_link_libraries(renderer GameEngine)

add_library(input MODULE ${INPUT_HEADERS} ${INPUT_SOURCE})

# build the games

#add_library(FlockingAlgorithm MODULE ${FLOCKING_ALGORITHM_HEADERS} ${FLOCKING_ALGORITHM_SOURCE})
#add_library(AdventureGame MODULE ${ADVENTURE_GAME_HEADERS} ${ADVENTURE_GAME_SOURCE})
add_library(MineSweeper MODULE ${MINE_SWEEPER_HEADERS} ${MINE_SWEEPER_SOURCE})
#target_link_libraries(FlockingAlgorithm GameEngine)
#target_link_libraries(AdventureGame GameEngine)
target_link_libraries(MineSweeper GameEngine)

# build the game launcher

add_executable(GameLauncher ${GAMELAUNCHER_SOURCE})

# link libraries
target_link_libraries(GameLauncher GameEngine)

find_package (GLEW)
if (GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIR})
  target_link_libraries (renderer ${GLEW_LIBRARY})
else(GLEW_FOUND)
	message(SEND_ERROR "GLEW is required")
endif (GLEW_FOUND)

find_package (GLFW)
if (GLFW_FOUND)
  include_directories(${GLFW_INCLUDE_DIR})
  target_link_libraries (renderer ${GLFW_LIBRARY})
  target_link_libraries (input ${GLFW_LIBRARY})
  target_link_libraries (GameEngine ${GLFW_LIBRARY})
else(GLFW_FOUND)
	message(SEND_ERROR "GLFW is required")
endif (GLFW_FOUND)

find_package (STB_IMAGE)
if (STB_IMAGE_FOUND)
	include_directories(${STB_IMAGE_INCLUDE_DIR})
else(STB_IMAGE_FOUND)
	message(SEND_ERROR "stb_image is required")
endif (STB_IMAGE_FOUND)

find_package (GLM)
if (GLM_FOUND)
  include_directories(${GLM_INCLUDE_DIR})
else(GLM_FOUND)
	message(SEND_ERROR "GLM is required")
endif (GLM_FOUND)

if (ANGELSCRIPT_FOUND)
  if(MSVC)
	target_link_libraries (GameEngine debug ${ANGELSCRIPT_LIBRARY_DEBUG})
	target_link_libraries (GameEngine optimized ${ANGELSCRIPT_LIBRARY_RELEASE})
  else(MSVC)
    target_link_libraries (GameEngine ${ANGELSCRIPT_LIBRARY_RELEASE})
	target_link_libraries (GameEngine ${ANGELSCRIPT_LIBRARY_RELEASE})
  endif(MSVC)
endif(ANGELSCRIPT_FOUND)

if(MSVC)

	target_link_libraries(renderer opengl32)
	
	# visual studio compile flags
	set(CMAKE_CXX_FLAGS_DEBUG "/W3 /sdl /MP /Od /arch:SSE2 /GR- /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "/W3 /sdl /MP /O2 /arch:SSE2 /GR- /MD")
else()

	target_link_libraries(GameEngine dl pthread)
	
	# compile flags
	set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -msse2 -fno-rtti -m64 -fPIC")
	set(CMAKE_CXX_FLAGS_DEBUG "-Wall -std=c++0x -msse2 -fno-rtti -m64 -g -fPIC")
	set(CMAKE_CXX_FLAGS_RELEASE "-Wall -std=c++0x -msse2 -O2 -fno-rtti -m64 -fPIC")
endif()

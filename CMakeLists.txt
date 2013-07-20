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
        ../angelscript/add_on/scriptbuilder/scriptbuilder.cpp
        ../angelscript/add_on/scriptstdstring/scriptstdstring.cpp
        ../angelscript/add_on/scripthelper/scripthelper.cpp
        ../angelscript/add_on/scriptmath/scriptmath.cpp
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
       ../angelscript/add_on/scriptbuilder/scriptbuilder.h
       ../angelscript/add_on/scriptstdstring/scriptstdstring.h
       ../angelscript/add_on/scripthelper/scripthelper.h
       ../angelscript/add_on/scriptmath/scriptmath.h

    )

set(GAMELAUNCHER_SOURCE
    GameLauncher/main.cpp
    )

include_directories("${CMAKE_SOURCE_DIR}/../angelscript/angelscript/include")
include_directories("${CMAKE_SOURCE_DIR}/GameEngine/")
include_directories("${CMAKE_SOURCE_DIR}/../glm/")
include_directories("${CMAKE_SOURCE_DIR}/../")

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

if(MSVC)
	target_link_libraries(GameEngine ../glfw/lib-msvc110/glfw3dll)
	target_link_libraries(renderer ../glfw/lib-msvc110/glfw3dll)
	target_link_libraries(input ../glfw/lib-msvc110/glfw3dll)
	target_link_libraries(GameEngine debug ../angelscript/angelscript/lib/angelscriptd) # debug version
	target_link_libraries(GameEngine optimized ../angelscript/angelscript/lib/angelscript) # release version
	
	target_link_libraries(renderer ../glew/lib/glew32)
	target_link_libraries(renderer opengl32)
	
	# visual studio compile flags
	set(VS_DEBUG_COMPILE_FLAGS "/MP /Od /arch:SSE2 /GR- /MDd")
	set(VS_RELEASE_COMPILE_FLAGS "/MP /O2 /arch:SSE2 /GR- /MD")
	
	set_target_properties(GameEngine PROPERTIES COMPILE_FLAGS_DEBUG ${VS_DEBUG_COMPILE_FLAGS})
	set_target_properties(renderer PROPERTIES COMPILE_FLAGS_DEBUG ${VS_DEBUG_COMPILE_FLAGS})
	set_target_properties(input PROPERTIES COMPILE_FLAGS_DEBUG ${VS_DEBUG_COMPILE_FLAGS})
	#set_target_properties(FlockingAlgorithm PROPERTIES COMPILE_FLAGS_DEBUG ${VS_DEBUG_COMPILE_FLAGS})
	#set_target_properties(AdventureGame PROPERTIES COMPILE_FLAGS_DEBUG ${VS_DEBUG_COMPILE_FLAGS})
	set_target_properties(GameLauncher PROPERTIES COMPILE_FLAGS_DEBUG ${VS_DEBUG_COMPILE_FLAGS})
	set_target_properties(MineSweeper PROPERTIES COMPILE_FLAGS_DEBUG ${VS_DEBUG_COMPILE_FLAGS})

	set_target_properties(GameEngine PROPERTIES COMPILE_FLAGS_RELEASE ${VS_RELEASE_COMPILE_FLAGS})
	set_target_properties(renderer PROPERTIES COMPILE_FLAGS_RELEASE ${VS_RELEASE_COMPILE_FLAGS})
	set_target_properties(input PROPERTIES COMPILE_FLAGS_RELEASE ${VS_RELEASE_COMPILE_FLAGS})
	#set_target_properties(FlockingAlgorithm PROPERTIES COMPILE_FLAGS_RELEASE ${VS_RELEASE_COMPILE_FLAGS})
	#set_target_properties(AdventureGame PROPERTIES COMPILE_FLAGS_RELEASE ${VS_RELEASE_COMPILE_FLAGS})
	set_target_properties(GameLauncher PROPERTIES COMPILE_FLAGS_RELEASE ${VS_RELEASE_COMPILE_FLAGS})
	set_target_properties(MineSweeper PROPERTIES COMPILE_FLAGS_RELEASE ${VS_RELEASE_COMPILE_FLAGS})
else()

	# check to make sure that the libraries are installed
	find_package(glfw REQUIRED)
	if(NOT glfw_FOUND)
	    message(SEND_ERROR "GLFW is required")
	endif()
	
	find_package(GLEW REQUIRED)
	if(NOT GLEW_FOUND)
	    message(SEND_ERROR "GLEW is required")
	endif()
	
	find_package(GLM REQUIRED)
	if(NOT GLM_FOUND)
	    message(SEND_ERROR "glm is required")
	endif()
	
	#include_directories(${glfw_INCLUDE_DIRS})
	#set(LIBS ${LIBS} ${glfw_LIBRARIES})

	target_link_libraries(GameEngine dl angelscript pthread angelscript glfw)
	target_link_libraries(renderer GLEW)
	target_link_libraries(input glfw)
	
	# compile flags
	set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -msse2 -fno-rtti -m64 -fPIC")
	set(CMAKE_CXX_FLAGS_DEBUG "-Wall -std=c++0x -msse2 -fno-rtti -m64 -g -fPIC")
	set(CMAKE_CXX_FLAGS_RELEASE "-Wall -std=c++0x -msse2 -O2 -fno-rtti -m64 -fPIC")
endif()

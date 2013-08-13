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

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)

if(MSVC)
    # visual studio compile flags
	
	set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /arch:SSE2 /RTC1 /W3 /MP /GR-")
	set(CMAKE_CXX_FLAGS_RELEASE "/W3 /MP /O2 /arch:SSE2 /GR- /MD")
else()

	#g++ compile flags
	set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -fno-rtti -fPIC -pipe")
	set(CMAKE_CXX_FLAGS_DEBUG "-Wall -std=c++11 -g -fno-rtti -fPIC -pipe")
	set(CMAKE_CXX_FLAGS_RELEASE "-Wall -std=c++11 -O2 -fno-rtti -fPIC -pipe")

endif()
	
add_subdirectory(GameEngine)
add_subdirectory(OpenGLRenderer)
add_subdirectory(DXInput)
add_subdirectory(MineSweeper)
add_subdirectory(GameLauncher)


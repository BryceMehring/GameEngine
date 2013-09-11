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
endif ()

find_package (GLFW)
if (GLFW_FOUND)
  include_directories(${GLFW_INCLUDE_DIR})
endif()

find_package (STB_IMAGE)
if (STB_IMAGE_FOUND)
	include_directories(${STB_IMAGE_INCLUDE_DIR})
endif ()

find_package (GLM)
if (GLM_FOUND)
  include_directories(${GLM_INCLUDE_DIR})
endif ()

find_package (ANGELSCRIPT)
if (ANGELSCRIPT_FOUND)
  include_directories(${ANGELSCRIPT_INCLUDE_DIR})
  include_directories(${ANGELSCRIPT_ADDON_INCLUDE_DIR})
endif ()

include_directories("${CMAKE_SOURCE_DIR}/common")
include_directories("${CMAKE_SOURCE_DIR}/GameEngine/interfaces")
include_directories("${CMAKE_SOURCE_DIR}/GameEngine")

if(WIN32)
	# First for the generic no-config case (e.g. with mingw)
	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )
	set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )
	set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )
	# Second, for multi-config builds (e.g. msvc)
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
		set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_SOURCE_DIR}/bin )
		set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_SOURCE_DIR}/bin )
		set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_SOURCE_DIR}/bin )
	endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
else()
	set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)
	set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)
endif()

if(BUILD_MEMORY_MANAGER)
	include_directories("${CMAKE_SOURCE_DIR}/MemoryManager")
	add_subdirectory(MemoryManager)
endif()

if(BUILD_GUI)
	add_subdirectory(GUI)
endif()

add_subdirectory(GameEngine)
add_subdirectory(common)
add_subdirectory(OpenGLRenderer)
add_subdirectory(Input)
#add_subdirectory(AdventureGame)
#add_subdirectory(MineSweeper)
#add_subdirectory(FlockingAlgorithm)

add_executable(GameLauncher GameLauncher/main.cpp)
target_link_libraries(GameLauncher GameEngine)

set(CPACK_GENERATOR "ZIP")
include(InstallRequiredSystemLibraries)
include(CPack)

install(TARGETS GameLauncher RUNTIME DESTINATION ./)
install(FILES base.r DESTINATION ./)
install(DIRECTORY textures/ DESTINATION textures/)
install(DIRECTORY shaders/ DESTINATION shaders/)



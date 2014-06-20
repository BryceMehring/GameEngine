cmake_minimum_required(VERSION 2.8)
project(GameEngine CXX)

option(BUILD_EXAMPLES "Build Examples" OFF)
option(BUILD_QUADTREE "Build QuadTree" OFF)
option(BUILD_MEMORY_MANAGER "Build Memory Manager" OFF)
option(BUILD_GUI "Build GUI" OFF)
option(ENABLE_CPACK "Enable CPack" OFF)

# The version number.
set(ENGINE_VERSION_MAJOR 1)
set(ENGINE_VERSION_MINOR 0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules")
include(${CMAKE_SOURCE_DIR}/CMake/VisualStudioVersionCheck.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/CompileFlags.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/ConfigurePluginExtension.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/RedirectOutputTo.cmake)

RedirectOutputTo("bin")

find_package(GLEW)
include_directories(${GLEW_INCLUDE_DIR})

find_package(GLFW)
include_directories(${GLFW_INCLUDE_DIR})

find_package(STB_IMAGE)
include_directories(${STB_IMAGE_INCLUDE_DIR})

find_package(GLM)
include_directories(${GLM_INCLUDE_DIR})
add_definitions(-DGLM_FORCE_RADIANS)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(${CMAKE_SOURCE_DIR}/source/GameEngine/interfaces)
include_directories(${CMAKE_SOURCE_DIR}/source/GameEngine)

if(BUILD_MEMORY_MANAGER)
	include_directories(${CMAKE_SOURCE_DIR}/source/MemoryManager)
	add_subdirectory(source/MemoryManager)
endif()

if(BUILD_GUI OR BUILD_EXAMPLES)
	include_directories(${CMAKE_SOURCE_DIR}/source/GUI)
	add_subdirectory(source/GUI)
endif()

add_subdirectory(source/GameEngine)
add_subdirectory(source/common)
add_subdirectory(source/OpenGLRenderer)
add_subdirectory(source/Input)

if(BUILD_QUADTREE)
	include_directories(${CMAKE_SOURCE_DIR}/source/QuadTree)
	add_subdirectory(source/QuadTree)
endif()

if(BUILD_EXAMPLES)
	add_subdirectory(Examples/MineSweeper)
	add_subdirectory(Examples/Joystick)
	add_subdirectory(Examples/MinimumSpanningTree)
endif()

add_executable(GameLauncher source/GameLauncher/main.cpp)
target_link_libraries(GameLauncher GameEngine)

if(ENABLE_CPACK)
	set(CPACK_GENERATOR "ZIP")
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "./")
	include(InstallRequiredSystemLibraries)
	include(CPack)

	install(TARGETS GameLauncher RUNTIME DESTINATION ./)
        install(FILES ${GLFW_SHARED_LIBRARY} DESTINATION ./)
	install(FILES base.r DESTINATION ./)
	install(DIRECTORY bin/plugin DESTINATION ./ FILES_MATCHING REGEX "(.*((\\.plug)|(\\.r)))|(textures/)")
	install(DIRECTORY textures/ DESTINATION textures/ FILES_MATCHING REGEX ".*(\\.png)")
	install(DIRECTORY shaders/ DESTINATION shaders/)
endif(ENABLE_CPACK)



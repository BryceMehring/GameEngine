
#find GAME_ENGINE

find_path(GAME_ENGINE_INCLUDE_DIR IGameState.h
	"${CMAKE_SOURCE_DIR}/../GameEngine/GameEngine"
	)
	
find_library(GAME_ENGINE_LIB NAMES GameEngine PATHS "${CMAKE_SOURCE_DIR}/../GameEngine/bin")
find_library(GAME_ENGINE_COMMON_LIB NAMES common PATHS "${CMAKE_SOURCE_DIR}/../GameEngine/bin")
find_library(GAME_ENGINE_QUADTREE_LIB NAMES QuadTree PATHS "${CMAKE_SOURCE_DIR}/../GameEngine/bin")
find_library(GAME_ENGINE_GUI_LIB NAMES GUI PATHS "${CMAKE_SOURCE_DIR}/../GameEngine/bin")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GAME_ENGINE REQUIRED_VARS GAME_ENGINE_INCLUDE_DIR GAME_ENGINE_LIB GAME_ENGINE_COMMON_LIB GAME_ENGINE_QUADTREE_LIB GAME_ENGINE_GUI_LIB)

#mark_as_advanced(GAME_ENGINE_INCLUDE_DIR GAME_ENGINE_LIB)

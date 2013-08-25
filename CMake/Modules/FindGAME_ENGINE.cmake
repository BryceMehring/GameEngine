
#find GAME_ENGINE

find_path(GAME_ENGINE_INCLUDE_DIR IGameState.h
	"${CMAKE_SOURCE_DIR}/../GameEngine/GameEngine"
	)
	
find_library(GAME_ENGINE_LIBRARY_DEBUG NAMES GameEngine PATHS "${CMAKE_SOURCE_DIR}/../GameEngine/bin/Debug")
find_library(GAME_ENGINE_LIBRARY_RELEASE NAMES GameEngine PATHS "${CMAKE_SOURCE_DIR}/../GameEngine/bin/Release")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GAME_ENGINE REQUIRED_VARS GAME_ENGINE_INCLUDE_DIR GAME_ENGINE_LIBRARY_DEBUG)

mark_as_advanced(GAME_ENGINE_INCLUDE_DIR GAME_ENGINE_LIBRARY_DEBUG)

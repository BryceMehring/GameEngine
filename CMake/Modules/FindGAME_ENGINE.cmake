
#find GAME_ENGINE

find_path(GAME_ENGINE_INCLUDE_DIR IGameState.h
	"${CMAKE_SOURCE_DIR}/../GameEngine/GameEngine"
	)
	
find_path(GAME_LAUNCHER_INCLUDE_DIR interfaces/IGame.h
	"${CMAKE_SOURCE_DIR}/../GameEngine/GameLauncher"
	)
	
find_library(GAME_ENGINE_LIBRARY NAMES GameEngine PATHS "${CMAKE_SOURCE_DIR}/../GameEngine/bin")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GAME_ENGINE REQUIRED_VARS GAME_ENGINE_INCLUDE_DIR GAME_ENGINE_LIBRARY)

mark_as_advanced(GAME_ENGINE_INCLUDE_DIR GAME_ENGINE_LIBRARY)


file(GLOB_RECURSE GAME_ENGINE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE GAME_ENGINE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# build the game engine
find_package(glfw3 CONFIG REQUIRED)
add_library(GameEngine SHARED ${GAME_ENGINE_HEADERS} ${GAME_ENGINE_SOURCE})
target_link_libraries(GameEngine common glfw)

add_definitions(-DGAME_ENGINE_EXPORT)

if(ENABLE_CPACK)
	install(TARGETS GameEngine
		RUNTIME DESTINATION ./ COMPONENT Runtime
		LIBRARY DESTINATION ./ COMPONENT Runtime
		)
endif(ENABLE_CPACK)

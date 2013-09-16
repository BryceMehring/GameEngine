file(GLOB_RECURSE GAME_ENGINE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE GAME_ENGINE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

list(APPEND GAME_ENGINE_HEADERS
	${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptbuilder/scriptbuilder.h
	${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptstdstring/scriptstdstring.h
	${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scripthelper/scripthelper.h
	${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptmath/scriptmath.h
	)
list(APPEND GAME_ENGINE_SOURCE
	${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptbuilder/scriptbuilder.cpp
	${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptstdstring/scriptstdstring.cpp
	${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scripthelper/scripthelper.cpp
	${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptmath/scriptmath.cpp
	)

# build the game engine
add_library(GameEngine SHARED ${GAME_ENGINE_HEADERS} ${GAME_ENGINE_SOURCE})
target_link_libraries(GameEngine ${GLFW_LIBRARY})
if(MSVC)
	target_link_libraries(GameEngine debug ${ANGELSCRIPT_LIBRARY_DEBUG})
	target_link_libraries(GameEngine optimized ${ANGELSCRIPT_LIBRARY_RELEASE})
else()
	target_link_libraries(GameEngine ${ANGELSCRIPT_LIBRARY_RELEASE})
endif()

add_definitions(-DGAME_ENGINE_EXPORT)

if(ENABLE_CPACK)
	Install(TARGETS GameEngine DESTINATION ./)
endif(ENABLE_CPACK)








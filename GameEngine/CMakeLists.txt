

set(GAME_ENGINE_HEADERS
		asVM.h
		FileManager.h
		Game.h
		GameStateMachine.h
		GLFWInit.h
		interfaces/IGameState.h
		interfaces/IGameState.h
		interfaces/IInput.h
		interfaces/IKMInput.h
		interfaces/IPlugin.h
		interfaces/IRenderer.h
		interfaces/IResourceManager.h
		PluginManager.h
		Singleton.h
		VecMath.h
		${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptbuilder/scriptbuilder.h
		${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptstdstring/scriptstdstring.h
		${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scripthelper/scripthelper.h
		${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptmath/scriptmath.h
	)

set(GAME_ENGINE_SOURCE
		asVM.cpp
		FileManager.cpp
		Game.cpp
		GameStateMachine.cpp
		GLFWInit.cpp
		PluginManager.cpp
		VecMath.cpp
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
	Install(TARGETS GameEngine LIBRARY DESTINATION ./)
endif(ENABLE_CPACK)








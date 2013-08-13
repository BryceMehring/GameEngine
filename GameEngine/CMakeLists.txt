

set(ENGINE_HEADERS
        asVM.h
        BNew.h
        Camera.h
        Delegates.h
        FileManager.h
        Game.h
        GameConstants.h
        GameStateMachine.h
        GameStateScript.h
        Heap.h
        IGameState.h
        IGameState.h
        IGrid.h
        IInput.h
        IKMInput.h
        IPlugin.h
        IRenderer.h
        IResourceManager.h
        MemoryPool.h
        Menu.h
        PluginManager.h
        QuadTree.h
        RefCounting.h
        RTTI.h
        SerializedState.h
        SerializedState.h
        Singleton.h
        StringAlgorithms.h
        Timer.h
        VecMath.h
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptbuilder/scriptbuilder.h
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptstdstring/scriptstdstring.h
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scripthelper/scripthelper.h
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptmath/scriptmath.h
    )

set(ENGINE_SOURCE
        asVM.cpp
        BNew.cpp
        Camera.cpp
        FileManager.cpp
        Game.cpp
        GameConstants.cpp
        GameStateMachine.cpp
        GameStateScript.cpp
        Heap.cpp
        IGrid.inl
        MemoryPool.cpp
        Menu.cpp
        PluginManager.cpp
        QuadTree.cpp
        RefCounting.cpp
        SerializedState.cpp
        StringAlgorithms.cpp
        Timer.cpp
        VecMath.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptbuilder/scriptbuilder.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptstdstring/scriptstdstring.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scripthelper/scripthelper.cpp
        ${ANGELSCRIPT_ADDON_INCLUDE_DIR}/scriptmath/scriptmath.cpp
        )
    
# build the game engine
add_library(GameEngine STATIC ${ENGINE_HEADERS} ${ENGINE_SOURCE})
target_link_libraries(GameEngine dl)
target_link_libraries(GameEngine ${GLFW_LIBRARY})
  
if(MSVC)
    target_link_libraries(GameEngine debug ${ANGELSCRIPT_LIBRARY_DEBUG})
    target_link_libraries(GameEngine optimized ${ANGELSCRIPT_LIBRARY_RELEASE})
else(MSVC)
    target_link_libraries(GameEngine ${ANGELSCRIPT_LIBRARY_RELEASE})
endif(MSVC)

#set_target_properties(GameEngine PROPERTIES COMPILE_FLAGS "-Wall -std=c++0x -g -fno-rtti -m64")
  
#add_definitions("flags1")
  
  
  
  
  
  

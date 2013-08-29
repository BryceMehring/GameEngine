

set(OPENGL_RENDERER_SOURCE
    oglRenderer.cpp
    LineEngine.cpp
    ResourceManager.cpp
    FontEngine.cpp
    SpriteEngine.cpp
    GenerateBuffers.cpp
    )
    
set(OPENGL_RENDERER_HEADERS
    oglRenderer.h
    LineEngine.h
    ResourceManager.h
    FontEngine.h
    SpriteEngine.h
    GenerateBuffers.h
    )
    
    
add_library(renderer MODULE ${OPENGL_RENDERER_HEADERS} ${OPENGL_RENDERER_SOURCE})
target_link_libraries(renderer GameEngine ${GLFW_LIBRARY} ${GLEW_LIBRARY})

if(WIN32)
    target_link_libraries(renderer opengl32)
endif(WIN32)



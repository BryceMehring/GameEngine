

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
target_link_libraries(renderer ${GLEW_LIBRARY})

# why is this needed?
target_link_libraries(renderer GameEngine)

if(MSVC)
    target_link_libraries(renderer opengl32)
endif(MSVC)

set_target_properties(renderer PROPERTIES COMPILE_FLAGS "-Wall -std=c++0x -g -fno-rtti -m64")


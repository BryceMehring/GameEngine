

file(GLOB_RECURSE OPENGL_RENDERER_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)

file(GLOB_RECURSE OPENGL_RENDERER_SHADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/*.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/*.frag)
    
add_library(renderer MODULE ${OPENGL_RENDERER_SOURCE} ${OPENGL_RENDERER_SHADERS})
target_link_libraries(renderer common ${GLFW_SHARED_LIBRARY} ${GLEW_STATIC_LIBRARY})

if(WIN32)
    target_link_libraries(renderer opengl32)
endif(WIN32)

set_target_properties(renderer PROPERTIES PREFIX "")
set_target_properties(renderer PROPERTIES SUFFIX ".plug")
add_definitions(-DGLFW_DLL -DGLEW_STATIC -DPLUGIN_EXPORTS)

if(ENABLE_CPACK)
        Install(TARGETS renderer LIBRARY DESTINATION ./)
endif(ENABLE_CPACK)



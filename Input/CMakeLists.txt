
add_library(input MODULE Input.h Input.cpp)
target_link_libraries (input ${GLFW_LIBRARY})
add_definitions(-DGLFW_DLL -DPLUGIN_EXPORTS)

Install(TARGETS input LIBRARY DESTINATION ./)

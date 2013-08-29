

add_library(input MODULE Input.h Input.cpp)
target_link_libraries (input ${GLFW_LIBRARY})

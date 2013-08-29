

add_library(input MODULE DXInput.h DXInput.cpp)
target_link_libraries (input ${GLFW_LIBRARY})

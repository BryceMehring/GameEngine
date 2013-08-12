

add_library(input MODULE DXInput.h DXInput.cpp)
target_link_libraries (input ${GLFW_LIBRARY})

set_target_properties(input PROPERTIES COMPILE_FLAGS "-Wall -std=c++0x -g -fno-rtti -m64")


add_library(input MODULE Input.h Input.cpp)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries (input glfw)
add_definitions(-DGLFW_DLL -DPLUGIN_EXPORTS)

if(ENABLE_CPACK)
	Install(TARGETS input LIBRARY DESTINATION ./)
endif(ENABLE_CPACK)

set_target_properties(input PROPERTIES PREFIX "")
set_target_properties(input PROPERTIES SUFFIX ".plug")

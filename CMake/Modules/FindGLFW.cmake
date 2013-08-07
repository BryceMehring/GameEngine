
#find GLFW

find_path(GLFW_INCLUDE_DIR glfw3.h "${CMAKE_SOURCE_DIR}/../glfw/include/GLFW/")
find_library(GLFW_LIBRARY glfw3dll "${CMAKE_SOURCE_DIR}/../glfw/lib-msvc110/")

set(GLFW_INCLUDE_DIR ${GLFW_INCLUDE_DIR}/../)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW DEFAULT_MSG GLFW_INCLUDE_DIR GLFW_LIBRARY)

mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)

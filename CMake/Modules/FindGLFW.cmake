
#find GLFW

find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h "${CMAKE_SOURCE_DIR}/../glfw/include/GLFW/glfw3.h")
find_library(GLFW_LIBRARY NAMES glfw glfw3dll PATHS "${CMAKE_SOURCE_DIR}/../glfw/lib-msvc110/")

if(WIN32)
    set(GLFW_INCLUDE_DIR ${GLFW_INCLUDE_DIR}/../)
endif(WIN32)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW DEFAULT_MSG GLFW_INCLUDE_DIR GLFW_LIBRARY)

mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)

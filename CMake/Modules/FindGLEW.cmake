
#find GLEW

find_path(GLEW_INCLUDE_DIR glew.h "${CMAKE_SOURCE_DIR}/../glew/include/GL/" GL/)
find_library(GLEW_LIBRARY NAMES GLEW glew32 PATHS "${CMAKE_SOURCE_DIR}/../glew/lib/Release/Win32/" PATH_SUFFIXES lib64)

if(WIN32)
    set(GLEW_INCLUDE_DIR ${GLEW_INCLUDE_DIR}/../)
endif(WIN32)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLEW DEFAULT_MSG GLEW_INCLUDE_DIR GLEW_LIBRARY)

mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)


#find GLEW

find_path(GLEW_INCLUDE_DIR GL/glew.h
	/usr/include 
	"${CMAKE_SOURCE_DIR}/../glew/include"
	)
	
find_library(GLEW_LIBRARY NAMES GLEW glew32 PATHS "${CMAKE_SOURCE_DIR}/../glew/lib/Release/Win32/")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLEW REQUIRED_VARS GLEW_INCLUDE_DIR GLEW_LIBRARY)

mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)

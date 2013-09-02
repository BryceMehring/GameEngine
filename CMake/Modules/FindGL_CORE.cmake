
#find GL_CORE

find_path(GL_CORE_INCLUDE_DIR gl_core_3_3.hpp)
	
find_library(GL_CORE_LIBRARY NAMES gl_core)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GL_CORE REQUIRED_VARS GL_CORE_INCLUDE_DIR GL_CORE_LIBRARY)

mark_as_advanced(GL_CORE_INCLUDE_DIR GL_CORE_LIBRARY)

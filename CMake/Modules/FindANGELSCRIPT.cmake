# Find Angelscript

find_path(ANGELSCRIPT_INCLUDE_DIR angelscript.h "${CMAKE_SOURCE_DIR}/../angelscript/angelscript/include/")
find_path(ANGELSCRIPT_ADDON_INCLUDE_DIR scriptmath/ "${CMAKE_SOURCE_DIR}/../angelscript/add_on")
find_library(ANGELSCRIPT_LIBRARY_RELEASE angelscript)
if(MSVC)
	find_library(ANGELSCRIPT_LIBRARY_DEBUG angelscriptd)
endif(MSVC)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ANGELSCRIPT REQUIRED_VARS ANGELSCRIPT_INCLUDE_DIR ANGELSCRIPT_ADDON_INCLUDE_DIR ANGELSCRIPT_LIBRARY_RELEASE)

mark_as_advanced(ANGELSCRIPT_INCLUDE_DIR ANGELSCRIPT_ADDON_INCLUDE_DIR ANGELSCRIPT_LIBRARY_RELEASE ANGELSCRIPT_LIBRARY_DEBUG)

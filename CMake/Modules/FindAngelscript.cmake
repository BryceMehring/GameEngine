# Find Angelscript

find_path(ANGELSCRIPT_INCLUDE_DIRS angelscript.h "${CMAKE_SOURCE_DIR}/../angelscript/angelscript/include/")
find_library(ANGELSCRIPT_LIBRARY angelscript "${CMAKE_SOURCE_DIR}/../angelscript/angelscript/lib/")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ANGELSCRIPT REQUIRED_VARS ANGELSCRIPT_INCLUDE_DIRS ANGELSCRIPT_LIBRARY)

mark_as_advanced(ANGELSCRIPT_INCLUDE_DIRS ANGELSCRIPT_LIBRARY)

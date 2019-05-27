
#find MINGW_DEPENDS_DIR

find_path(MINGW_DEPENDS_DIR NAMES libstdc++-6.dll libgcc_s_sjlj-1.dll libwinpthread-1.dll)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MINGW_DEPENDS REQUIRED_VARS MINGW_DEPENDS_DIR)
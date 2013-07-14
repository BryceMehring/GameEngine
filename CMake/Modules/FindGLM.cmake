# Find GLM
#
# GLM_INCLUDE_DIR
# GLM_LIBRARY
# GLM_FOUND

find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp)

set(GLM_NAMES ${GLM_NAMES} glm GLM)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)

mark_as_advanced(GLM_INCLUDE_DIR GLM_LIBRARY)

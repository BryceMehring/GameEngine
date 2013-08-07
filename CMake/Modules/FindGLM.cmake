# Find GLM

find_path(GLM_INCLUDE_DIR glm.hpp "${CMAKE_SOURCE_DIR}/../glm/glm/")

set(GLM_INCLUDE_DIR ${GLM_INCLUDE_DIR}/../)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)

mark_as_advanced(GLM_INCLUDE_DIR)

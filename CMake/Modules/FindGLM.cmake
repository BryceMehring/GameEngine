# Find GLM

find_path(GLM_INCLUDE_DIR glm/glm.hpp "${CMAKE_SOURCE_DIR}/../glm")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM REQUIRED_VARS GLM_INCLUDE_DIR)

mark_as_advanced(GLM_INCLUDE_DIR)

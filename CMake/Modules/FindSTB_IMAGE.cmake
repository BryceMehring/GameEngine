# Find STB_IMAGE

find_path(STB_IMAGE_INCLUDE_DIR stb_image.c "${CMAKE_SOURCE_DIR}/../stb_image/")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(STB_IMAGE REQUIRED_VARS STB_IMAGE_INCLUDE_DIR)

mark_as_advanced(STB_IMAGE_INCLUDE_DIR)

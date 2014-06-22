
RedirectOutputTo("bin/plugin/PluginLoader")

if(MSVC)
	find_path(DIRENT_INCLUDE_DIR dirent.h)
	include_directories(${DIRENT_INCLUDE_DIR})
endif()

add_definitions(-DPLUGIN_EXPORTS)
add_library(PluginLoader MODULE PluginLoader.h PluginLoader.cpp)
target_link_libraries(PluginLoader GameEngine GUI)
if(ENABLE_CPACK)
        Install(TARGETS PluginLoader LIBRARY DESTINATION ./plugin/PluginLoader/)
endif(ENABLE_CPACK)

ConfigurePluginExtension("PluginLoader")


RedirectOutputTo("bin/plugin/PluginLoader")

if(MSVC)
	find_path(DIRENT_INCLUDE_DIR dirent.h)
	include_directories(${DIRENT_INCLUDE_DIR})
endif()

add_library(PluginLoader MODULE PluginLoader.h PluginLoader.cpp)
target_link_libraries(PluginLoader GameEngine GUI)

add_definitions(-DPLUGIN_EXPORTS)

ConfigurePluginExtension("PluginLoader")

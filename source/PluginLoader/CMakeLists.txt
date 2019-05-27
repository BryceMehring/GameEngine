
RedirectOutputTo("bin/plugin/PluginLoader")

add_definitions(-DPLUGIN_EXPORTS)
add_library(PluginLoader MODULE PluginLoader.h PluginLoader.cpp)
target_link_libraries(PluginLoader GameEngine GUI)
if(ENABLE_CPACK)
        Install(TARGETS PluginLoader LIBRARY DESTINATION ./plugin/PluginLoader/)
endif(ENABLE_CPACK)

ConfigurePluginExtension("PluginLoader")


RedirectOutputTo("bin/plugin/MinimumSpanningTree")

add_library(MinimumSpanningTree MODULE MinimumSpanningTree.h MinimumSpanningTree.cpp PriorityQueue.h)
target_link_libraries(MinimumSpanningTree GameEngine)

add_definitions(-DPLUGIN_EXPORTS)

ConfigurePluginExtension("MinimumSpanningTree")

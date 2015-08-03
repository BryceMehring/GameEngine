
RedirectOutputTo("bin/plugin/MinimumSpanningTree")

file(GLOB_RECURSE MINIMUM_SPANNING_TREE_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(MinimumSpanningTree MODULE ${MINIMUM_SPANNING_TREE_SOURCE})
target_link_libraries(MinimumSpanningTree GameEngine)

add_definitions(-DPLUGIN_EXPORTS)

ConfigurePluginExtension("MinimumSpanningTree")

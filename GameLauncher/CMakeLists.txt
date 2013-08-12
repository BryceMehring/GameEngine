

add_executable(GameLauncher main.cpp)
target_link_libraries(GameLauncher GameEngine)

set_target_properties(GameLauncher PROPERTIES COMPILE_FLAGS "-Wall -std=c++0x -g -fno-rtti -m64")

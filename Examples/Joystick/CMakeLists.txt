
RedirectOutputTo("bin/plugin/Joystick")

add_library(Joystick MODULE Joystick.h Joystick.cpp)
target_link_libraries(Joystick GameEngine)

add_definitions(-DPLUGIN_EXPORTS)

ConfigurePluginExtension("Joystick")

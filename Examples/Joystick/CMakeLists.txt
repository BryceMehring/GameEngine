
RedirectOutputTo("Examples/Joystick/plugin/Joystick")

add_library(Joystick MODULE Joystick.h Joystick.cpp)
target_link_libraries(Joystick GameEngine)

ConfigurePluginExtension("Joystick")

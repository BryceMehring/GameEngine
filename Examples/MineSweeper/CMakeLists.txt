
RedirectOutputTo("Examples/MineSweeper/plugin/MineSweeper")
add_library(MineSweeper MODULE MineSweeper.h MineSweeper.cpp)
target_link_libraries(MineSweeper GameEngine GUI)

GameEngine
==========
| System | Compiler | Status |
| ------ | -------- | ------ |
| Linux 64 bit | GCC 4.8 | [![Build Status](https://drone.io/github.com/BryceMehring/GameEngine/status.png)](https://drone.io/github.com/BryceMehring/GameEngine/latest) |
| Windows | Visual Studio 2015 | [![Build status](https://ci.appveyor.com/api/projects/status/ctb7oafj5gsri990?svg=true)](https://ci.appveyor.com/project/BryceMehring/gameengine) |


#### To create projects:
Open CMakeLists.txt with the CMake GUI to generate projects.

#### Windows: 
* Run `CreateWinSymLinks.bat` to create the symbolic links.
* Set GameLauncher as the startup project, and build the solution in at least Visual Studio 2013.
* If debugging within Visual Studio, the working directory must be set to `./bin`
* Copy `glfw3.dll` into `GameEngine/bin`

#### Linux:
* Run `CreateLinuxSymLinks.sh` to create the symbolic links.
* Build the solution via the cmake generated makefile

#### All Platforms: 
* To start a game, the command line argument must be set to name of the Game Project that the GameLauncher should load first. If a game is not specified via the command line, the user will have the option to choose which game to load from the main menu.

* When creating an out of source game plugin or running the examples, symbolic link the game plugin folder `Examples/<GameName>/plugin/<GameName>` into `GameEngine/bin/plugin/<GameName>`.

==========
How the root folder should look so that cmake should automatically detect dependencies:

    GameEngine/ - this is the root of the repo
    glfw/
    glew/
    glm/
    stb_image/stb_image.h

Dependencies
==========

* [GLFW](https://github.com/glfw/glfw)
* [GLEW](http://glew.sourceforge.net/)
* [GLM](https://github.com/g-truc/glm)
* [stb_image](https://github.com/nothings/stb)
* [cmake](http://www.cmake.org/)
* OpenGL 3.3 hardware support

#### If building in Visual Studio:
* [dirent](http://www.softagalleria.net/dirent.php)

#### Linux: 

    sudo apt-get install g++ git cmake cmake-qt-gui libgl1-mesa-dev libglu1-mesa-dev xorg-dev libglm-dev libglew-dev

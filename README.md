GameEngine
==========


#### To create projects:

Open CMakeLists.txt with the CMake GUI to generate projects.

#### Windows: 
* Run `CreateWinSymLinks.bat` as an administrator to create the symbolic links.
* Set GameLauncher as the startup project, and build the solution in at least Visual Studio 2013.
* If debugging within Visual Studio, the working directory must be set to `./bin`
* Copy `glfw3.dll` into `GameEngine/bin`

#### Linux:
* Run `CreateLinuxSymLinks.sh` to create the symbolic links.
* Build the solution via the cmake generated makefile

#### All Platforms: 

* To start a game, the command line argument must be set to name of the Game Project that the GameLauncher should load first.

* When creating an out of source game plugin or running the examples, symbolic link the game plugin folder: `Examples/<GameName>/plugin/<GameName>` into: `GameEngine/bin/plugin/<GameName>`.

==========

How the root folder should look so that cmake should automatically detect dependencies: 

    GameEngine/ - this is the root of the repo
    glfw/
    glew/
    glm/
    stb_image/stb_image.c

Dependencies
==========

* [GLFW](http://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [GLM](http://glm.g-truc.net/)
* [stb_image](http://nothings.org/stb_image.c)
* [cmake](http://www.cmake.org/)
* OpenGL 3.3 hardware support

#### Linux: 

    sudo apt-get install g++ git cmake cmake-qt-gui libgl1-mesa-dev libglu1-mesa-dev xorg-dev libglm-dev libglew-dev



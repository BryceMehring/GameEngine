GameEngine
==========


#### To create projects:

Open CMakeLists.txt with the CMake GUI to generate projects.

When creating an out of source game plugin, copy or use symbolic links to link the external plugin folder `plugin` into `GameEngine/bin`.

#### Windows: 
* Run `CreateWinSymLinks.bat` as an administrator to create the symbolic links.
* Set GameLauncher as the startup project, and build the solution in at least Visual Studio 2013.
* Copy `glfw3.dll` into `GameEngine/bin`
* To run a game, the command line argument must be set to name of the Game Project that the Game Engine should load first.

#### Linux: todo

==========

How the github folder should look:

    GameEngine/ - this is the root of the repo
    glfw/
    glew/
    glm/
    angelscript/
    stb_image/stb_image.c

Dependencies
==========

* [GLFW](http://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [GLM](http://glm.g-truc.net/)
* [stb_image](http://nothings.org/stb_image.c)
* [AngelScript](http://www.angelcode.com/angelscript/)
* [cmake](http://www.cmake.org/)
* OpenGL 3.3 hardware support

#### Linux: 

    sudo apt-get install g++ git cmake cmake-qt-gui libgl1-mesa-dev libglu1-mesa-dev xorg-dev libglm-dev libglew-dev



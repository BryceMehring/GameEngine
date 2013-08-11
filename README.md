GameEngine
==========


### To create projects:

#### Open CMakeLists.txt with the CMake GUI to generate projects

#### Windows: Run `CreateWinSymLinks.bat` as an administrator to create the symbolic links

Set GameLauncher as the startup project, and build the solution in Visual Studio 2012.

To run a game, the command line argument must be set to name of the Game Project that the Game Engine should load first.

==========

The compiled output will be located in source in a directory called bin.
How the github folder should look on windows:

`GameEngine/ - this is the root of the repo

glfw/

glew/

glm/

angelscript/

stb_image/stb_image.c`

Dependencies
==========

* [GLFW](http://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [GLM](http://glm.g-truc.net/)
* [stb_image](http://nothings.org/stb_image.c)
* [AngelScript](http://www.angelcode.com/angelscript/)
* [cmake](http://www.cmake.org/)

#### Linux: 
`sudo apt-get install g++ git mesa-common-dev libgl1-mesa-dev`
 `libglu1-mesa-dev libxrandr-dev libxi-dev libxmu-dev`



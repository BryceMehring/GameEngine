GameEngine
==========


### To create projects:

#### Windows: Run `WindowsBuildScript.bat` which translates to `cmake -G "visual studio 11" CMakeLists.txt` and also creates the symbolic links

Set GameLauncher as the startup project, and build the solution in Visual Studio 2012.

#### Linux: `cmake CMakeLists.txt`

compile: `make`

To run the game, the command line argument must be set to name of the Game Project that the Game Engine should load first.

==========

The compiled output will in source in a directory called bin.
An example of a GitHub folder:

GameEngine/ - this is the root of the repo

glfw/

glew/

glm/

angelscript/

stb_img.c

Dependencies
==========

* [GLFW](http://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [GLM](http://glm.g-truc.net/)
* [stb_img](http://nothings.org/stb_image.c)
* [AngelScript](http://www.angelcode.com/angelscript/)
* [cmake](http://www.cmake.org/)


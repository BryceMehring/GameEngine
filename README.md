GameEngine
==========


### To create projects:

#### Windows: `cmake -g "visual studio 10" CMakeLists.txt`

Set GameLauncher as the startup project, and build the solution.

#### Linux: `cmake CMakeLists.txt`

compile: `make`

==========

The compiled output will be in the directory above the repository. So on windows for example, in Debug mode in visual studio, all of the output will be in Debug. 
Also, the compiler will be looking for Angelscript, GLFW, GLEW, GLM in this same directory as shown below:

All within the folder GameEngine:

Debug/

GameEngine/

angelscript/

glew/

glfw/

glm/

Starting point is GameLauncher.

Dependencies
==========

* [GLFW](http://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [GLM](http://glm.g-truc.net/)
* [stb_img](http://nothings.org/stb_image.c)
* [AngelScript](http://www.angelcode.com/angelscript/)
* [cmake](http://www.cmake.org/)



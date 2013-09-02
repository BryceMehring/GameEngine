#include "GLFWInit.h"
#include <GLFW/glfw3.h>
#include <cassert>

GLFWInit::GLFWInit()
{
	assert(glfwInit() == GL_TRUE);
}

GLFWInit::~GLFWInit()
{
	glfwTerminate();
}



#include "GLFWInit.h"
#include <GLFW/glfw3.h>
#include <cassert>
#include "Log.h"

static void ErrorCallback(int, const char* error)
{
	Log::Instance().Write(error);
	abort();
}

GLFWInit::GLFWInit()
{
	glfwSetErrorCallback(ErrorCallback);

	assert(glfwInit() == GL_TRUE);
}

GLFWInit::~GLFWInit()
{
	glfwTerminate();
}



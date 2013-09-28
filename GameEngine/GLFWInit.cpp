#include "GLFWInit.h"
#include <GLFW/glfw3.h>
#include <cassert>
#include "FileManager.h"

static void ErrorCallback(int, const char* error)
{
	FileManager::Instance().WriteToLog(error);
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



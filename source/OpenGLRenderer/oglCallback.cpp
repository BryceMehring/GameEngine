#include "oglCallback.h"
#include "Log.h"

#include <sstream>
#include <iostream>

void GLAPIENTRY OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::ostringstream stream;
	stream << "OpenGL Error" << std::endl << "Source: ";

	switch(source)
	{
		case GL_DEBUG_SOURCE_API: stream << "API"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: stream << "GLSL compiler"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: stream << "Windowing system"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: stream << "External Libraries"; break;
		case GL_DEBUG_SOURCE_APPLICATION: stream << "Application"; break;
		case GL_DEBUG_SOURCE_OTHER: stream << "Other"; break;
		default: stream << "Unknown"; break;
	}

	stream << std::endl << "Type: ";

	switch(type)
	{
		case GL_DEBUG_TYPE_ERROR: stream << "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: stream << "Deprecated"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: stream << "Undefined"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: stream << "Performance"; break;
		case GL_DEBUG_TYPE_PORTABILITY: stream << "Portability"; break;
		case GL_DEBUG_TYPE_OTHER: stream << "Other"; break;
		default: stream << "Unknown"; break;
	}

	stream << std::endl << "Severity: ";

	switch(severity)
	{
		case GL_DEBUG_SEVERITY_LOW: stream << "low"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: stream << "medium"; break;
		case GL_DEBUG_SEVERITY_HIGH: stream << "high"; break;
		default: stream << "Unknown"; break;
	}

	stream << std::endl << "Message: " << message;

	Log::Instance().Write(stream.str());

	if(severity ==  GL_DEBUG_SEVERITY_HIGH)
	{
		// todo: maybe we could throw here
		abort();
	}
}

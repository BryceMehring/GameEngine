#define GLFW_DLL
#define GLFW_NO_GLU

#include "Timer.h"
#include <GL/glfw.h>

Timer::Timer() : m_fStart(0.0), m_fEnd(0.0)
{
	Reset();
}

void Timer::Start()
{
	_Active = true;
	PollCounter(m_fStart);
}

void Timer::Stop()
{
	_Active = false;
	PollCounter(m_fEnd);
}

// Stops the timer if it's active and resets all
// of the Timer's members to their initial values.
void Timer::Reset()
{
	if (_Active)
		Stop();

	glfwSetTime(0.0);
	_Active = false;
}

// Returns the time elapsed since Start() was called
// in micro-seconds
double Timer::GetTime()
{
	if (_Active) { PollCounter(m_fEnd); }
	
	return m_fEnd - m_fStart;
}

// Returns TRUE if the Timer is currently active
bool Timer::IsActive() const
{
	return _Active;
}

void Timer::PollCounter(double& Out)
{
	Out = glfwGetTime();
}



#include "Timer.h"

using namespace std::chrono;

Timer::Timer() : m_bActive(false)
{
}

void Timer::Start()
{
	m_bActive = true;
	PollCounter(m_start);
}

void Timer::Stop()
{
	m_bActive = false;
	PollCounter(m_end);
}

// Stops the timer if it's active and resets all
// of the Timer's members to their initial values.
void Timer::Reset()
{
	if (m_bActive)
		Stop();

	m_bActive = false;
}

// Returns the time elapsed since Start() was called
// in micro-seconds
double Timer::GetTime()
{
	if (m_bActive) { PollCounter(m_end); }

	duration<double> time_span = duration_cast<duration<double>>(m_end - m_start);
	return time_span.count();
}

// Returns TRUE if the Timer is currently active
bool Timer::IsActive() const
{
	return m_bActive;
}

void Timer::PollCounter(high_resolution_clock::time_point& Out)
{
	Out = high_resolution_clock::now();
}


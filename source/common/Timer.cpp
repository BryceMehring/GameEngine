
#include "Timer.h"

using namespace std;

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

void Timer::Reset()
{
	if (m_bActive)
	{
		Stop();
	}

	m_start = m_end;
	m_bActive = false;
}

double Timer::GetTime()
{
	typedef std::chrono::high_resolution_clock::duration duration;
	typedef std::chrono::high_resolution_clock::period period;

	if (m_bActive)
	{
		PollCounter(m_end);
	}

	duration time_span = std::chrono::duration_cast<duration>(m_end - m_start);
	return time_span.count() / (double)period::den;
}

bool Timer::IsActive() const
{
	return m_bActive;
}

void Timer::PollCounter(chrono::high_resolution_clock::time_point& Out)
{
	Out = chrono::high_resolution_clock::now();
}


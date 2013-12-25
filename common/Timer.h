#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>

#include "CommonExport.h"

// Defines a simple High Resolution Timer
class Timer
{
public:

	// Construct the timer initally stopped.
	COMMON_API Timer();

	// Activate the timer and poll the counter.
	COMMON_API void Start();

	// Deactivate the timer and poll the counter.
	COMMON_API void Stop();

	// Stops the timer if it's active and resets all
	// of the Timer's members to their initial values.
	COMMON_API void Reset();

	// Returns the time elapsed since Start() was called in seconds,
	// the accuracy of the time is specified by the template parameter T
	// which can either be nanoseconds, microseconds, milliseconds, or seconds
	template< class T = std::chrono::microseconds>
	COMMON_API double GetTime();

	// Returns true if the Timer is currently active
	COMMON_API bool IsActive() const;

private:

	// Poll the query performance counter
	void PollCounter(std::chrono::high_resolution_clock::time_point& Out);

private:
	bool m_bActive;
	std::chrono::high_resolution_clock::time_point m_start;
	std::chrono::high_resolution_clock::time_point m_end;
}; // class Timer

template< class T >
double Timer::GetTime()
{
	typedef typename T::period period;

	if(m_bActive)
	{
		PollCounter(m_end);
	}

	T time_span = std::chrono::duration_cast<T>(m_end - m_start);
	return time_span.count() / (double)period::den;
}


#endif

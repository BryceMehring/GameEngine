#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>

// Defines a simple High Resolution Timer
class Timer
{
public:
	// Construct the timer and initialize all of the
	// members by resetting them to their zero-values.
	Timer();

	// Activate the timer and poll the counter.
	void Start();

	// Deactivate the timer and poll the counter.
	void Stop();

	// Stops the timer if it's active and resets all
	// of the Timer's members to their initial values.
	void Reset();

	// Returns the time elapsed since Start() was called in seconds
	double GetTime();

	// Returns true if the Timer is currently active
	bool IsActive() const;

private:

	// Poll the query performance counter
	void PollCounter(std::chrono::high_resolution_clock::time_point& Out);

private:
	bool m_bActive;
	std::chrono::high_resolution_clock::time_point m_start;
	std::chrono::high_resolution_clock::time_point m_end;
}; // class Timer

#endif

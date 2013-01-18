#ifndef _TIMER_H_
#define _TIMER_H_

#include <Windows.h>
 
// Defines a simple High Resolution Timer
class Timer
{
public:
    // Construct the timer and initialize all of the
    // members by resetting them to their zero-values.
    Timer();

	~Timer();
 
    // Activate the timer and poll the counter.
    void Start();
 
    // Deactivate the timer and poll the counter.
    void Stop();
 
    // Stops the timer if it's active and resets all
    // of the Timer's members to their initial values.
    void Reset();
 
    // Returns the time elapsed since Start() was called
    // in micro-seconds
    double GetTime();
 
    // Returns TRUE if the Timer is currently active
    bool IsActive() const;

private:
    // Poll the query performance counter, safely by tying
    // the polling functionality temporarily to a single
    // logical processor (identified by 0).
    void PollCounter(LARGE_INTEGER& Out);
 
private:
    bool _Active;
	double _Frequency;
    LARGE_INTEGER
        _StartCount,
        _EndCount;
}; // class Timer
 
#endif
#ifndef _TIMER_H_
#define _TIMER_H_
 
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
 
// Defines a simple High Resolution Timer
class Timer
{
public:
    // Construct the timer and initialize all of the
    // members by resetting them to their zero-values.
    Timer()
	{
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);

		_Frequency = (double)f.QuadPart;

        Reset();
    }
 
    // Activate the timer and poll the counter.
    void Start()
	{
        _Active = true;
        PollCounter(_StartCount);
    }
 
    // Deactivate the timer and poll the counter.
    void Stop() {
        _Active = false;
        PollCounter(_EndCount);
    }
 
    // Stops the timer if it's active and resets all
    // of the Timer's members to their initial values.
    void Reset() {
        if (_Active)
            Stop();

        _StartCount.QuadPart = (_EndCount.QuadPart = 0);
        _Active = false;
    }
 
    // Returns the time elapsed since Start() was called
    // in micro-seconds
    double GetTime()
	{
		if (_Active) { PollCounter(_EndCount); }
 
        return (_EndCount.QuadPart - _StartCount.QuadPart) / _Frequency;
    }
 
    // Returns the time elapsed since Start() was called
    // in milli-seconds
  /*  long double GetTimeInMilliseconds(void) {
        return GetTimeInMicroSeconds() * 0.001;
    }
 
    // Returns the time elapsed since Start() was called
    // in seconds
    long double GetTimeInSeconds( void ) {
        return GetTimeInMicroSeconds() * 0.000001;
    }*/
 
    // Returns TRUE if the Timer is currently active
    const bool IsActive() const {
        return _Active;
    }
private:
    // Poll the query performance counter, safely by tying
    // the polling functionality temporarily to a single
    // logical processor (identified by 0).
    void PollCounter(LARGE_INTEGER& Out) {
        HANDLE Thread = GetCurrentThread();
 
        DWORD_PTR OldMask = SetThreadAffinityMask(Thread, 0);
            QueryPerformanceCounter(&Out);
        SetThreadAffinityMask(Thread, OldMask);
    }
 
private:
    bool _Active;
	double _Frequency;
    LARGE_INTEGER
        _StartCount,
        _EndCount;
}; // class Timer
 
#endif
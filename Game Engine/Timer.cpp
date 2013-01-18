#include "Timer.h"

#pragma comment(lib,"Winmm.lib")

Timer::Timer()
{
	timeBeginPeriod(1);
	LARGE_INTEGER f;
	QueryPerformanceFrequency(&f);

	_Frequency = (double)f.QuadPart;

    Reset();
}

Timer::~Timer()
{
	timeEndPeriod(1);
}

void Timer::Start()
{
	_Active = true;
	PollCounter(_StartCount);
}

void Timer::Stop()
{
	_Active = false;
	PollCounter(_EndCount);
}

 // Stops the timer if it's active and resets all
    // of the Timer's members to their initial values.
void Timer::Reset()
{
    if (_Active)
        Stop();

    _StartCount.QuadPart = (_EndCount.QuadPart = 0);
    _Active = false;
}
 
// Returns the time elapsed since Start() was called
// in micro-seconds
double Timer::GetTime()
{
	if (_Active) { PollCounter(_EndCount); }
 
    return (_EndCount.QuadPart - _StartCount.QuadPart) / _Frequency;
}

// Returns TRUE if the Timer is currently active
bool Timer::IsActive() const
{
	return _Active;
}

void Timer::PollCounter(LARGE_INTEGER& Out)
{
    HANDLE Thread = GetCurrentThread();
 
    DWORD_PTR OldMask = SetThreadAffinityMask(Thread, 0);
        QueryPerformanceCounter(&Out);
    SetThreadAffinityMask(Thread, OldMask);
}


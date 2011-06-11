
#ifndef _THREAD_
#define _THREAD_
#pragma once

class Threaded
{
public:

	Threaded();
	virtual ~Threaded() {}

	void Go();
	void Stop();
	
protected:

	volatile bool m_stoprequested;
    boost::shared_ptr<boost::thread> m_thread;
    boost::mutex m_mutex;

	virtual void DoWork() = 0;

};

#endif //_THREAD_
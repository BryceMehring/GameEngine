
#ifndef _THREAD_
#define _THREAD_


class Threaded
{
public:

	Threaded();
	virtual ~Threaded() {}

	void Go();
	void Stop();
	
protected:

	volatile bool m_stoprequested;
    boost::thread* m_pthread;
    boost::mutex m_mutex;

	virtual void DoWork() = 0;

};

#endif //_THREAD_
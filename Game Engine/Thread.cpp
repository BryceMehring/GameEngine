
#include "stdafx.h"
#include "Thread.h"

using namespace std;

Threaded::Threaded() : m_stoprequested(false), m_pthread(nullptr)
{
}

void Threaded::Go()
{
	//boost::

	if(!m_pthread)
	{
		m_stoprequested = false;
		m_pthread = (new boost::thread(boost::bind(&Threaded::DoWork, this)));
	}
}

void Threaded::Stop()
{
	if(m_pthread)
	{
		m_stoprequested = true;
		m_pthread->join();

		delete m_pthread;
		m_pthread = nullptr;
	}
}
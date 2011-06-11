
#include "stdafx.h"
#include "Thread.h"

Threaded::Threaded() : m_stoprequested(false)
{
}

void Threaded::Go()
{
	assert(!m_thread);
	m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Threaded::DoWork, this)));
}

void Threaded::Stop()
{
	assert(m_thread);
	m_stoprequested = true;
	m_thread->join();
}
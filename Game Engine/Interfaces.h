
#pragma once
#include <d3dx9math.h>

class IRefCounting
{
public:

	IRefCounting() : m_iCounter(1) {}

	virtual int AddRef() { return (++m_iCounter); }
	virtual int Release()
	{
		int iTempCount = --m_iCounter;

		if(m_iCounter <= 0)
		{
			delete this;
		}

		return iTempCount;
	}

protected:

	virtual ~IRefCounting() {}

private:

	int m_iCounter;

};

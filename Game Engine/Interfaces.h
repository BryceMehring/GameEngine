
#pragma once
#include "Mtrl.h"

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

class IFunct
{
public:

	virtual ~IFunct() {}

	virtual void SetParameters(ID3DXEffect* pEffect, const Mtrl& mtrl, DWORD i) = 0;
};

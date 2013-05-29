#ifndef _REFCOUNTING_
#define _REFCOUNTING_

class RefCounting
{
public:

	/*
	Once the object(inherited from RefCounting) is created, the counter
	will start at one. When the pointer to the object is copied, AddRef must be called to
	increate the counter. Once the pointer is no longer needed, Release must be called to
	decrease the counter. Once the counter reaches 0, the object will be deleted.
	*/

	// constructor sets the counter of references to 1
	RefCounting();

	int GetCounter() const;

	// increases counter
	virtual void AddRef();

	// decreases counter, if it reaches 0, the object will then be deleted
	virtual void Release();

	static int GetAddRefCount();
	static int GetAddReleaseCount();

protected:

	// protected destructor, forcing classes to use Release() to delete the object
	virtual ~RefCounting() {}

private:

	static int s_iAddRef;
	static int s_iRelease;

	int m_iCounter;

};

#endif // _REFCOUNTING_

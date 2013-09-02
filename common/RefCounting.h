#ifndef _REFCOUNTING_
#define _REFCOUNTING_

#ifdef _WIN32
#ifdef COMMON_EXPORT
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
#else
#define EXPORT
#endif

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
	EXPORT RefCounting();

	EXPORT int GetCounter() const;

	// increases counter
	EXPORT virtual void AddRef();

	// decreases counter, if it reaches 0, the object will then be deleted
	EXPORT virtual void Release();

protected:

	// protected destructor, forcing classes to use Release() to delete the object
	virtual ~RefCounting() {}

private:

	int m_iCounter;

};

#endif // _REFCOUNTING_

// Programmed By Bryce Mehring

#ifndef _ENGINEHELPER_
#define _ENGINEHELPER_
#pragma once

namespace EngineHelper
{

void LoadAllFilesFromDictionary(std::vector<std::string>& files,
	const boost::filesystem::path& path, const std::string& ext);

template< class A, class B >
B* refCast(A* a)
{
    // If the handle already is a null handle, then just return the null handle
    if( a == nullptr ) return 0;

    // Now try to dynamically cast the pointer to the wanted type
    B* b = dynamic_cast<B*>(a);
    if( b != nullptr )
    {
        // Since the cast was made, we need to increase the ref counter for the returned handle
        b->AddRef();
    }
    return b;
}

template< class T >
void Construct(void* pMem)
{
	new(pMem) T();
}

template< class T >
void CopyConstruct(const T& other, T* pThis)
{
	new(pThis) T(other);
}

template< class T >
void Destroy(void* pMem)
{
	((T*)pMem)->~T();
}

}; // end of namespace

#endif // _ENGINEHELPER_
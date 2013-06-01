#ifndef _SPATIALHASHTABLE_
#define _SPATIALHASHTABLE_

#include <vector>
#include <list>
#include <d3dx9.h>
#include "VecMath.h"

template< class T >
class SpatialHashTable
{
public:

	SpatialHashTable(unsigned int x, unsigned int y, float cellSize) :
	m_iWidth(x), m_iHeight(y), m_buckets(x*y), m_iSize(0), m_fCellSize(cellSize), m_iBucketsUsed(0)
	{
	}

	void Insert(const D3DXVECTOR2& pos, const T& object)
	{
		unsigned int x = 0;
		unsigned int y = 0;

		Hash(pos,x,y);
		_Insert(GetIndex(x,y),object);
	}
	void Insert(const Math::ICollisionPolygon& polygon, const T& object)
	{
		unsigned int iMinX = 0;
		unsigned int iMinY = 0;
		unsigned int iMaxX = 0;
		unsigned int iMaxY = 0;

		Math::AABB aabb;
		polygon.GetAABB(aabb);

		Hash(aabb.min,iMinX,iMinY);
		Hash(aabb.max,iMaxX,iMaxY);

		for(unsigned int i = iMinX; i != (iMaxX + 1); i = (i + 1) % m_iWidth)
		{
			for(unsigned int j = iMinY; j != (iMaxY + 1); j = (j + 1) % m_iHeight)
			{
				_Insert(GetIndex(i,j),object);
			}
		}

	}
	void Erase(const D3DXVECTOR2& pos, const T& object)
	{
		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int index = 0;

		Hash(pos,x,y);

		index = GetIndex(x,y);

		for(auto iter = m_buckets[index].begin(); iter != m_buckets[index].end(); )
		{
			if(*iter == &object)
			{
				iter = m_buckets[index].erase(iter);
				--m_iSize;
			}
			else
			{
				++iter;
			}
		}
	}

	void Find(const D3DXVECTOR2& pos, std::list<T>& out) const
	{
		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int index = 0;

		Hash(pos,x,y);
		out = m_buckets[GetIndex(x,y)];
	}

	unsigned int Size() const
	{
		return m_iSize; 
	}
	unsigned int Buckets() const
	{
		return m_iBucketsUsed;
	}

	float LoadCap() const
	{
		return (float)m_iBucketsUsed / (float)m_buckets.size();
	}

	void Clear()
	{
		for(unsigned int i = 0; i < m_buckets.size(); ++i)
		{
			m_buckets[i].clear();
		}
	}

private:

	std::vector<std::list<T>> m_buckets;
	float m_fCellSize;
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	unsigned int m_iSize;
	unsigned int m_iBucketsUsed;

	void _Insert(unsigned int index, const T& object)
	{
		m_buckets[index].push_back(object);

		if(m_buckets[index].size() == 1)
		{
			++m_iBucketsUsed;
		}

		++m_iSize;
	}

	void Hash(const D3DXVECTOR2& pos, unsigned int& x, unsigned int& y) const
	{
		x = (unsigned int)(pos.x / m_fCellSize) % m_iWidth;
		y = (unsigned int)(pos.y / m_fCellSize) % m_iHeight;
	}

	unsigned int GetIndex(unsigned int x, unsigned int y) const
	{
		return x + y * m_iWidth;
	}
};


#endif // _SPATIALHASHTABLE_
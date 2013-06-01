#ifndef _TILER_
#define _TILER_

#include <vector>

// todo: implement
class SpriteTest
{

};

class Tiler
{
public:

	Tiler(unsigned int x, unsigned int y, unsigned int dx, unsigned int dy) : m_uiWidth(x), m_uiHeight(y),
	m_uiDx(dx), m_uiDy(dy)
	{
		m_tiles.resize(x*y);
	}

	SpriteTest& operator()(unsigned int x, unsigned int y)
	{
		x /= m_uiDx;
		y /= m_uiDy;

		return m_tiles[x + y*m_uiWidth];
	}

	const SpriteTest& operator()(unsigned int x, unsigned int y) const
	{
		x /= m_uiDx;
		y /= m_uiDy;

		return m_tiles[x + y*m_uiWidth];
	}

	void Render()
	{
	}

private:


	std::vector<SpriteTest> m_tiles;
	unsigned int m_uiWidth;
	unsigned int m_uiHeight;
	unsigned int m_uiDx;
	unsigned int m_uiDy;

};

#endif
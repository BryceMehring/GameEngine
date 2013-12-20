

template< class T >
IGrid<T>::IGrid() : m_gridSize(0.0f), m_numTiles(0.0f), m_center(0.0f)
{
}

template< class T >
IGrid<T>::IGrid(const std::string& file) : m_gridSize(0.0f), m_numTiles(0.0f), m_center(0.0f)
{
	Load(file);
}

template< class T >
void IGrid<T>::Render(class IRenderer& renderer) const
{
	glm::vec2 tileSize = GetTileSize();

	unsigned int index = 0;
	for(int i = 0; i < m_numTiles.y; ++i)
	{
		for(int j = 0; j < m_numTiles.x; ++j)
		{
			glm::vec2 pos(tileSize.x * j - (m_gridSize.x / 2.0f) + (tileSize.x / 2.0f), tileSize.y * i - (m_gridSize.y / 2.0f) + (tileSize.y / 2.0f));
			glm::mat4 transformation(glm::translate(pos.x + m_center.x,pos.y + m_center.y,-2.0f));
			transformation = glm::scale(transformation,tileSize.x,tileSize.y,1.0f);

			RenderTileCallback(renderer,m_tiles[index],transformation);

			index++;
		}
	}
}

template< class T >
bool IGrid<T>::WorldSpaceToTile(const glm::vec2& pos, T** outTile, glm::ivec2* pRoundedPosOut)
{
	if(pos.x >= (m_center.x - m_gridSize.x / 2.0f) && pos.x <= (m_center.x + m_gridSize.x / 2.0f) && 
	   pos.y <= (m_center.y + m_gridSize.y / 2.0f) && pos.y >= (m_center.y - m_gridSize.y / 2.0f))
	{

		glm::vec2 tileSize = GetTileSize();
		glm::vec2 transformedPos = pos;

		transformedPos += m_gridSize / 2.0f;

		transformedPos *= (1.0f / tileSize); // [0, m_uiWidth], [0, m_uiHeight]

		glm::ivec2 roundedPos(floor(transformedPos.x - m_center.x / tileSize.x),floor(transformedPos.y - m_center.y / tileSize.y));

		int index = m_numTiles.x*roundedPos.y + roundedPos.x;

		if(index >= 0 && index < (int)m_tiles.size())
		{
			if(pRoundedPosOut != nullptr)
			{
				*pRoundedPosOut = roundedPos;
			}

			if(outTile != nullptr)
			{
				*outTile = &m_tiles[index];
			}

			return true;
		}
	}

    return false;
}

template< class T >
bool IGrid<T>::Load(std::ifstream& stream)
{
	if(!stream.is_open())
		return false;

	unsigned int uiNumTiles = 0;
	stream >> uiNumTiles;
	stream >> m_gridSize.x >> m_gridSize.y;
	stream >> m_numTiles.x >> m_numTiles.y;
	stream >> m_center.x >> m_center.y;

	if(uiNumTiles == 0)
		return false;

	m_tiles.resize(uiNumTiles);

	unsigned int i = 0;
	while(!stream.eof() && i < uiNumTiles)
	{
		stream >> m_tiles[i];
		++i;
	}

	return true;
}

template< class T >
bool IGrid<T>::Save(std::ofstream& stream) const
{
	if(!stream.is_open())
		return false;

	stream << m_tiles.size() << std::endl;
	stream << m_gridSize.x << " " << m_gridSize.y << std::endl;
	stream << m_numTiles.x << " " << m_numTiles.y << std::endl;
	stream << m_center.x << " " << m_center.y << std::endl;

	for(unsigned int i = 0; i < m_tiles.size(); ++i)
	{
		stream << m_tiles[i] << std::endl;
	}

	return true;
}

template< class T >
void IGrid<T>::SetGridSize(const glm::vec2& size)
{
	m_gridSize = size;
}

template< class T >
void IGrid<T>::SetNumTiles(const glm::ivec2& size)
{
	m_numTiles = size;
	m_tiles.resize(m_numTiles.x * m_numTiles.y);
}

template< class T >
void IGrid<T>::SetCenter(const glm::vec2& center)
{
	m_center = center;
}

template< class T >
glm::vec2 IGrid<T>::GetTileSize() const
{
	return glm::vec2(m_gridSize.x / m_numTiles.x, m_gridSize.y / m_numTiles.y);
}

template< class T >
const glm::vec2& IGrid<T>::GetGridSize() const
{
	return m_gridSize;
}

template< class T >
const glm::ivec2& IGrid<T>::GetNumTiles() const
{
	return m_numTiles;
}

template< class T >
const glm::vec2& IGrid<T>::GetCenter() const
{
	return m_center;
}

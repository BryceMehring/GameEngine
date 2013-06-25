


template< class T >
IGrid<T>::IGrid(float width, float height,
				unsigned int tileWidth, unsigned int tileHeight,
				const glm::vec2& center) : m_tiles(tileWidth*tileHeight), m_gridSize(width,height),
				m_uiNumTilesWidth(tileWidth), m_uiNumTilesiHeight(tileHeight), m_tileSize(m_gridSize.x / m_uiNumTilesWidth,
				m_gridSize.y / m_uiNumTilesiHeight), m_center(center) {}

template< class T >
void IGrid<T>::Render(class IRenderer& renderer)
{
	unsigned int index = 0;
	for(unsigned int i = 0; i < m_uiNumTilesiHeight; ++i)
	{
		for(unsigned int j = 0; j < m_uiNumTilesWidth; ++j)
		{
			glm::vec2 pos(m_tileSize.x * j - (m_gridSize.x / 2.0f) + (m_tileSize.x / 2.0f), m_tileSize.y * i - (m_gridSize.y / 2.0f) + (m_tileSize.y / 2.0f));
			glm::mat4 T(glm::translate(pos.x,pos.y,-2.0f));
			T = glm::scale(T,m_tileSize.x,m_tileSize.y,1.0f);

			RenderTileCallback(renderer,m_tiles[index],T);

			index++;
		}
	}
}

template< class T >
bool IGrid<T>::WorldSpaceToTile(const glm::vec2& pos, T** outTile, glm::ivec2* pRoundedPosOut)
{
	glm::vec2 transformedPos(pos);
	transformedPos += m_gridSize / 2.0f;

	transformedPos *= (1.0f / m_tileSize); // [0, m_uiWidth], [0, m_uiHeight]

	glm::ivec2 roundedPos(floor(transformedPos.x),floor(transformedPos.y));

	unsigned int index = m_uiNumTilesWidth*roundedPos.y + roundedPos.x;

	if(index < m_tiles.size())
	{
		if(pRoundedPosOut != nullptr)
		{
			*pRoundedPosOut = roundedPos;
		}

		if(outTile != nullptr)
		{
			*outTile = &m_tiles[index];
		}
	}

	return index < m_tiles.size();
}


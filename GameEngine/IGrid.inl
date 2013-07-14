


template< class T >
IGrid<T>::IGrid(float width, float height,
                int tileWidth, int tileHeight,
				const glm::vec2& center) : m_tiles(tileWidth*tileHeight), m_gridSize(width,height),
                m_iNumTilesWidth(tileWidth), m_iNumTilesiHeight(tileHeight), m_tileSize(m_gridSize.x / m_iNumTilesWidth,
                m_gridSize.y / m_iNumTilesiHeight), m_center(center) {}

template< class T >
void IGrid<T>::Render(class IRenderer& renderer)
{
	unsigned int index = 0;
    for(int i = 0; i < m_iNumTilesiHeight; ++i)
	{
        for(int j = 0; j < m_iNumTilesWidth; ++j)
		{
			glm::vec2 pos(m_tileSize.x * j - (m_gridSize.x / 2.0f) + (m_tileSize.x / 2.0f), m_tileSize.y * i - (m_gridSize.y / 2.0f) + (m_tileSize.y / 2.0f));
            glm::mat4 transformation(glm::translate(pos.x,pos.y,-2.0f));
            transformation = glm::scale(transformation,m_tileSize.x,m_tileSize.y,1.0f);

            RenderTileCallback(renderer,m_tiles[index],transformation);

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

    int index = m_iNumTilesWidth*roundedPos.y + roundedPos.x;

    if(index < (int)m_tiles.size() && index >= 0)
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

    return index < (int)m_tiles.size();
}


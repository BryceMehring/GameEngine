
#include <fstream>

using namespace std;

template< class T >
IGrid<T>::IGrid(const std::string& file)
{
	//Load(file);
}

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

template< class T >
bool IGrid<T>::Load(std::ifstream& stream)
{
	if(!stream.is_open())
		return false;

	unsigned int uiNumTiles = 0;
	stream >> uiNumTiles;
	stream >> m_gridSize.x >> m_gridSize.y;
	stream >> m_iNumTilesWidth >> m_iNumTilesiHeight;
	stream >> m_tileSize.x >> m_tileSize.y;
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

	stream << m_tiles.size() << endl;
	stream << m_gridSize.x << " " << m_gridSize.y << endl;
	stream << m_iNumTilesWidth << " " << m_iNumTilesiHeight << endl;
	stream << m_tileSize.x << " " << m_tileSize.y << endl;
	stream << m_center.x << " " << m_center.y << endl;

	for(unsigned int i = 0; i < m_tiles.size(); ++i)
	{
		stream << m_tiles[i] << endl;
	}

	return true;
}


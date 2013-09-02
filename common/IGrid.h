#ifndef _IGRID_
#define _IGRID_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "ISerializable.h"

// Grid abstract interface which implements a basic way to render them, which should be abstracted into another class 
template< class T >
class IGrid : public ISerializable
{
public:

	// ctor builds the grid
	IGrid(const std::string& file);
	IGrid(float width, float height,
          int tileWidth, int tileHeight,
		  const glm::vec2& center = glm::vec2(0.0f));
	virtual ~IGrid() {}

	virtual int Update(IKMInput&) = 0;

	// Render calls RenderTileCallback for each tile
	virtual void Render(class IRenderer& renderer);

	virtual bool Load(std::ifstream& stream);
	virtual bool Save(std::ofstream& stream) const;

protected:

	// RenderTileCallback is called for each tile
    virtual void RenderTileCallback(class IRenderer& renderer,const T& tile, const glm::mat4& transformation) const = 0;

	//WorldSpaceToTile returns the tile from the input parameter pos via the parameter outTile
	// If the pos is required in array coordinates set pRoundedPosOut to point to an ivec2, else leave it null.
	// returns true if valid input
	// returns false if invalid input
	bool WorldSpaceToTile(const glm::vec2& pos, T** outTile, glm::ivec2* pRoundedPosOut = nullptr);

	std::vector<T> m_tiles; // the grid
	glm::vec2 m_gridSize;
    int m_iNumTilesWidth; // width of the grid, in tiles
    int m_iNumTilesiHeight; // height of the grid, in tiles

	glm::vec2 m_tileSize;
	
private:
	glm::vec2 m_center;
};



#include "IGrid.inl"

#endif // _IGRID_

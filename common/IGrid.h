#ifndef _IGRID_
#define _IGRID_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <fstream>
#include "ISerializable.h"

// Grid abstract interface which implements a basic way to render them, which should be abstracted into another class 
template< class T >
class IGrid : public ISerializable
{
public:

	IGrid();
	IGrid(const std::string& file);
	virtual ~IGrid() {}

	virtual int Update(class IInput&) = 0;

	// Render calls RenderTileCallback for each tile
	virtual void Render(class IRenderer& renderer) const;

	virtual bool Load(std::ifstream& stream);
	virtual bool Save(std::ofstream& stream) const;

	void SetGridSize(const glm::vec2& size);
	void SetNumTiles(const glm::ivec2& size);
	void SetCenter(const glm::vec3& center);

	glm::vec2 GetTileSize() const;
	const glm::vec2& GetGridSize() const;
	const glm::ivec2& GetNumTiles() const;
	const glm::vec3& GetCenter() const;

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
	glm::ivec2 m_numTiles;
	
private:
	glm::vec3 m_center;
};

#include "IGrid.inl"

#endif // _IGRID_


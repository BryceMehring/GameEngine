#ifndef _IGRID_
#define _IGRID_

#include <vector>
#include <fstream>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>

// Defines a 2D updateable and renderable grid
template< class T >
class IGrid
{
public:

	// Builds an empty grid
	IGrid();

	// Loads the grid from the specified file
	IGrid(const std::string& file);

	virtual ~IGrid() {}

	// The logic updater method
	virtual int Update(class IInput&) = 0;

	// Render calls RenderTileCallback for each tile
	virtual void Render(class IRenderer& renderer) const;

	// Loads grid from stream
	virtual bool Load(std::ifstream& stream);

	// Saves grid to stream
	virtual bool Save(std::ofstream& stream) const;

	// Sets the the width and height of the renderable grid
	void SetGridSize(const glm::vec2& size);

	// Sets the number of tiles in the x and y axis
	void SetNumTiles(const glm::uvec2& size);
	
	// Sets the center of the grid
	void SetCenter(const glm::vec3& center);

	glm::vec2 GetTileSize() const;

	// Returns the grid size
	const glm::vec2& GetGridSize() const;

	// Returns the number of tiles in the x and y axis
	const glm::uvec2& GetNumTiles() const;

	// Returns the center of the grid
	const glm::vec3& GetCenter() const;

protected:

	// RenderTileCallback is called for each tile
    virtual void RenderTileCallback(class IRenderer& renderer,const T& tile, const glm::mat4& transformation) const = 0;

	//WorldSpaceToTile returns the tile from the input parameter pos via the parameter outTile
	// If the pos is required in array coordinates set pRoundedPosOut to point to an ivec2, else leave it null.
	// returns true if valid input
	// returns false if invalid input
	bool WorldSpaceToTile(const glm::vec2& pos, T** outTile, glm::uvec2* pRoundedPosOut = nullptr);

	std::vector<T> m_tiles; // the grid
	glm::vec2 m_gridSize;
	glm::uvec2 m_numTiles;
	
private:
	glm::vec3 m_center;
};

#include "IGrid.inl"

#endif // _IGRID_


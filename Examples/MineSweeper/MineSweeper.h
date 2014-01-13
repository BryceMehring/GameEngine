#ifndef _MINESWEEPER_
#define _MINESWEEPER_


#include "IGrid.h"
#include "IGameState.h"
#include "GUI.h"
#include <stack>

enum class GameStatus
{
	Playing,
	Won,
	Lost,
};

struct Tile
{
	bool selsected;
	bool mine;
	bool marked;
	unsigned int minesNearby;
};

std::ostream& operator <<(std::ostream&, const Tile&);
std::istream& operator >>(std::istream&, Tile&);

// Defines a Grid to be used for MineSweeper
class Grid : public IGrid<Tile>
{
public:

	Grid();

	// Checks for user input and apply's their action on the grid
	virtual int Update(IInput&);

	// Renders the grid
	virtual void Render(IRenderer&) const;

	// Loads the grid from stream
	virtual bool Load(std::ifstream& stream);

	// Saves the grid to stream
	virtual bool Save(std::ofstream& stream) const;

	// Randomizes the location of the mines and unselects all tiles
	void Reset();

protected:

	// Callback method called by IGrid to render each tile of the grid
	virtual void RenderTileCallback(IRenderer&,const Tile&, const glm::mat4&) const;

private:

	// The number of mines on the grid
	unsigned int m_uiMineCount;

	// The number of tiles that have been marked
	unsigned int m_uiMarkedCount;

	// The number of tiles that have been marked correctly
	unsigned int m_uiMarkedCorrectlyCount;

	// Expands all zero tiles if the user clicks a zero tile
	void Expand(const glm::ivec2& pos);

	// Builds a randomized grid
	void BuildGrid();

};

class MineSweeper : public IGameState
{
public:

	MineSweeper();

	virtual DLLType GetPluginType() const { return DLLType::Game; }

	// returns the name of the plug-in
	virtual const char* GetName() const { return "MineSweeper"; }

	// returns the version number of the plugin
	virtual int GetVersion() const { return 0; }

	// Called only once when the plugin is created
	virtual void Init(class Game& game);

	// Called only once when the plugin is destroyed
	virtual void Destroy(class Game& game);

	// Called every frame to update the date of the game
	virtual void Update(class Game& game);

	// Called every frame to render the game
	virtual void Draw(class Game& game);

private:

	Grid m_grid;
	UI::GUI m_gui;
	UI::GUI m_mainMenu;
	GameStatus m_gameState;
	double m_fTime;

	// Builds menus
	void CreatePlayingMenu(Game&);
	void CreateMainMenu(Game&);

	// Clamps the cursor to the screen
	void ClampMouse(IInput&, IRenderer&) const;

	void Reset();


};

#endif // _MINESWEEPER_
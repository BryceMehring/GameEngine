#ifndef _MINESWEEPER_
#define _MINESWEEPER_



#include "IGameState.h"
#include "RTTI.h"
#include "Menu.h"
#include "IGrid.h"
#include <stack>

enum class GameStatus
{
	Playing,
	Won,
	Lost,
};

struct Tile
{
	Tile() : selsected(false), mine(false), marked(false), minesNearby(0) {}

	bool selsected;
	bool mine;
	bool marked;
	unsigned int minesNearby;
};

class Grid : public IGrid<Tile>
{
public:

	Grid(float width, float height, unsigned int tileWidth, unsigned int tileHeight);

	virtual int Update(IKMInput&);

	virtual void Render(IRenderer&);

	void Reset();

protected:

	virtual void RenderTileCallback(IRenderer&,const Tile&, const glm::mat4&) const;

private:

	unsigned int m_uiMineCount;
	unsigned int m_uiMarkedCount;
	unsigned int m_uiMarkedCorrectlyCount;

	// Expands all zero tiles if the user clicks a zero tile
	void Expand(const glm::ivec2& pos);

	void BuildGrid();

};

class MineSweeper : public IGameState
{
public:

	RTTI_DECL;

	MineSweeper();

	virtual DLLType GetPluginType() const { return DLLType::Game; }

	// returns the name of the plug-in
	virtual const char* GetName() const { return "MineSweeper"; }

	// returns the version number of the plugin
	virtual int GetVersion() const { return 0; }

	virtual void Init(class asIScriptEngine*) {}
	virtual void Destroy(class asIScriptEngine*) {}

	// Called only once when the plugin is created
	virtual void Init(class Game& game);

	// Called only once when the plugin is destroyed
	virtual void Destroy(class Game& game);

	// Called every frame to update the date of the game
	virtual void Update(class Game& game);

	// Called every frame to render the game
	virtual void Draw(class Game& game);

private:

	void CreatePlayingMenu(Game&);
	void CreateMainMenu(Game&);

	void Reset();

	Grid m_grid;
	UI::GUI m_gui;
	UI::GUI m_mainMenu;
	GameStatus m_gameState;
	double m_fTime;


};

#endif // _MINESWEEPER_
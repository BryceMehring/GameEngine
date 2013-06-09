#ifndef _MINESWEEPER_
#define _MINESWEEPER_



#include "IGameState.h"
#include "RTTI.h"
#include "Menu.h"

class Grid
{
public:

	Grid();

	void Render(IRenderer&);

private:

	struct Tile
	{
		Tile() : selsected(false), mine(false), minesNearby(0) {}

		bool selsected;
		bool mine;
		unsigned int minesNearby;
	};

	std::vector<Tile> m_tiles;

};

class MineSweeper : public IGameState
{
public:

	RTTI_DECL;

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

	Grid m_grid;

};

#endif // _MINESWEEPER_
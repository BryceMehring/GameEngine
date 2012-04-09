#ifndef _GAMEOFLIFE_
#define _GAMEOFLIFE_

#include "GameState.h"
#include "d3dx9math.h"
#include "DxPolygon.h"
#include "QuadTree.h"
#include "Menu.h"

class GameOfLife : public IGameState
{
public:

	GameOfLife();

	virtual void Init(class Game* pGame);
	virtual void Destroy(class Game* pGame);
	virtual void Update(class Game* pGame);
	virtual void Draw(class Game* pGame);

private:

	QuadTree* m_pQuadTree;
	std::vector<class Unit*> m_units;
	D3DXVECTOR2 m_VEC[4];
	UI m_ui;

	void BuildMenu(Game* pGame);

};

#endif _GAMEOFLIFE_
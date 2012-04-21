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

	GameOfLife(class Game* pGame);
	virtual ~GameOfLife();

	virtual void Init();
	virtual void Destroy();
	virtual void Update();
	virtual void Draw();

	virtual const char* GetName() const { return "GameOfLife"; }

private:

	QuadTree* m_pQuadTree;
	std::vector<ISpatialObject*> m_units;
	D3DXVECTOR2 m_VEC[4];
	UI m_ui;

	class Mouse* m_pMouse;
	POINT m_pos;
	float m_fTime;

	void BuildMenu(Game* pGame);

};

#endif _GAMEOFLIFE_
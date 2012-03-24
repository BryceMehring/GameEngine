#ifndef _GAMEOFLIFE_
#define _GAMEOFLIFE_

#include "GameState.h"
#include "d3dx9math.h"
#include "DxPolygon.h"

class GameOfLife : public IGameState
{
public:

	GameOfLife();
	virtual ~GameOfLife();

	virtual void Init(class Game* pGame);
	virtual void Destroy(class Game* pGame);
	virtual void Update(class Game* pGame);
	virtual void Draw(class Game* pGame);

private:

	D3DXVECTOR2 m_VEC[4];

};

#endif _GAMEOFLIFE_
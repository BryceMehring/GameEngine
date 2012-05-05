#ifndef _GAMEOFLIFE_
#define _GAMEOFLIFE_

#include "GameState.h"
#include "d3dx9math.h"
#include "DxPolygon.h"

#include "RTTI.h"

class GameOfLife : public IGameState
{
public:

	RTTI_DECL;

	GameOfLife();
	virtual ~GameOfLife();

	virtual void Init(class Game*);
	virtual void Destroy(class Game*);
	virtual void Update(class Game*);
	virtual void Draw(class Game*);

private:

	class QuadTree* m_pQuadTree;
	std::vector<class ISpatialObject*> m_units;
	D3DXVECTOR2 m_VEC[4];

	class Mouse* m_pMouse;
	POINT m_pos;
	float m_fTime;

	void BuildMenu(Game* pGame);

};

#endif // _GAMEOFLIFE_
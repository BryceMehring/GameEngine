#ifndef _GAMEOFLIFE_
#define _GAMEOFLIFE_

#include "GameState.h"
#include "d3dx9math.h"
#include "DxPolygon.h"
#include "QuadTree.h"

#include "RTTI.h"

class GOL : public IGameState
{
public:

	RTTI_DECL;

	GOL();
	virtual ~GOL();

	virtual void Init(class Game& game);
	virtual void Destroy(class Game& game);
	virtual void Update(class Game& game);
	virtual void Draw(class Game& game);

private:

	class QuadTree* m_pQuadTree;
	std::vector<class Cell*> m_units;

	class Mouse* m_pMouse;
	POINT m_pos;
	float m_fTime;

	bool m_bDrawQuadTree;

	void BuildMenu(Game* pGame);

	void RegisterScript(class Game&);
	void Reset();

};

#endif // _GAMEOFLIFE_
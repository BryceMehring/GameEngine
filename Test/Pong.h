#include "GameState.h"
#include "RTTI.h"
#include "VecMath.h"
#include "QuadTree.h"
#include "BNew.h"
#include "Menu.h"
#include <vector>
#include <list>

class Ball : public ISpatialObject, public Pooled
{
public:

	Ball(const D3DXVECTOR2& pos, const D3DXVECTOR2& dir, float V, float R);

	virtual void Update(QuadTree* pTree, double dt);
	virtual void Render(class IRenderer* pRenderer);

	virtual Type GetType() const { return ISpatialObject::Unit; }

	virtual D3DXVECTOR2 GetPos() const
	{
		return m_pos;
	}
	
	const D3DXVECTOR2& GetDir() const { return m_dir; }

protected:

	D3DXVECTOR2 m_pos;
	D3DXVECTOR2 m_dir;
	float m_fSpeed;
	float m_fR;

	bool m_bChangeColor;
	double m_fTime;

	bool m_bCollision;
	double m_fCTime;

};

class Paddle : public ISpatialObject
{
public:

	Paddle(const FRECT& pos);
	virtual ~Paddle() {}
	virtual void Update(class IKMInput* pInput, QuadTree* pTree, double dt) = 0;
	virtual void Render(class IRenderer* pRenderer);

	void IncreaseScore() { ++m_iScore; }
	int GetScore() const { return m_iScore; }

	virtual Type GetType() const { return ISpatialObject::AnotherUnit; }

	virtual D3DXVECTOR2 GetPos() const
	{
		return m_pos;
	}

protected:

	D3DXVECTOR2 m_pos;
	int m_iScore;
};

class PlayerPaddle : public Paddle
{
public:

	PlayerPaddle(const FRECT& pos);

	virtual void Update(IKMInput* pInput, QuadTree* pTree, double dt);
	
};

class ComputerPaddle : public Paddle
{
public:

	ComputerPaddle(const FRECT& pos);

	virtual void Update(IKMInput* pInput, QuadTree* pTree, double dt);

protected:

	float m_fVelocity;
	double m_fTime;

	float m_fFinalPosY;

};

class Pong : public IGameState
{
public:

	RTTI_DECL;

	Pong();
	virtual ~Pong();

	virtual void Init(class Game*);
	virtual void Destroy(class Game*);
	virtual void Update(class Game*);
	virtual void Draw(class Game*);

private:

	// data members

	Paddle* m_pLeftPaddle;
	Paddle* m_pRightPaddle;
	std::list<Ball*> m_balls;

	QuadTree* m_pQuadTree;

	GUI m_gui;

	bool m_bDrawQuadTree;

	::D3DXVECTOR2 m_MouseClick;
	D3DXVECTOR2 m_MousePos;

	int m_iLeftScore;
	int m_iRightScore;

	// helper functions

	void BuildMenu();

	void RegisterScript(class Game*);

	void UpdateBalls(Game* pGame);
	void UpdatePaddles(Game* pGame);
	void DrawScore(Game* pGame);

	void ResetScores();

	void IncreasePaddlePoints(const std::vector<Paddle*>& ref);

};

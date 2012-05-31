#include "GameState.h"
#include "RTTI.h"
#include "VecMath.h"
#include "QuadTree.h"
#include <vector>
#include <list>

class Ball : public ISpatialObject
{
public:

	Ball(const D3DXVECTOR2& pos, const D3DXVECTOR2& dir, float V, float R);

	virtual void Update(double dt);
	virtual void Render(class IRenderer* pRenderer);

	virtual Type GetType() const { return ISpatialObject::Unit; }

	virtual D3DXVECTOR2 GetPos() const
	{
		return m_pos;
	}

protected:

	D3DXVECTOR2 m_pos;
	D3DXVECTOR2 m_dir;
	float m_fSpeed;
	float m_fR;

};

class Paddle : public ISpatialObject
{
public:

	Paddle(const D3DXVECTOR2& pos);
	virtual ~Paddle() {}
	virtual void Update(class IKMInput* pInput, double dt) = 0;
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

	void GetRect(D3DXVECTOR2& topLeft, D3DXVECTOR2& bottomRight);

};

class PlayerPaddle : public Paddle
{
public:

	PlayerPaddle(const D3DXVECTOR2& pos);

	virtual void Update(IKMInput* pInput, double dt);
	
};

class ComputerPaddle : public Paddle
{
public:

	virtual void Update(IKMInput* pInput, double dt);

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

	std::vector<Paddle*> m_LeftPaddles;
	std::vector<Paddle*> m_RightPaddles;
	std::list<Ball*> m_balls;

	QuadTree* m_pQuadTree;

	int m_iLeftScore;
	int m_iRightScore;

	// helper functions

	void UpdateBalls(Game* pGame);
	void UpdatePaddles(Game* pGame);
	void DrawScore(Game* pGame);

	void IncreasePaddlePoints(const std::vector<Paddle*>& ref);

};

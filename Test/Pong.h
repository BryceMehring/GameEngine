#include "GameStateScript.h"
#include "RTTI.h"
#include "VecMath.h"
#include "QuadTree.h"
#include "BNew.h"
#include "Menu.h"
#include "GameStateConstants.h"
#include "SerializedState.h"
#include <vector>
#include <list>
#include <map>
#include <boost\any.hpp>

class Ball : public ISpatialObject, public Pooled
{
public:

	Ball(const D3DXVECTOR2& pos, const D3DXVECTOR2& dir, float V, float R, const std::string& texture);

	virtual void Update(QuadTree&, double dt);
	virtual void Render(class IRenderer&);

	virtual Type GetType() const;

	virtual D3DXVECTOR2 GetPos() const
	{
		return m_pos;
	}

	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const;
	
	const D3DXVECTOR2& GetDir() const { return m_dir; }

protected:

	Math::CCircle m_CollisionPolygon;

	D3DXVECTOR2 m_pos;
	D3DXVECTOR2 m_dir;
	float m_fSpeed;
	float m_fR;

	std::string m_texture;

	bool m_bChangeColor;
	double m_fTime;

	bool m_bCollision;
	double m_fCTime;

};

class Paddle : public ISpatialObject
{
public:

	Paddle(const Math::FRECT& pos);
	virtual ~Paddle() {}

	virtual void Update(class IKMInput&, QuadTree&, double dt);
	virtual void Render(class IRenderer&);

	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const;

	void IncreaseScore() { ++m_iScore; }
	int GetScore() const { return m_iScore; }

	virtual Type GetType() const { return ISpatialObject::AnotherUnit; }

	virtual D3DXVECTOR2 GetPos() const
	{
		return m_pos;
	}

protected:

	Math::CRectangle m_CollisionPolygon;

	D3DXVECTOR2 m_pos;
	int m_iScore;
};

class PlayerPaddle : public Paddle
{
public:

	PlayerPaddle(const Math::FRECT& pos);

	virtual void Update(class IKMInput&, QuadTree&, double dt);
	
};

class ComputerPaddle : public Paddle
{
public:

	ComputerPaddle(const Math::FRECT& pos, const float& fAIspeed);

	virtual void Update(class IKMInput&, QuadTree&, double dt);

protected:

	float m_fVelocity;
	const float& m_fAISpeed;

	double m_fTime;

	float m_fFinalPosY;

};


template< class T >
class IConstants
{
public:

	friend T;

	// todo: everything sould just inherit from the same base class

	bool GetConstant(const std::string& str, boost::any& out) const
	{
		return m_const.GetConstant(str,out);
	}

private:

	GameStateConstants<boost::any> m_const;

};


class Pong : public GameStateScript, public SerializedState
{
public:

	RTTI_DECL;

	Pong();
	virtual ~Pong();

	virtual void Init(class Game&);
	virtual void Destroy(class Game&);
	virtual void Update(class Game&);
	virtual void Draw(class Game&);

	virtual void Save();

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

	float m_aiLevel;

	std::string m_texture;

	int m_iLeftScore;
	int m_iRightScore;

	IConstants<Pong> m_const;

	// helper functions

	void BuildMenu();

	void RegisterScript(class Game&);

	void UpdateBalls(Game&);
	void UpdatePaddles(Game&);
	void DrawScore(Game&);

	void ResetScores();
	void ClearObjects();

	void SetAILevel(float);

};

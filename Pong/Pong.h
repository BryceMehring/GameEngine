
#define PLUGIN_EXPORTS
#include "PluginManager.h"
#include "GameStateScript.h"
#include "RTTI.h"
#include "VecMath.h"
#include "QuadTree.h"
#include "BNew.h"
#include "Menu.h"
#include "SerializedState.h"
#include "PooledAllocator.h"
#include <vector>
#include <list>
#include <map>

class Ball : public ISpatialObject, public Pooled
{
public:

	Ball(const D3DXVECTOR2& pos, const D3DXVECTOR2& dir, float V, float R, const std::string& texture);

	virtual void Update(QuadTree&, double dt);
	virtual void Render(class IRenderer&);

	virtual void* QueryInterface(unsigned int i) const
	{
		if(i == INTERFACE_BALL)
		{
			return (void*)(this);
		}

		return nullptr;
	}

	virtual D3DXVECTOR2 GetPos() const
	{
		return m_pos;
	}
	virtual D3DXVECTOR2 GetDir() const { return m_dir; }
	virtual float GetSpeed() const { return m_fSpeed; }

	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const;
	
	static const unsigned int INTERFACE_BALL = 0xe8cbe6e9;

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

class AniBall : public Ball
{
public:

	AniBall(const D3DXVECTOR2& pos, const D3DXVECTOR2& dir, float V, float R, const std::string& texture, unsigned uiCells);

	virtual void Update(QuadTree&, double dt);
	virtual void Render(class IRenderer&);

private:

	double m_AniTime;
	unsigned int m_uiMaxCells;
	unsigned int m_uiCurrentCell;

};

class Paddle : public ISpatialObject
{
public:

	Paddle(const Math::Sprite&);
	virtual ~Paddle() {}

	virtual void Update(class IKMInput&, QuadTree&, double dt);
	virtual void Render(class IRenderer&);

	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const;

	void IncreaseScore() { ++m_iScore; }
	int GetScore() const { return m_iScore; }

	virtual D3DXVECTOR2 GetPos() const
	{
		return m_pos;
	}

	// todo: maybe I could actually return the dir in the future based off of its movement
	virtual D3DXVECTOR2 GetDir() const { return D3DXVECTOR2(0.0f,0.0f); }
	virtual float GetSpeed() const { return 0.0f; }

	virtual void* QueryInterface(unsigned int i) const
	{
		if(i == INTERFACE_PADDLE)
		{
			return (void*)this;
		}

		return nullptr;
	}

	static const unsigned int INTERFACE_PADDLE = 714198838;

protected:

	Math::CRectangle m_CollisionPolygon;

	D3DXVECTOR2 m_pos;
	int m_iScore;
};

class PlayerPaddle : public Paddle
{
public:

	PlayerPaddle(const Math::Sprite&);

	virtual void Update(class IKMInput&, QuadTree&, double dt);
	
};

class ComputerPaddle : public Paddle
{
public:

	ComputerPaddle(const Math::Sprite&, const float& fAISpeed);

	virtual void Update(class IKMInput&, QuadTree&, double dt);

protected:

	float m_fVelocity;
	const float& m_fAISpeed;

	double m_fTime;

	float m_fFinalPosY;

};

// todo: this class needs to be subdivided into a menu class and a game class
class Pong : public GameStateScript, public SerializedState
{
public:

	RTTI_DECL;

	Pong(PluginManager& mgr);
	virtual ~Pong();

	virtual DLLType GetPluginType() const { return DLLType::GamePlugin; }
	virtual const char* GetName() const { return "Pong"; }
	virtual int GetVersion() const { return 0; }

	// displays a info box about the plug-in
	virtual void About() const {}

	virtual void Init(class Game&);
	virtual void Destroy(class Game&);
	virtual void Update(class Game&);
	virtual void Draw(class Game&);

	virtual void Save();

private:

	enum State
	{
		MENU,
		GAME
	};

	State m_state;

	// data members

	Paddle* m_pLeftPaddle;
	Paddle* m_pRightPaddle;
	std::list<Ball*,PooledAllocator<Ball*>> m_balls;

	QuadTree* m_pQuadTree;

	GUI m_gui;

	bool m_bDrawQuadTree;

	::D3DXVECTOR2 m_MouseClick;
	D3DXVECTOR2 m_MousePos;

	float m_aiLevel;

	float m_fMinBallVelocity;
	float m_fMaxBallVelocity;

	std::string m_texture;

	int m_iLeftScore;
	int m_iRightScore;

	// helper functions

	void RegisterScript(class Game&);

	void UpdateMenu(Game&);
	void UpdateGame(Game&);
	void DrawMenu(Game&);
	void DrawGame(Game&);

	void BuildMenu(Game& game);

	void UpdateBalls(Game&);
	void UpdatePaddles(Game&);
	void DrawScore(Game&);

	void ResetScores();
	void ClearObjects();

	unsigned int GetNumBalls() const;

	void SetAILevel(float);

};

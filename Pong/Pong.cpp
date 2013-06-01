
#include "Pong.h"
#include "Game.h"
#include "IResourceManager.h"
#include "asVM.h"
#include "FileManager.h"
#include "StringAlgorithms.h"
#include <ctime>
#include <sstream>
#include <fstream>

#include "GameStateCreator.h"

#include <boost/lexical_cast.hpp>

using namespace std;

#pragma comment(lib,"d3dx9.lib")

PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr)
{
	return new Pong(mgr);
}

// Ball class

const D3DXVECTOR2 normal[4] = 
{
	D3DXVECTOR2(1.0f,0.0f),
	D3DXVECTOR2(-1.0f,0.0f),
	D3DXVECTOR2(0.0f,-1.0f),
	D3DXVECTOR2(0.0f,1.0f)
};

Ball::Ball(const D3DXVECTOR2& pos, const D3DXVECTOR2& dir, float V, float R, const std::string& texture) 
: m_pos(pos), m_dir(dir), m_fSpeed(V), m_fR(R), m_bChangeColor(false), m_texture(texture)
, m_fTime(0.0), m_fCTime(0.0), m_bCollision(false), m_CollisionPolygon(Math::Circle(pos,R))
{
}


const Math::ICollisionPolygon& Ball::GetCollisionPolygon() const
{
	return m_CollisionPolygon;
}


void Ball::Update(QuadTree& tree, double dt)
{
	if(m_bChangeColor)
	{
		m_fTime += dt;

		if(m_fTime > 0.4)
		{
			m_fTime = 0.0;
			m_bChangeColor = false;
		}
	}

	const Math::FRECT& R = tree.GetRect();

	unsigned int index = -1;

	if(m_pos.y > R.topLeft.y)
	{
		//m_pos.y = 600.0f;
		index = 2;
	}
	else if(m_pos.y < R.bottomRight.y)
	{
		//m_pos.y = 0.0f;
		index = 3;
		//G = D3DXVECTOR2(0.0f,-1.2f);
	}

	if(index != -1)
	{
		m_dir = Math::Reflect(-m_dir,normal[index]);
	}

	if(!m_bChangeColor)
	{
		std::vector<::ISpatialObject*> objs;
		tree.QueryNearObjects(GetCollisionPolygon(),objs);

		for(unsigned int i = 0; i < objs.size(); ++i)
		{
			if(objs[i] != this)
			{
				D3DXVECTOR2 velocity = m_fSpeed*this->m_dir;
				//D3DXVECTOR2 oldPos = m_pos;
				//Math::CCircle oldCircle = m_CollisionPolygon;
				//for(unsigned int j = 0; j < 4; ++j)
				{
					//oldPos += m_dir*(m_fSpeed / 4.0f)*dt;
					//oldCircle.GetCircle() = oldPos;

					if(m_CollisionPolygon.Intersects(objs[i]->GetCollisionPolygon()))
					{
						D3DXVECTOR2 normal;
						objs[i]->GetCollisionPolygon().GetNormal(m_pos,normal);

						m_dir = Math::Reflect(-m_dir,normal);
						m_bChangeColor = true;
						m_bCollision = true;
						break;
					}
				}
			}
		}
	}

	m_pos += m_dir*m_fSpeed*dt;

	tree.Erase(*this);
	m_CollisionPolygon.GetCircle().center = m_pos;
	tree.Insert(*this);
}

void Ball::Render(IRenderer& renderer)
{
	D3DXMATRIX S, T;
	D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);
	D3DXMatrixScaling(&S,m_fR,m_fR,1.0f);

	renderer.Get2DRenderer().DrawSprite(S*T,m_texture);
}

AniBall::AniBall(const D3DXVECTOR2& pos, const D3DXVECTOR2& dir, float V, float R, const std::string& texture, unsigned uiCells) :
Ball(pos,dir,V,R,texture), m_uiMaxCells(uiCells), m_uiCurrentCell(0), m_AniTime(0.0) {}

void AniBall::Update(QuadTree& tree, double dt)
{
	m_AniTime += dt;

	if(m_AniTime > (5.0f / m_fSpeed))
	{
		m_uiCurrentCell = (m_uiCurrentCell + 1) % (m_uiMaxCells);
		m_AniTime = 0;
	}

	Ball::Update(tree,dt);
}

void AniBall::Render(IRenderer& renderer)
{
	float fScale = m_fR * 2.0f;

	::D3DXMATRIX S, T, R;
	::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);
	::D3DXMatrixScaling(&S,fScale,fScale,1.0f);
	::D3DXMatrixRotationZ(&R,atan2(m_dir.y,m_dir.x));

	//::D3DXVECTOR2 pos(m_pos);
	//pos += D3DXVECTOR2(-5.0f,10.0f);

	//::ostringstream stream;
	//stream <<"("<< m_dir.x << "," << m_dir.y << ")" ;

	renderer.Get2DRenderer().DrawSprite(R*S*T,m_texture,m_uiCurrentCell);
	//renderer.Get2DRenderer().DrawString(stream.str().c_str(),pos);
}


// paddle class
Paddle::Paddle(const Math::Sprite& sprite)
: m_pos(sprite.Middle()),m_iScore(0), m_CollisionPolygon(sprite)
{
}

const Math::ICollisionPolygon& Paddle::GetCollisionPolygon() const
{
	return m_CollisionPolygon;
}

void Paddle::Update(IKMInput& input, QuadTree& tree, double dt)
{
	tree.Erase(*this);

	m_CollisionPolygon.GetRect() = m_pos;

	tree.Insert(*this);
}

void Paddle::Render(IRenderer& renderer)
{
	::D3DXMATRIX S,T;
	::D3DXMatrixScaling(&S,m_CollisionPolygon.GetRect().Width(),m_CollisionPolygon.GetRect().Height(),1.0f);
	::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);

	renderer.Get2DRenderer().DrawSprite(S*T,"paddle");
}

PlayerPaddle::PlayerPaddle(const Math::Sprite& sprite) : Paddle(sprite) {}

void PlayerPaddle::Update(IKMInput& input, QuadTree& tree, double dt)
{
	m_pos.y = input.GetTransformedMousePos().y;

	const Math::FRECT& R = tree.GetRect();
	m_pos.y = Math::Clamp(m_pos.y,(float)R.bottomRight.y,(float)R.topLeft.y);

	Paddle::Update(input,tree,dt);
}

ComputerPaddle::ComputerPaddle(const Math::Sprite& sprite, const float& fAISpeed)
: Paddle(sprite), m_fTime(0.0), m_fVelocity(0.0f), m_fFinalPosY(-1.0f), m_fAISpeed(fAISpeed)
{

}

void ComputerPaddle::Update(IKMInput& input, QuadTree& tree, double dt)
{
	const Math::FRECT& R = tree.GetRect();

	// Get rect of area to poll for balls
	Math::CRectangle cRect(Math::FRECT(::D3DXVECTOR2(-50.0f,50.0f),D3DXVECTOR2(-5.0f,-50.0f)));

	// poll every 1 seconds
	m_fTime += dt;
	if(m_fTime >= .2f)
	{
		m_fTime = 0.0;

		// find target

		std::vector<ISpatialObject*> nodes;
		tree.QueryNearObjects(cRect,nodes);

		ISpatialObject* pTarget = nullptr;
		float fTime = FLT_MAX;

		// Loop over all objects in the area
		for(unsigned int i = 0; i < nodes.size(); ++i)
		{
			ISpatialObject* pObj = nodes[i];

			// If the object is not a paddle
			if(pObj->QueryInterface(Paddle::INTERFACE_PADDLE) == nullptr)
			{
				// If the object is heading to the left
				if(pObj->GetDir().x < 0)
				{
					D3DXVECTOR2 ballPos = pObj->GetPos();
					D3DXVECTOR2 dir = pObj->GetDir();
					float speed = pObj->GetSpeed();

					// calculate when the object will hit the paddle
					float xVelocity = dir.x*speed;
					float xDistance = ballPos.x - 50.0f;
					float fTempTime = abs(xDistance / xVelocity);

					// If this object will hit sooner than the previous
					if(fTempTime < fTime)
					{
						// set pObj as the new target
						pTarget = pObj;
						fTime = fTempTime;
					}
				}
			}

		}

		if(pTarget != nullptr)
		{
			m_fFinalPosY = Math::PongRayTrace(pTarget->GetPos(),pTarget->GetDir(),m_pos.x);
		}
		else
		{
			m_fFinalPosY = -1.0f;
			m_fVelocity = 0.0f;
		}

	}

	if(m_fFinalPosY != -1.0f)
	{
		m_fVelocity = (m_fFinalPosY - m_pos.y);
		m_pos.y += m_fAISpeed*m_fVelocity * dt;

		//m_fVelocity = (m_fFinalPosY - m_pos.y);
		//m_pos.y = m_fFinalPosY;
	}

	Paddle::Update(input,tree,dt);

}



// Pong state

RTTI_IMPL(Pong);

Pong::Pong(PluginManager& mgr) : m_iLeftScore(0), m_iRightScore(0),
m_pRightPaddle(nullptr), m_pLeftPaddle(nullptr), m_bDrawQuadTree(false), m_state(MENU), SerializedState("..//Scripts/pong/pong.as"), m_aiLevel(5.0f)
{
}

Pong::~Pong()
{
	delete m_pLeftPaddle;
	delete m_pRightPaddle;

	ClearObjects();

	delete m_pQuadTree;
}

void Pong::Init(Game& game)
{
	// first register the game with script
	RegisterScript(game);

	// Load Config file
	Load(game.GetAs());

	// Load Init script
	GameStateScript::Init(game);

	IRenderer& renderer = game.GetRenderer();

	// build the quadtree
	m_pQuadTree = new QuadTree(Math::FRECT(D3DXVECTOR2(-50.0f,50.0f),D3DXVECTOR2(50.0f,-50.0)));

	// build paddles and add them to the quadtree
	m_pRightPaddle = new PlayerPaddle(Math::Sprite(3.0f,20.0f,D3DXVECTOR2(45.0f,0.0f)));
	m_pQuadTree->Insert(*m_pRightPaddle);

	m_pLeftPaddle = new ComputerPaddle(Math::Sprite(3.0f,20.0f,D3DXVECTOR2(-45.0f,0.0f)),m_aiLevel);
	m_pQuadTree->Insert(*m_pLeftPaddle);

	// todo: maybe implement this in the future
	BuildMenu(game);
}


void Pong::Save()
{
	auto iter = m_scriptFile.find("pong.fAILevel");
	if(iter != m_scriptFile.end())
	{
		iter->second = boost::lexical_cast<string>(m_aiLevel);
	}

	iter = m_scriptFile.find("pong.sBallTex");
	if(iter != m_scriptFile.end())
	{
		iter->second = WrapWithQuote(m_texture);
	}

	iter = m_scriptFile.find("pong.fMaxBallVelocity");
	if(iter != m_scriptFile.end())
	{
		iter->second = boost::lexical_cast<string>(m_fMaxBallVelocity);
	}

	iter = m_scriptFile.find("pong.fMinBallVelocity");
	if(iter != m_scriptFile.end())
	{
		iter->second = boost::lexical_cast<string>(m_fMinBallVelocity);
	}

	SerializedState::SaveMap();
}

void Pong::Destroy(Game& game)
{
	//::ShowCursor(TRUE);

	// Save config file
	Save();

	// Load Destroy script
	GameStateScript::Destroy(game);

	asIScriptEngine* pScriptEngine = game.GetAs().GetScriptEngine();

	DBAS(pScriptEngine->GarbageCollect());

	DBAS(pScriptEngine->RemoveConfigGroup(s_rtti.GetName().c_str()));

	pScriptEngine->Release();
}

void Pong::Update(Game& game)
{
	IKMInput& input = game.GetInput();

	if(input.KeyDown(ESCAPE))
	{
		m_state = (m_state == MENU) ? GAME : MENU;
		//game.SetNextState("PongMenu");
	}

	UpdateGame(game);

	/*switch(m_state)
	{
		case MENU:
			UpdateMenu(game);
			break;
		case GAME:
			UpdateGame(game);
			break;
	}*/
 }

void Pong::UpdateGame(Game& game)
{
	IKMInput& input = game.GetInput();

	if(input.KeyDown(SPACE))
	{
		m_bDrawQuadTree = !m_bDrawQuadTree;
	}

	if(input.MouseClick(0))
	{
		const Math::FRECT& R = this->m_pQuadTree->GetRect();

		// add new ball
		POINT P = input.MousePos();
		//float a = Math::GetRandFloat(0.0f,365.0f) * 0.01745329f;
		float a = Math::GetRandFloat(120.0f,180.0f) * 0.01745329f;
		//float s = Math::GetRandFloat(7.0f,10.0f);
		float s = Math::GetRandFloat(3.0f,7.5f);
		float v = Math::GetRandFloat(m_fMinBallVelocity,m_fMaxBallVelocity);

		//Ball* pBall = new Ball(D3DXVECTOR2(P.x,P.y),D3DXVECTOR2(-cosf(a),sinf(a)),0.0f,s);
		//Ball* pBall = new Ball(D3DXVECTOR2(R.bottomRight.x/2.0f,R.bottomRight.y/8.0f),D3DXVECTOR2(-cosf(a),sinf(a)),v,s,this->m_texture);
		Ball* pBall = new AniBall(D3DXVECTOR2(0.0f,R.topLeft.y - 5.0f),D3DXVECTOR2(sin(a),cos(a)),v,s,"flame_sprite",6);
		m_balls.push_back(pBall);
		m_pQuadTree->Insert(*pBall);
	}

	//m_pQuadTree->Update();
	//m_pQuadTree->Update();

	//m_gui.Update(pInput,pGame->GetDt());

	UpdatePaddles(game);
	UpdateBalls(game);
}

void Pong::UpdateMenu(Game& game)
{
	m_gui.Update(game.GetInput(),game.GetDt());
}

void Pong::BuildMenu(Game& game)
{
	// todo: need to finish implementing

	/*Menu* pMenu = new Menu();

	RECT R;
	GetWindowRect(R);

	const unsigned int uiWidth = R.right - R.left;
	const unsigned int uiHeight = R.bottom - R.top;

	RECT SquareButtonRect = {R.left + uiWidth / 3,R.top,R.left + (2 * uiWidth / 3),R.top + 30};
	SquareButton<void>* pButton = new SquareButton<void>(SquareButtonRect,"Reset Objects");
	pButton->SetCallback(SquareButton<void>::DELEGATE(this,&Pong::ClearObjects));

	pMenu->AddElement(pButton);

	m_gui.SetMenu(pMenu);*/
	
}

void Pong::SetAILevel(float f)
{
	m_aiLevel = f;
}

unsigned int Pong::GetNumBalls() const
{
	return m_balls.size();
}

void Pong::UpdateBalls(Game& game)
{
	// Update balls
	auto iter = m_balls.begin();
	const Math::FRECT& R = m_pQuadTree->GetRect();

	// Loop over all of the balls
	while(iter != m_balls.end())
	{
		Ball* pBall = *iter;

		pBall->Update(*m_pQuadTree,game.GetDt());

		if(pBall->GetPos().x < (float)R.topLeft.x || pBall->GetPos().x > (float)R.bottomRight.x)
		{
			if(!m_pQuadTree->IsWithin(*pBall))
			{
				if(pBall->GetPos().x < (float)R.topLeft.x)
				{
					m_iRightScore++;
				}
				else if(pBall->GetPos().x > (float)R.bottomRight.x)
				{
					m_iLeftScore++;
				}

				delete pBall;
				iter = m_balls.erase(iter);
			}
		}
		else
		{
			++iter;
		}
	}
}

void Pong::UpdatePaddles(Game& game)
{
	IKMInput& input = game.GetInput();
	double dt = game.GetDt();

	if(m_pLeftPaddle)
	{
		m_pLeftPaddle->Update(input,*m_pQuadTree,dt);
	}

	if(m_pRightPaddle)
	{
		m_pRightPaddle->Update(input,*m_pQuadTree,dt);
	}
}

void Pong::Draw(Game& game)
{
	DrawGame(game);

	/*switch(m_state)
	{
		case MENU:
			DrawMenu(game);
			break;
		case GAME:
			DrawGame(game);
			break;
	}*/
}

void Pong::DrawGame(Game& game)
{
	IRenderer& renderer = game.GetRenderer();

	if(m_pLeftPaddle)
	{
		m_pLeftPaddle->Render(renderer);
	}

	if(m_pRightPaddle)
	{
		m_pRightPaddle->Render(renderer);
	}

	for_each(m_balls.begin(),m_balls.end(),[&](Ball* pBall)
	{
		pBall->Render(renderer);
	});

	DrawScore(game);

	//m_gui.Render(pRenderer);

	if(m_bDrawQuadTree)
	{
		m_pQuadTree->Render(renderer);
	}
}

void Pong::DrawMenu(Game& game)
{
	m_gui.Render(game.GetRenderer());
}

void Pong::DrawScore(Game& game)
{
	IRenderer& renderer = game.GetRenderer();

	RECT R;
	GetClientRect(::GetActiveWindow(),&R);

	const D3DXVECTOR2 POS[2] = 
	{
		D3DXVECTOR2(-40.0f,40.0f),
		D3DXVECTOR2(40.0f,40.0f),
	};

	std::stringstream stream;
	stream << m_iLeftScore << '\t' << '\t' << '\t' << m_iRightScore;

	renderer.Get2DRenderer().DrawString(stream.str().c_str(),POS[0]);
	

}

void Pong::ResetScores()
{
	m_iLeftScore = m_iRightScore = 0;
}

void Pong::ClearObjects()
{
	for_each(m_balls.begin(),m_balls.end(),[&](Ball* pBall)
	{
		m_pQuadTree->Erase(*pBall);
		delete pBall;
	});

	m_balls.clear();
}

void Pong::RegisterScript(Game& game)
{
	// register the pong interface in script within a ConfigGroup

	asIScriptEngine* pScriptEngine = game.GetAs().GetScriptEngine();

	const char* pName = s_rtti.GetName().c_str();

	DBAS(pScriptEngine->BeginConfigGroup(pName));

	DBAS(pScriptEngine->RegisterObjectType(pName,0,asOBJ_REF | asOBJ_NOHANDLE));

	// methods
	DBAS(pScriptEngine->RegisterObjectMethod(pName,"void ResetScores()",asMETHOD(Pong,ResetScores),asCALL_THISCALL));
	DBAS(pScriptEngine->RegisterObjectMethod(pName,"void ResetObjects(void)",asMETHOD(Pong,ClearObjects),asCALL_THISCALL));
	DBAS(pScriptEngine->RegisterObjectMethod(pName,"void SetAILevel(float)",asMETHOD(Pong,SetAILevel),asCALL_THISCALL));
	DBAS(pScriptEngine->RegisterObjectMethod(pName,"uint GetNumBalls() const",asMETHOD(Pong,GetNumBalls),asCALL_THISCALL));

	// properties
	DBAS(pScriptEngine->RegisterObjectProperty(pName,"float fAILevel",asOFFSET(Pong,m_aiLevel)));
	DBAS(pScriptEngine->RegisterObjectProperty(pName,"string sBallTex",asOFFSET(Pong,m_texture)));
	DBAS(pScriptEngine->RegisterObjectProperty(pName,"float fMinBallVelocity",asOFFSET(Pong,m_fMinBallVelocity)));
	DBAS(pScriptEngine->RegisterObjectProperty(pName,"float fMaxBallVelocity",asOFFSET(Pong,m_fMaxBallVelocity)));

	DBAS(pScriptEngine->RegisterGlobalProperty("Pong pong",(void*)this));

	DBAS(pScriptEngine->EndConfigGroup());

	pScriptEngine->Release();

}

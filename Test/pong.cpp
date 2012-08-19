#include "Pong.h"
#include "Game.h"
#include "ITextureManager.h"
#include "asVM.h"
#include "FileManager.h"
#include "StringAlgorithms.h"
#include <ctime>
#include <sstream>
#include <fstream>

#include <boost/lexical_cast.hpp>

using namespace std;

#pragma comment(lib,"d3dx9.lib")

void GetWindowRect(RECT& R)
{
	GetClientRect(::GetActiveWindow(),&R);
}

// Ball class

const D3DXVECTOR2 normal[4] = 
{
	D3DXVECTOR2(1.0f,0.0f),
	D3DXVECTOR2(-1.0f,0.0f),
	D3DXVECTOR2(0.0f,1.0f),
	D3DXVECTOR2(0.0f,-1.0f)
};

Ball::Ball(const D3DXVECTOR2& pos, const D3DXVECTOR2& dir, float V, float R, const std::string& texture) 
: m_pos(pos), m_dir(dir), m_fSpeed(V), m_fR(R), m_bChangeColor(false), m_texture(texture)
, m_fTime(0.0), m_fCTime(0.0), m_bCollision(false), m_CollisionPolygon(Math::Circle(pos,R))
{
}

Ball::Type Ball::GetType() const { return ISpatialObject::Unit; }

const Math::ICollisionPolygon& Ball::GetCollisionPolygon() const
{
	return m_CollisionPolygon;
}


void Ball::Update(QuadTree& tree, double dt)
{
	if(m_bChangeColor)
	{
		m_fTime += dt;

		if(m_fTime > 0.1)
		{
			m_fTime = 0.0;
			m_bChangeColor = false;
		}
	}

	/*if(m_bCollision)
	{
		m_fCTime += dt;
		if(m_fCTime > .1)
		{
			m_fCTime = 0.0;
			m_bCollision = false;
		}
	}*/

	const Math::FRECT& R = tree.GetRect();

	unsigned int index = -1;

	if(m_pos.y <= 0.0f)
	{
		//m_pos.y = 600.0f;
		index = 2;
	}
	else if(m_pos.y >= R.bottomRight.y)
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
		ProccessNearNodes(this->m_nodes,[&](const ::ISpatialObject* pObj) -> bool
		{
			if(pObj != this)
			{
				// todo: need to fix
				if(m_CollisionPolygon.Intersects(pObj->GetCollisionPolygon()))
				{
					D3DXVECTOR2 normal;
					pObj->GetCollisionPolygon().GetNormal(this->m_pos,normal);
					//m_pCollisionPolygon->GetNormal(pObj->GetPos(),normal);

					m_dir = Math::Reflect(-m_dir,normal);
					m_bChangeColor = true;
					m_bCollision = true;

					return true;
				}
			}

			return false;
		});
	}

	m_pos += m_dir*m_fSpeed*dt;

	m_CollisionPolygon.GetCircle().center = m_pos;

	tree.Update(*this);
}

void Ball::Render(IRenderer& renderer)
{
	::D3DXMATRIX S, T;
	::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);
	::D3DXMatrixScaling(&S,m_fR / 16.0f,m_fR/16.0f,1.0f);

	//TextureInfo info;
	//renderer.GetTextureManager().GetTextureInfo(m_texture,info);

	renderer.DrawSprite(S*T,m_texture,2,m_bChangeColor ? 0xffff0000 : 0xffffffff);
}

// paddle class
Paddle::Paddle(const Math::FRECT& pos)
: m_pos(pos.Middle()),m_iScore(0), m_CollisionPolygon(pos)
{
}

const Math::ICollisionPolygon& Paddle::GetCollisionPolygon() const
{
	return m_CollisionPolygon;
}

void Paddle::Update(IKMInput& input, QuadTree& tree, double dt)
{
	m_CollisionPolygon.GetRect() = m_pos;

	tree.Update(*this);
}

void Paddle::Render(IRenderer& renderer)
{
	::D3DXMATRIX T;
	::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);

	renderer.DrawSprite(T,"paddle",2);
}

PlayerPaddle::PlayerPaddle(const Math::FRECT& pos) : Paddle(pos) {}

void PlayerPaddle::Update(IKMInput& input, QuadTree& tree, double dt)
{
	const Math::FRECT& R = tree.GetRect();

	m_pos.y = input.MousePos().y;

	/*if(pInput->KeyDown(UP))
	{
		m_pos.y -= 500.0f * dt;
	}
	else if(pInput->KeyDown(DOWN))
	{
		m_pos.y += 500.0f * dt;
	}
	else
	{
		m_pos.y += -500.0f * pInput->MouseY() * dt;
	}*/

	m_pos.y = Math::Clamp(m_pos.y,(float)R.topLeft.y,(float)R.bottomRight.y);

	Paddle::Update(input,tree,dt);
}

ComputerPaddle::ComputerPaddle(const Math::FRECT& pos, const float& fAIspeed)
: Paddle(pos), m_fTime(0.0), m_fVelocity(0.0f), m_fFinalPosY(-1.0f), m_fAISpeed(fAIspeed)
{

}

void ComputerPaddle::Update(IKMInput& input, QuadTree& tree, double dt)
{
	const Math::FRECT& R = tree.GetRect();

	// Get rect of area to poll for balls
	Math::CRectangle cRect(Math::FRECT(::D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(R.bottomRight.x/2.0f-10.0f,R.bottomRight.y)));

	// poll every 1 seconds
	m_fTime += dt;
	if(m_fTime >= .2f)
	{
		bool bFoundBall = false;

		// find near nodes in the area
		std::vector<Node*> nodes;
		tree.FindNearNodes(&cRect,nodes);

		// loop over ISpatialObject in the area 
		ProccessNearNodes(nodes,[&](const ::ISpatialObject* pObj) -> bool
		{
			if(pObj->GetType() == ISpatialObject::Unit)
			{
				Ball* pBall = (Ball*)(pObj);
				if(pBall->GetDir().x < 0)
				{
					D3DXVECTOR2 ballPos = pBall->GetPos();
					D3DXVECTOR2 dir = pBall->GetDir();

					m_fFinalPosY = Math::PongRayTrace(ballPos,dir,50.0f);

					//m_pos.y = (y - this->m_pos.y) / 2.0f;
					bFoundBall = true;
					return true;
				}
			}
			return false;
		});

		if(!bFoundBall)
		{
			m_fFinalPosY = -1.0f;
			m_fVelocity = 0.0f;
		}

		m_fTime = 0.0;
	}

	if(m_fFinalPosY != -1.0f)
	{
		m_fVelocity = (m_fFinalPosY - m_pos.y);
		m_pos.y += m_fAISpeed*m_fVelocity * dt;
	}

	Paddle::Update(input,tree,dt);

}



// Pong state

RTTI_IMPL(Pong);

Pong::Pong(): m_iLeftScore(0), m_iRightScore(0),
m_pRightPaddle(nullptr), m_pLeftPaddle(nullptr), m_bDrawQuadTree(false), SerializedState("..//Scripts/pong.as")
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

	// Build the rect of the window
	RECT R;
	GetWindowRect(R);

	/*R.right -= R.left;
	R.bottom -= R.top;
	R.top = R.left = 0;*/

	Math::FRECT fRect(D3DXVECTOR2(R.left,R.top),D3DXVECTOR2(R.right,R.bottom));

	// build the quadtree
	m_pQuadTree = new QuadTree(fRect);

	// Get the size of the texture we are using
	TextureInfo textureInfo;
	
	renderer.GetTextureManager().GetTextureInfo("paddle",textureInfo);
	Math::FRECT rect(D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(textureInfo.uiWidth,textureInfo.uiHeight));
	rect = D3DXVECTOR2(D3DXVECTOR2(R.right - 50.0f,(R.bottom + R.top) / 2.0f));

	// build paddles and add them to the quadtree
	m_pRightPaddle = new PlayerPaddle(rect);
	m_pQuadTree->Insert(*m_pRightPaddle);

	rect = D3DXVECTOR2(D3DXVECTOR2(R.left + 50.0f,(R.bottom + R.top) / 2.0f));

	m_pLeftPaddle = new ComputerPaddle(rect,m_aiLevel);
	m_pQuadTree->Insert(*m_pLeftPaddle);

	// todo: maybe implement this in the future
	//BuildMenu();

	//::ShowCursor(FALSE);

	//Ball* pBall = new Ball(::D3DXVECTOR2(R.left / 2,R.right / 2),D3DXVECTOR2(1.0f,0.0f),100.0f,1.0f);
	//m_balls.push_back(pBall);
	//m_pQuadTree->Insert(pBall);
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

	if(input.KeyDown(SPACE))
	{
		m_bDrawQuadTree = !m_bDrawQuadTree;
	}

	if(input.KeyDown(ENTER) || input.MouseClick(0) || m_balls.empty())
	{
		const Math::FRECT& R = this->m_pQuadTree->GetRect();

		// add new ball
		POINT P = input.MousePos();
		float a = Math::GetRandFloat(135.0f,225.0f) * 0.01745329f;
		float s = Math::GetRandFloat(5.0f,8.0f);
		float v = Math::GetRandFloat(500.0f,600.0f);

		//Ball* pBall = new Ball(D3DXVECTOR2(P.x,P.y),D3DXVECTOR2(-cosf(a),sinf(a)),0.0f,s);
		Ball* pBall = new Ball(D3DXVECTOR2(R.bottomRight.x/2.0f,R.bottomRight.y/8.0f),D3DXVECTOR2(-cosf(a),sinf(a)),v,s,this->m_texture);
		m_balls.push_back(pBall);
		m_pQuadTree->Insert(*pBall);
	}

	//m_pQuadTree->Update();
	//m_pQuadTree->Update();

	//m_gui.Update(pInput,pGame->GetDt());

	UpdatePaddles(game);
	UpdateBalls(game);
}

void Pong::BuildMenu()
{
	// todo: need to finish implementing

	/*Menu* pMenu = new Menu();

	RECT R;
	GetWindowRect(R);

	const unsigned int uiWidth = R.right - R.left;
	const unsigned int uiHeight = R.bottom - R.top;

	RECT SquareButtonRect = {R.left + uiWidth / 3,R.top,R.left + (2 * uiWidth / 3),R.top + 30};
	SquareButton<void>* pButton = new SquareButton<void>(SquareButtonRect,"Test Button");

	pMenu->AddElement(pButton);

	m_gui.SetMenu(pMenu);*/
	
}

void Pong::SetAILevel(float f)
{
	m_aiLevel = f;
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

	// properties
	DBAS(pScriptEngine->RegisterObjectProperty(pName,"float fAILevel",asOFFSET(Pong,m_aiLevel)));
	DBAS(pScriptEngine->RegisterObjectProperty(pName,"string sBallTex",asOFFSET(Pong,m_texture)));

	DBAS(pScriptEngine->RegisterGlobalProperty("Pong pong",(void*)this));

	DBAS(pScriptEngine->EndConfigGroup());

	pScriptEngine->Release();

}
void Pong::UpdateBalls(Game& game)
{
	//static CRectangle rect;
	//rect.GetRect().topLeft = ::D3DXVECTOR2(0.0f,0.0f);
	//rect.GetRect().bottomRight = ::D3DXVECTOR2(800.0f,600.0f);

	//if(pGame->GetInput()->MouseClick(0))
	/*{
		std::vector<Node*> nodes;
		this->m_pQuadTree->FindNearNodes(&rect,nodes);

		for(unsigned int i = 0; i < nodes.size(); ++i)
		{
			auto pList = nodes[i]->GetNearObjects();

			for(Node::LIST_DTYPE::iterator iter = pList->begin(); iter != pList->end(); ++iter)
			{
				Ball* pBall = (Ball*)(*iter);
				pBall->Update(pGame->GetDt());
			}
		}
	}*/

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
			if(pBall->GetPos().x < (float)R.topLeft.x)
			{
				m_iRightScore++;
			}
			else if(pBall->GetPos().x > (float)R.bottomRight.x)
			{
				m_iLeftScore++;
			}

			pBall->EraseFromQuadtree(this->m_pQuadTree);

			delete pBall;
			iter = m_balls.erase(iter);
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

/*void Pong::IncreasePaddlePoints(const std::vector<Paddle*>& ref)
{
	for_each(ref.begin(),ref.end(),[](Paddle* pBall)
	{
		pBall->IncreaseScore();
	});
}*/

void Pong::Draw(Game& game)
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

void Pong::DrawScore(Game& game)
{
	IRenderer& renderer = game.GetRenderer();
	const Math::FRECT& R = m_pQuadTree->GetRect();

	const POINT POS[2] = 
	{
		{R.topLeft.x + 200,R.topLeft.y + 50},
		{R.bottomRight.x - 200,R.topLeft.y + 50}
	};

	std::stringstream stream;
	stream << m_iLeftScore << " " << m_iRightScore;

	unsigned int i = 0;
	while(stream)
	{
		std::string str;
		stream >> str;

		renderer.DrawString(str.c_str(),POS[i++],0xffffffff);
	}
}

void Pong::ResetScores()
{
	m_iLeftScore = m_iRightScore = 0;
}

void Pong::ClearObjects()
{
	for_each(m_balls.begin(),m_balls.end(),[&](Ball* pBall)
	{
		pBall->EraseFromQuadtree(m_pQuadTree);
		m_pQuadTree->Erase(*pBall);
		delete pBall;
	});

	m_balls.clear();
}
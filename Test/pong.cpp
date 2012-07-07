#include "Pong.h"
#include "Game.h"
#include "TextureManager.h"
#include "asVM.h"
#include <ctime>
#include <sstream>

using namespace std;

void GetWindowRect(RECT& R)
{
	GetClientRect(GetActiveWindow(),&R);
}

// Ball class

const D3DXVECTOR2 normal[4] = 
{
	D3DXVECTOR2(1.0f,0.0f),
	D3DXVECTOR2(-1.0f,0.0f),
	D3DXVECTOR2(0.0f,1.0f),
	D3DXVECTOR2(0.0f,-1.0f)
};

Ball::Ball(const D3DXVECTOR2& pos, const D3DXVECTOR2& dir, float V, float R) 
: m_pos(pos), m_dir(dir), m_fSpeed(V), m_fR(R), m_bChangeColor(false)
, m_fTime(0.0), m_fCTime(0.0), m_bCollision(false)
{
	SetCollisionPolygon(new CCircle(Circle(pos,R)));
}

void Ball::Update(QuadTree* pTree, double dt)
{
	if(m_bChangeColor)
	{
		m_fTime += dt;

		if(m_fTime > 0.2)
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

	RECT R;
	GetWindowRect(R);

	unsigned int index = -1;

	if(m_pos.y <= 0.0f)
	{
		//m_pos.y = 600.0f;
		index = 2;
	}
	else if(m_pos.y >= R.bottom)
	{
		//m_pos.y = 0.0f;
		index = 3;
		//G = D3DXVECTOR2(0.0f,-1.2f);
	}

	if(index != -1)
	{
		m_dir = Reflect(-m_dir,normal[index]);
	}

	pTree->Update(this);

	//std::vector<Node*> nodes;
	//pTree->Erase(this,&nodes);

	if(!m_bChangeColor)
	{
		ProccessNearNodes(this->m_nodes,[&](const ::ISpatialObject* pObj) -> bool
		{
			if(pObj != this)
			{
				if(m_pCollisionPolygon->Intersects(pObj->GetCollisionPolygon()))
				{
					D3DXVECTOR2 normal;
					pObj->GetCollisionPolygon()->GetNormal(this->m_pos,normal);
					//m_pCollisionPolygon->GetNormal(pObj->GetPos(),normal);

					m_dir = Reflect(-m_dir,normal);
					m_bChangeColor = true;
					m_bCollision = true;

					return true;
				}
			}

			return false;
		});
	}

	m_pos += m_dir*m_fSpeed*dt;

	static_cast<CCircle*>(m_pCollisionPolygon)->GetCircle().center = m_pos;

	
	
	//pTree->Insert(this);
	//static_cast<CCircle*>(m_pCollisionPolygon)->Update(m_pos,m_fR);
}

void Ball::Render(IRenderer* pRenderer)
{
	::D3DXMATRIX S, T;
	::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);
	::D3DXMatrixScaling(&S,m_fR / 16.0f,m_fR/16.0f,1.0f);

	DWORD color = D3DCOLOR_XRGB(rand() % 256,rand() % 256,rand() % 256);

	pRenderer->DrawSprite(S*T,"point",2,m_bChangeColor ? 0xffff0000 : 0xffffffff);
}

// paddle class
Paddle::Paddle(const FRECT& pos) : m_pos(pos.Middle()), m_iScore(0)
{
	SetCollisionPolygon(new CRectangle(pos));
}

void Paddle::Render(IRenderer* pRenderer)
{
	::D3DXMATRIX T;
	::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);

	pRenderer->DrawSprite(T,"paddle",2);
}

PlayerPaddle::PlayerPaddle(const FRECT& pos) : Paddle(pos) {}

void PlayerPaddle::Update(IKMInput* pInput, QuadTree* pTree, double dt)
{
	RECT R;
	GetWindowRect(R);

	m_pos.y = pInput->MousePos().y;

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

	m_pos.y = Clamp(m_pos.y,(float)R.top,(float)R.bottom);

	//std::vector<Node*> nodes;
	//pTree->Erase(this,&nodes);

	static_cast<CRectangle*>(m_pCollisionPolygon)->GetRect() = m_pos;

	pTree->Update(this);
}

ComputerPaddle::ComputerPaddle(const FRECT& pos) : Paddle(pos), m_fTime(0.0), m_fVelocity(0.0f), m_fFinalPosY(-1.0f)
{

}

void ComputerPaddle::Update(IKMInput* pInput, QuadTree* pTree, double dt)
{
	pTree->Update(this);

	RECT R;
	GetWindowRect(R);

	// Get rect of area to poll for balls
	CRectangle cRect(FRECT(::D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(R.right/2.0f-10.0f,R.bottom)));

	// poll every 1 seconds
	m_fTime += dt;
	if(m_fTime >= .2f)
	{
		bool bFoundBall = false;

		// find near nodes in the area
		std::vector<Node*> nodes;
		pTree->FindNearNodes(&cRect,nodes);


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

					m_fFinalPosY = PongRayTrace(ballPos,dir,50.0f);

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
		m_pos.y += 5.0f*m_fVelocity * dt;
	}
	//m_pos.y += 10.5f * m_fVelocity * dt;

	static_cast<CRectangle*>(m_pCollisionPolygon)->GetRect() = m_pos;

}



// Pong state

RTTI_IMPL(Pong);

Pong::Pong() : m_iLeftScore(0), m_iRightScore(0), m_pRightPaddle(nullptr), m_pLeftPaddle(nullptr), m_bDrawQuadTree(false)
{
}

Pong::~Pong()
{
	delete m_pLeftPaddle;
	delete m_pRightPaddle;

	for_each(m_balls.begin(),m_balls.end(),[](Ball* pBall)
	{
		delete pBall;
	});

	delete m_pQuadTree;
}

void Pong::Init(Game* pGame)
{
	// first register the game with script
	RegisterScript(pGame);

	// Build the rect of the window
	RECT R;
	GetWindowRect(R);
	FRECT fRect(D3DXVECTOR2(R.left,R.top),D3DXVECTOR2(R.right,R.bottom));

	// build the quadtree
	m_pQuadTree = new QuadTree(fRect);

	// Get the size of the texture we are using
	const Texture* pTex = pGame->GetRenderer()->GetTextureManager()->GetTexture("paddle");
	FRECT rect(D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(pTex->uiWidth,pTex->uiHeight));
	rect = D3DXVECTOR2(D3DXVECTOR2(R.right - 50.0f,(R.bottom + R.top) / 2.0f));

	// build paddles and add them to the quadtree
	m_pRightPaddle = new PlayerPaddle(rect);
	m_pQuadTree->Insert(m_pRightPaddle);

	rect = D3DXVECTOR2(D3DXVECTOR2(R.left + 50.0f,(R.bottom + R.top) / 2.0f));
	m_pLeftPaddle = new ComputerPaddle(rect);
	m_pQuadTree->Insert(m_pLeftPaddle);

	// todo: maybe implement this in the future
	//BuildMenu();

	::ShowCursor(FALSE);

	//Ball* pBall = new Ball(::D3DXVECTOR2(R.left / 2,R.right / 2),D3DXVECTOR2(1.0f,0.0f),100.0f,1.0f);
	//m_balls.push_back(pBall);
	//m_pQuadTree->Insert(pBall);
}

void Pong::Destroy(Game* pGame)
{
	::ShowCursor(TRUE);

	asVM* pVM = pGame->GetAs();
	asIScriptEngine* pScriptEngine = pVM->GetScriptEngine();

	DBAS(pScriptEngine->GarbageCollect());
	DBAS(pScriptEngine->RemoveConfigGroup(s_rtti.GetName().c_str()));

	pScriptEngine->Release();
}

void Pong::Update(Game* pGame)
{
	IKMInput* pInput = pGame->GetInput();

	if(pInput->KeyDown(SPACE))
	{
		m_bDrawQuadTree = !m_bDrawQuadTree;
	}

	if(pInput->KeyDown(ENTER) || pInput->MouseClick(0))
	{
		RECT R;
		GetWindowRect(R);

		// add new ball
		POINT P = pInput->MousePos();
		float a = ::GetRandFloat(135.0f,225.0f) * 0.01745329f;
		Ball* pBall = new Ball(D3DXVECTOR2(R.right/2.0f,R.bottom/8.0f),D3DXVECTOR2(-cosf(a),sinf(a)),700.0f,10.0f);
		m_balls.push_back(pBall);
		m_pQuadTree->Insert(pBall);
	}

	//m_pQuadTree->Update();
	//m_pQuadTree->Update();

	//m_gui.Update(pInput,pGame->GetDt());

	UpdatePaddles(pGame);
	UpdateBalls(pGame);

}

void Pong::BuildMenu()
{
	// todo: need to finish implementing

	Menu* pMenu = new Menu();

	RECT R;
	GetWindowRect(R);

	const unsigned int uiWidth = R.right - R.left;
	const unsigned int uiHeight = R.bottom - R.top;

	RECT SquareButtonRect = {R.left + uiWidth / 3,R.top,R.left + (2 * uiWidth / 3),R.top + 30};
	SquareButton<void>* pButton = new SquareButton<void>(SquareButtonRect,"Test Button");

	pMenu->AddElement(pButton);

	m_gui.SetMenu(pMenu);
	
}

void Pong::RegisterScript(Game* pGame)
{
	// register the pong interface in script within a ConfigGroup

	asVM* pVM = pGame->GetAs();
	asIScriptEngine* pScriptEngine = pVM->GetScriptEngine();

	const char* pName = s_rtti.GetName().c_str();

	DBAS(pScriptEngine->BeginConfigGroup(pName));

	DBAS(pScriptEngine->RegisterObjectType(pName,0,asOBJ_REF | asOBJ_NOHANDLE));

	DBAS(pScriptEngine->RegisterObjectMethod(pName,"void ResetScores()",asMETHOD(Pong,ResetScores),asCALL_THISCALL))

	DBAS(pScriptEngine->RegisterGlobalProperty("Pong pong",(void*)this));

	DBAS(pScriptEngine->EndConfigGroup());

	pScriptEngine->Release();

}
void Pong::UpdateBalls(Game* pGame)
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
	RECT R;
	GetWindowRect(R);

	// Loop over all of the balls
	while(iter != m_balls.end())
	{
		Ball* pBall = *iter;

		pBall->Update(this->m_pQuadTree,pGame->GetDt());

		if(pBall->GetPos().x < (float)R.left || pBall->GetPos().x > (float)R.right)
		{
			if(pBall->GetPos().x < (float)R.left)
			{
				m_iRightScore++;
			}
			else if(pBall->GetPos().x > (float)R.right)
			{
				m_iLeftScore++;
			}

			pBall->EraseFromQuadtree(m_pQuadTree);

			delete pBall;
			iter = m_balls.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void Pong::UpdatePaddles(Game* pGame)
{
	IKMInput* pInput = pGame->GetInput();
	double dt = pGame->GetDt();

	if(m_pLeftPaddle)
	{
		m_pLeftPaddle->Update(pInput,this->m_pQuadTree,dt);
	}

	if(m_pRightPaddle)
	{
		m_pRightPaddle->Update(pInput,m_pQuadTree,dt);
	}
}

void Pong::IncreasePaddlePoints(const std::vector<Paddle*>& ref)
{
	for_each(ref.begin(),ref.end(),[](Paddle* pBall)
	{
		pBall->IncreaseScore();
	});
}

void Pong::Draw(Game* pGame)
{
	typedef void (Ball::*BallPtr)(IRenderer*); 

	BallPtr a = &Ball::Render;

	IRenderer* pRenderer = pGame->GetRenderer();

	if(m_pLeftPaddle)
	{
		m_pLeftPaddle->Render(pRenderer);
	}

	if(m_pRightPaddle)
	{
		m_pRightPaddle->Render(pRenderer);
	}

	for_each(m_balls.begin(),m_balls.end(),[&](Ball* pBall)
	{
		pBall->Render(pRenderer);
	});

	DrawScore(pGame);

	//m_gui.Render(pRenderer);

	if(m_bDrawQuadTree)
	{
		m_pQuadTree->Render(pRenderer);
	}

}

void Pong::DrawScore(Game* pGame)
{
	IRenderer* pRenderer = pGame->GetRenderer();
	RECT R;
	GetWindowRect(R);

	const POINT POS[2] = 
	{
		{R.left + 200,R.top + 50},
		{R.right - 200,R.top + 50}
	};

	std::stringstream stream;
	stream << m_iLeftScore << " " << m_iRightScore;

	unsigned int i = 0;
	while(stream)
	{
		std::string str;
		stream >> str;

		pRenderer->DrawString(str.c_str(),POS[i++],0xffffffff);
	}
}

void Pong::ResetScores()
{
	m_iLeftScore = m_iRightScore = 0;
}
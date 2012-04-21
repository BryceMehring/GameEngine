#include "BNew.h"
#include "GameOfLife.h"
#include "IRenderer.h"
#include "Game.h"
#include "FileManager.h"
#include "GameOfLifeFactory.h"
#include "StateUpdater.h"
#include <sstream>
#include <ctime>
#include "BNew.h"

using namespace std;

const RECT SCREEN_R = {50,50,700,550};

RECT g_R;

class Unit : public ISpatialObject, public Pooled
{
public:

	Unit(DWORD color, float speed)
	: m_color(color), m_fSpeed(speed), m_fAcceleration(0.0f), m_fdistance(0.0f) {}

	virtual KEY GetPos() const
	{
		POINT P = {m_pos.x,m_pos.y};
		return P;
	}
	virtual Type GetType() const
	{
		return Type::Unit;
	}
	void SetPos(const D3DXVECTOR2& P)
	{
		m_pos = P;
	}
	void SetDir(const D3DXVECTOR2& dir) { m_dir = dir; }
	virtual void Update(float dt)
	{
		/*if(m_pNode != nullptr)
		{
			const Node::LIST_DTYPE& nearObj = *(m_pNode->GetNearObjects());
			for(auto iter = nearObj.begin(); iter != nearObj.end(); ++iter)
			{
				if(*iter != this)
				{
					D3DXVECTOR2 L = m_dir - D3DXVECTOR2((*iter)->GetPos().x,(*iter)->GetPos().y);
					float d = fabsf(D3DXVec2Length(&L));
					m_fdistance = d;
					if(d < 100.0f)
					{
						//m_dir = -m_dir;
						break;
						//m_dir = 2*normal[index]*D3DXVec2Dot(&m_dir,&normal[index]) - m_dir;
					}
				}
			}
		}*/

		D3DXVECTOR2 normal[4];
		normal[0] = D3DXVECTOR2(1.0f,0.0f);
		normal[1] = D3DXVECTOR2(-1.0f,0.0f);
		normal[2] = D3DXVECTOR2(0.0f,1.0f);
		normal[3] = D3DXVECTOR2(0.0f,-1.0f);

		unsigned int index = -1;
		if(m_pos.x <= 0)
		{
			//m_pos.x = 800.0f;
			index = 0;
		}
		else if(m_pos.x >= g_R.right)
		{
			//m_pos.x = 0.0f;
			index = 1;
		}

		if(m_pos.y <= 0.0f)
		{
			//m_pos.y = 600.0f;
			index = 2;
		}
		else if(m_pos.y >= g_R.bottom)
		{
			//m_pos.y = 0.0f;
			index = 3;
		}

		if(index != -1)
		{
			m_dir = -m_dir;
			// R = 2 * N * ( DotProduct[ I,N] ) - I
			m_dir = 2*normal[index]*D3DXVec2Dot(&m_dir,&normal[index]) - m_dir;
			//m_dir = reflect;
		}


		m_fSpeed += m_fAcceleration;
		m_pos += m_dir * m_fSpeed * dt;
		//m_pos.x = (m_pos.x +(m_dir.x * m_speed)) * dt;
		//m_pos.y = (m_pos.y +(m_dir.y * m_speed)) * dt;
	}

	virtual void Render(IRenderer* pRenderer)
	{
		/*char buffer[64];
		sprintf_s(buffer,"D: %f",m_fdistance);
		POINT P = GetPos();
		//P.y += 20;*/
		//pRenderer->DrawString(buffer,P,0xffffffff);
		pRenderer->DrawPoint(D3DXVECTOR2(m_pos.x,m_pos.y),m_color);
	}

	/*void Move()
	{
		if(m_pNode)
		{
			const Node::LIST_DTYPE* pNearObjs = m_pNode->GetNearObjects();
			for_each(pNearObjs->begin(),pNearObjs->end(),[&](ISpatialObject* pObj)
			{
				if(pObj != this)
				{
					if(pObj->GetType() == ISpatialObject::Unit)
					{
						Unit* pUnit = static_cast<Unit*>(pObj);
						cout<<pNearObjs->size()<<endl;
					}
				}
			});
		}

		int scalar = 10;
		m_pos.x += scalar;
		//m_pos.y -= scalar;
	}*/

private:

	D3DXVECTOR2 m_pos;
	float m_fSpeed;
	float m_fAcceleration;
	float m_fdistance;
	D3DXVECTOR2 m_dir;
	DWORD m_color;

};

class Mouse : public ISpatialObject
{
public:

	virtual KEY GetPos() const
	{ 
		return m_pos;
	}
	virtual Type GetType() const { return AnotherUnit; }

	void SetMousePos(const POINT& P) { m_pos = P; }

	virtual void Update(float dt)
	{
	}

	virtual void Render(IRenderer* pRenderer)
	{
		DWORD color = D3DCOLOR_XRGB(rand() % 256,rand() % 256,rand() % 256);
		pRenderer->DrawPoint(D3DXVECTOR2(m_pos.x,m_pos.y),color);
	}

private:

	POINT m_pos;
};

GameOfLife::GameOfLife(Game* m_pGame) : IGameState(m_pGame), m_fTime(0.0f)
{
	srand(time(0));

	
}
GameOfLife::~GameOfLife()
{
	
}


void GameOfLife::BuildMenu(Game* m_pGame)
{
	{
		//Menu* pMenu = new Menu();
		//m_ui.SetMenu(pMenu);

		/*Text name = {"Back",{300,300}};
		Delegate<void,int> buttonCallback(m_pGame,&Game::SetState);
		RECT R = {300,300,400,400};
		DxPolygon* pPolygon = new DxSquare(R);

		IUIElement* pButton = new GenericButton<int>(name,buttonCallback,MENU,pPolygon);
		pMenu->AddElement(pButton);*/
	}
}

void GameOfLife::Init()
{
	RECT R;
	GetClientRect(m_pGame->GetWindow()->GetWindowHandle(),&R);
	g_R = R;
	InflateRect(&R,40,40);

	m_pQuadTree = new QuadTree(R);
	m_pMouse = new Mouse();

	m_pos = m_pGame->GetInput()->MousePos();

	m_pMouse->SetMousePos(m_pos);

	//m_pQuadTree->Insert(m_pMouse);
	m_units.push_back(m_pMouse);

	BuildMenu(m_pGame);

	ShowCursor(false);
}
void GameOfLife::Destroy()
{
	// clear screen
	IRenderer* pRenderer = m_pGame->GetRenderer();

	for_each(m_units.begin(),m_units.end(),[=](ISpatialObject* pUnit)
	{
		delete pUnit;
	});

	m_units.clear();

	delete m_pQuadTree;
	m_pQuadTree = nullptr;
}
void GameOfLife::Update()
{
	StateUpdater su(m_state,UPDATING);

	float dt = m_pGame->GetDt();
	m_fTime += dt;

	/*if(m_timer.GetTimeInSeconds() > 5.0f)
	{
		m_pGame->SetState(0);
	}*/

	IKMInput* pInput = m_pGame->GetInput();

	m_pos = pInput->MousePos();
	m_pMouse->SetMousePos(m_pos);
	
	if(m_fTime > 100.0f)
	{
		m_fTime = 0.0f;
		//m_pos.y += 1;
		DWORD color = D3DCOLOR_XRGB(rand() % 256,rand() % 256,rand() % 256);
		float angle = (float)(1 + rand() % 360) * 0.01745329f;
		D3DXVECTOR2 dir(cosf(angle),sinf(angle));
		Unit* pUnit = new Unit(color,0.2f);
		pUnit->SetPos(D3DXVECTOR2(m_pos.x,m_pos.y));
		pUnit->SetDir(dir);

		//m_ui.Update(pInput);

		if(m_pQuadTree->Insert(pUnit) == false)
		{
			delete pUnit;
		}
		else
		{
			m_units.push_back(pUnit);
		}
	}

	for_each(m_units.begin(),m_units.end(),[=](ISpatialObject* pUnit)
	{
		pUnit->Update(dt);
	});

	m_pQuadTree->Update();
	

	//m_VEC[3] = D3DXVECTOR2(Pos.x,Pos.y);*/
}
void GameOfLife::Draw()
{
	StateUpdater su(m_state,RENDERING);

	IRenderer* pRenderer = m_pGame->GetRenderer();

	for_each(m_units.begin(),m_units.end(),[=](ISpatialObject* pUnit)
	{
		pUnit->Render(pRenderer);
	});

	//m_ui.Render(pRenderer);

	//m_pQuadTree->Render(pRenderer);

	//pRenderer->DrawLine(m_VEC,4,0xdeadc0de);
	//pRenderer->DrawString(out.str().c_str(),P,0xffffffff);
}
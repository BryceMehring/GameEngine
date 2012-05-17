
#include "GameOfLife.h"
#include "IRenderer.h"
#include "Game.h"
#include "FileManager.h"
#include "QuadTree.h"
#include "BNew.h"
#include "VecMath.h"
#include "RTTI.h"

#include <sstream>
#include <ctime>

#pragma comment(lib,"d3dx9.lib")

using namespace std;

const RECT SCREEN_R = {50,50,700,550};

// note: global variable only temporary, simple to test concept
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
			m_dir = Reflect(-m_dir,normal[index]);
		}

		bool bMouseIsNear = false;

		D3DXVECTOR2 box[5];
		box[0] = D3DXVECTOR2(m_pos.x - 8.0f,m_pos.y + 8.0f);
		box[1] = D3DXVECTOR2(m_pos.x - 8.0f,m_pos.y - 8.0f);
		box[2] = D3DXVECTOR2(m_pos.x + 8.0f,m_pos.y - 8.0f);
		box[3] = D3DXVECTOR2(m_pos.x + 8.0f,m_pos.y + 8.0f);
		box[4] = D3DXVECTOR2(m_pos.x - 8.0f,m_pos.y + 8.0f);


		if(m_pNode != nullptr)
		{
			const Node::LIST_DTYPE& nearObj = *(m_pNode->GetNearObjects());
			for(auto iter = nearObj.begin(); iter != nearObj.end(); ++iter)
			{
				if(*iter != this)
				{
					Unit* pUnit = nullptr;

					if((*iter)->GetType() == AnotherUnit)
					{
						bMouseIsNear = true;
					}
					else
					{
						pUnit = (Unit*)(*iter);
					}

					if(pUnit)
					{
						// If they are parallel
						//if(::D3DXVec2Dot(&m_dir,&pUnit->m_dir) < 0.0f)
						{
							D3DXVECTOR2 pos = D3DXVECTOR2((*iter)->GetPos().x,(*iter)->GetPos().y);
							D3DXVECTOR2 dist = pos - m_pos;

							if(IsPointInPolygon(box,5,(*iter)->GetPos()))
							{
							// todo: use a ray casting algorithm here for collision
							//if(D3DXVec2LengthSq(&(dist)) < 100)
							//{
								/*D3DXVECTOR2 thisPos;

								D3DXVec2Normalize(&pos,&pos);
								D3DXVec2Normalize(&thisPos,&m_pos);

								float fDot = D3DXVec2Dot(&pos,&thisPos);*/

								::D3DXVec2Normalize(&dist,&dist);

								m_dir = Reflect(-m_dir,dist);
								break;

								//m_fSpeed = -m_fSpeed;
								//break;
							}
						}
					}
				}
			}

			::D3DXVec2Normalize(&m_dir,&m_dir);
		}

		/*if(bMouseIsNear)
		{
			m_fAcceleration += 0.5f*dt;
		}
		else
		{
			m_fAcceleration -= 0.4f*dt;

			if(m_fSpeed <= 0.0f)
			{
				m_fAcceleration = 0.0f;
			}
		}*/

		m_fSpeed += m_fAcceleration;
		m_pos += m_dir * m_fSpeed * dt;
	}

	virtual void Render(IRenderer* pRenderer)
	{
		/*char buffer[64];
		sprintf_s(buffer,"D: %f",m_fdistance);
		POINT P = GetPos();
		//P.y += 20;*/
		//pRenderer->DrawString(buffer,P,0xffffffff);
		pRenderer->DrawSprite(D3DXVECTOR2(m_pos.x,m_pos.y),"Point",m_color);
		//pRenderer->DrawPoint(D3DXVECTOR2(m_pos.x,m_pos.y),m_color);
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
		//DWORD color = D3DCOLOR_XRGB(rand() % 256,rand() % 256,rand() % 256);
		pRenderer->DrawSprite(D3DXVECTOR2(m_pos.x,m_pos.y),"Square");
		//pRenderer->DrawPoint(D3DXVECTOR2(m_pos.x,m_pos.y),color);
	}

private:

	POINT m_pos;
};

RTTI_IMPL(GameOfLife);

GameOfLife::GameOfLife() : m_fTime(0.0f), m_pQuadTree(nullptr), m_bDrawQuadTree(false)
{
	srand(time(0));
}
GameOfLife::~GameOfLife()
{
	for_each(m_units.begin(),m_units.end(),[=](ISpatialObject* pUnit)
	{
		delete pUnit;
	});

	delete m_pQuadTree;
	m_pQuadTree = nullptr;
}


void GameOfLife::BuildMenu(Game* m_pGame)
{
}

void GameOfLife::Init(Game* pGame)
{
	RECT R;
	GetClientRect(pGame->GetWindow()->GetWindowHandle(),&R);
	g_R = R;
	InflateRect(&R,40,40);

	m_pQuadTree = new QuadTree(R);
	m_pMouse = new Mouse();

	m_pos = pGame->GetInput()->MousePos();

	m_pMouse->SetMousePos(m_pos);

	//m_pQuadTree->Insert(m_pMouse);
	m_units.push_back(m_pMouse);

	BuildMenu(pGame);

	ShowCursor(false);
}
void GameOfLife::Destroy(Game* pGame)
{
	ShowCursor(true);

	// clear screen
	IRenderer* pRenderer = pGame->GetRenderer();
}
void GameOfLife::Update(Game* pGame)
{
	float dt = pGame->GetDt();
	m_fTime += dt;

	/*if(m_timer.GetTimeInSeconds() > 5.0f)
	{
		m_pGame->SetState(0);
	}*/

	IKMInput* pInput = pGame->GetInput();

	m_pos = pInput->MousePos();
	m_pMouse->SetMousePos(m_pos);

	if(pInput->KeyDown(SPACE))
	{
		m_bDrawQuadTree = !m_bDrawQuadTree;
	}
	
	if(pInput->MouseClick(0))
	//if(m_fTime > 0.1f && m_units.size() < 200)
	{
		m_fTime = 0.0f;
		//m_pos.y += 1;
		DWORD color = D3DCOLOR_XRGB(rand() % 256,rand() % 256,rand() % 256);
		//float angle = (float)(1 + rand() % 360) * 0.01745329f;
		D3DXVECTOR2 dir(cosf(0),sinf(0));
		Unit* pUnit = new Unit(color,200.0f);
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
}
void GameOfLife::Draw(Game* pGame)
{
	IRenderer* pRenderer = pGame->GetRenderer();

	for_each(m_units.begin(),m_units.end(),[=](ISpatialObject* pUnit)
	{
		pUnit->Render(pRenderer);
	});

	if(m_bDrawQuadTree)
	{
		m_pQuadTree->Render(pRenderer);
	}

	//pRenderer->DrawLine(m_VEC,4,0xdeadc0de);
	//pRenderer->DrawString(out.str().c_str(),P,0xffffffff);
}
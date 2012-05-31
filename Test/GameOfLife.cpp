
#include "Game.h"
#include "GameOfLife.h"
#include "IRenderer.h"
#include "FileManager.h"
#include "QuadTree.h"
#include "BNew.h"

#include <sstream>
#include <ctime>

#pragma comment(lib,"d3dx9.lib")

using namespace std;

const RECT SCREEN_R = {50,50,700,550};

// note: global variable only temporary, simple to test concept
RECT g_R;

class ISpatialObjectUpdate : public ISpatialObject
{
public:

	virtual void Update(double dt) = 0;

};

// todo: need to rewrite this class
class Unit : public ISpatialObjectUpdate, public Pooled
{
public:

	Unit(const D3DXVECTOR2& center, float fRad, DWORD color, float speed)
	: m_color(color), m_fSpeed(speed), m_fAcceleration(0.0f), m_fdistance(0.0f), m_fRad(fRad), m_pos(center), m_fTime(0.0f)
	{
		m_pCollisionPolygon = new CCircle(Circle(center,fRad));
	}

	virtual D3DXVECTOR2 GetPos() const
	{
		return m_pos;
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
	virtual void Update(double dt)
	{
		D3DXVECTOR2 normal[4];
		normal[0] = D3DXVECTOR2(1.0f,0.0f);
		normal[1] = D3DXVECTOR2(-1.0f,0.0f);
		normal[2] = D3DXVECTOR2(0.0f,1.0f);
		normal[3] = D3DXVECTOR2(0.0f,-1.0f);

		D3DXVECTOR2 G = D3DXVECTOR2(0.0f,.58f);

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
			//G = D3DXVECTOR2(0.0f,-1.2f);
		}

		if(index != -1)
		{
			m_dir = Reflect(-m_dir,normal[index]);
		}
		else
		{
			//G = D3DXVECTOR2(0.0f,2.8f);
		}

		/*if(m_pNode != nullptr)
		{
			const Node::LIST_DTYPE& nearObj = *(m_pNode->GetNearObjects());
			for(auto iter = nearObj.begin(); iter != nearObj.end(); ++iter)
			{
				if(*iter != this)
				{
					if(m_pCollisionPolygon->Intersects((*iter)->GetCollisionPolygon()))
					{
						D3DXVECTOR2 pos = D3DXVECTOR2((*iter)->GetPos().x,(*iter)->GetPos().y);
						D3DXVECTOR2 dist = pos - m_pos;
						D3DXVECTOR2 dir = m_dir + G;
								
						::D3DXVec2Normalize(&dir,&dir);
						D3DXVec2Normalize(&dist,&dist);


						m_dir = Reflect(-dir,dist);
						break;
					}
				}
			}
		}
		*/
		//D3DXVec2Normalize(&m_dir,&m_dir);

		m_fTime += dt;
		G *= m_fTime;

		m_pos += (m_dir + G) * m_fSpeed * dt;

		static_cast<CCircle*>(m_pCollisionPolygon)->GetCircle().center = m_pos;
	}

	virtual void Render(IRenderer* pRenderer)
	{
		//ClearNodes();

		/*char buffer[64];
		sprintf_s(buffer,"D: %f",m_fdistance);
		POINT P = GetPos();
		//P.y += 20;*/
		//pRenderer->DrawString(buffer,P,0xffffffff);
		::D3DXMATRIX S, T;
		::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);
		::D3DXMatrixScaling(&S,m_fRad / 16.0f,m_fRad/16.0f,1.0f);

		pRenderer->DrawSprite(S*T,"Point",0,m_color);
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
	float m_fRad;
	float m_fTime;
	D3DXVECTOR2 m_dir;
	DWORD m_color;

};

class Mouse : public ISpatialObjectUpdate
{
public:

	virtual D3DXVECTOR2 GetPos() const
	{
		return m_pos;
	}
	virtual Type GetType() const { return AnotherUnit; }

	void SetMousePos(const D3DXVECTOR2& P) { m_pos = P; }

	virtual void Update(double dt)
	{
	}

	virtual void Render(IRenderer* pRenderer)
	{
		::D3DXMATRIX T;
		::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);

		//DWORD color = D3DCOLOR_XRGB(rand() % 256,rand() % 256,rand() % 256);
		pRenderer->DrawSprite(T,"Square",1);
		//pRenderer->DrawPoint(D3DXVECTOR2(m_pos.x,m_pos.y),color);
	}

private:

	D3DXVECTOR2 m_pos;
};

RTTI_IMPL(GameOfLife);

GameOfLife::GameOfLife() : m_fTime(0.0f), m_pQuadTree(nullptr), m_bDrawQuadTree(false)
{
	
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
	//InflateRect(&R,40,40);

	// build FRECT from regular rect
	FRECT fRect(D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(800.0f,600.0f));

	m_pQuadTree = new QuadTree(fRect);
	m_pMouse = new Mouse();

	m_pos = pGame->GetInput()->MousePos();

	m_pMouse->SetMousePos(D3DXVECTOR2(m_pos.x,m_pos.y));

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
	m_pMouse->SetMousePos(::D3DXVECTOR2(m_pos.x,m_pos.y));

	if(pInput->KeyDown(SPACE))
	{
		m_bDrawQuadTree = !m_bDrawQuadTree;
	}
	
	if(pInput->MouseClick(0))
	{
		if(m_fTime > 0.05f && m_units.size() < 200)
		{
			m_fTime = 0.0f;
			//m_pos.y += 1;
			DWORD color = D3DCOLOR_XRGB(rand() % 256,rand() % 256,rand() % 256);
			float angle = (float)(rand() % 180) * 0.01745329f;
			float v = GetRandFloat(50.0f,250.0f);
			float s = ::GetRandFloat(15.0f,20.0f);
			D3DXVECTOR2 dir(cosf(angle),-sinf(angle));
			Unit* pUnit = new Unit(D3DXVECTOR2(m_pos.x,m_pos.y),s,color,v);
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
	}

	// Update all units
	for(unsigned int i = 0; i < m_units.size();)
	{
		m_units[i]->Update(dt);

		// if it the unit is a ball, and is left the screen
		if((m_units[i]->GetType() == ISpatialObject::Unit) && (!m_units[i]->HasNode()))
		{
			// remove it
			delete m_units[i];
			m_units.erase(m_units.begin() + i);
		}
		else
		{
			// loop to next node
			++i;
		}
	}

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
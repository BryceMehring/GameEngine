
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

class Cell : public ISpatialObject
{
public:

	Cell(const D3DXVECTOR2& pos, float R, bool a) :
	  pos(pos), m_CCircle(Math::Circle(pos,R)), m_bAlive(a), m_time(0.0) {}

	virtual D3DXVECTOR2 GetPos() const { return pos; }
	virtual Type GetType() const { return ISpatialObject::Unit; }
	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const
	{
		return m_CCircle;
	}

	bool IsAlive() const { return m_bAlive; }

	virtual void Update(double dt)
	{
		unsigned int uiLive = 0;

		m_time += dt;

		//.5
		if(m_time > 1.0)
		{
			m_time = 0.0;

			ProccessNearNodes(m_nodes,[&](const ISpatialObject* pObj) -> bool
			{
				if(pObj != this)
				{
					Cell* pCell = (Cell*)pObj;

					if(pCell->IsAlive())
					{
						++uiLive;
					}
					//else
					{
						///++uiDead;
					}
				}

				return false;
			});

		
			// note: if this object is dead, 
			// note: the counter uiLive is not always 0
			// note: this is because the cells around it might be alive
			if(m_bAlive && (uiLive < 2 || uiLive > 3))
			{
				m_bAlive = false;
			}
			else if(!m_bAlive && uiLive > 1)
			{
				m_bAlive = true;
			}
		}
	}

	virtual void Render(::IRenderer& renderer)
	{
		if(m_bAlive)
		{
			D3DXMATRIX T, R;
			D3DXMatrixTranslation(&T,pos.x,pos.y,0.0f);

			float rad = m_CCircle.GetCircle().r;

			::D3DXMatrixScaling(&R,rad / 32.0f,rad/ 32.0f,1.0f);

			renderer.DrawSprite(R*T,"cell",2);
		}
	}

protected:

	bool m_bAlive;
	double m_time;

	D3DXVECTOR2 pos;
	Math::CCircle m_CCircle;

};



RTTI_IMPL(GOL);

GOL::GOL() : m_fTime(0.0f), m_pQuadTree(nullptr), m_bDrawQuadTree(false)
{
	
}
GOL::~GOL()
{
	for(unsigned int i = 0; i < m_units.size(); ++i)
	{
		delete m_units[i];
	}

	delete m_pQuadTree;
	m_pQuadTree = nullptr;
}


void GOL::BuildMenu(Game* m_pGame)
{
}

void GOL::Init(Game& game)
{
	RegisterScript(game);

	RECT R;
	GetClientRect(game.GetWindow().GetWindowHandle(),&R);
	//InflateRect(&R,40,40);

	// build FRECT from regular rect
	Math::FRECT fRect(D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(R.right,R.bottom));

	m_pQuadTree = new QuadTree(fRect);

	//Reset();

	//TextureInfo info;
	//game.GetRenderer().GetTextureManager().GetTextureInfo("cell",info);
	
	
}
void GOL::Destroy(Game& game)
{
	asIScriptEngine* pScriptEngine = game.GetAs().GetScriptEngine();

	DBAS(pScriptEngine->GarbageCollect());
	DBAS(pScriptEngine->RemoveConfigGroup(s_rtti.GetName().c_str()));

	pScriptEngine->Release();
}
void GOL::Update(Game& game)
{
	static double time = 5.0;

	time += game.GetDt();

	if(time > 0.04)
	{
		time = 0.0;

		if(game.GetInput().MouseClick(0,false))
		{
			POINT P = game.GetInput().MousePos();
			Cell* pCell = new Cell(D3DXVECTOR2(P.x,P.y),8.0f,true);
			this->m_pQuadTree->Insert(*pCell);
			this->m_units.push_back(pCell);
		}
	}

	// Update all units
	for(unsigned int i = 0; i < m_units.size(); ++i)
	{
		m_units[i]->Update(game.GetDt());
	}
}
void GOL::Draw(Game& game)
{
	for(unsigned int i = 0; i < m_units.size(); ++i)
	{
		m_units[i]->Render(game.GetRenderer());
	}

	//this->m_pQuadTree->Render(game.GetRenderer());
}

void GOL::RegisterScript(Game& game)
{
	// register the pong interface in script within a ConfigGroup

	asIScriptEngine* pScriptEngine = game.GetAs().GetScriptEngine();

	const char* pName = s_rtti.GetName().c_str();

	DBAS(pScriptEngine->BeginConfigGroup(pName));

	DBAS(pScriptEngine->RegisterObjectType(pName,0,asOBJ_REF | asOBJ_NOHANDLE));

	DBAS(pScriptEngine->RegisterObjectMethod(pName,"void Reset()",asMETHOD(GOL,Reset),asCALL_THISCALL));

	DBAS(pScriptEngine->RegisterGlobalProperty("GOL gol",(void*)this));

	DBAS(pScriptEngine->EndConfigGroup());

	pScriptEngine->Release();

}

void GOL::Reset()
{
	RECT R;
	GetClientRect(::GetActiveWindow(),&R);

	for(unsigned int i = 0; i < m_units.size(); ++i)
	{
		m_pQuadTree->Erase(*m_units[i]);
		delete m_units[i];
	}

	m_units.clear();

	// 15
	//50
	for(unsigned int i = 0; i < 200; ++i)
	{
		bool alive = i < 40;
		//bool alive = ((rand() % 1) == 0);

		//float x =  alive ? ((R.left + R.right + 20) / 2) : GetRandFloat(R.left,R.right);
		float x =  Math::GetRandFloat(R.left,R.right);
		float y = Math::GetRandFloat(R.top,R.bottom);
		Cell* pCell = new Cell(D3DXVECTOR2(x,y),16.0f,alive);
		m_pQuadTree->Insert(*pCell);
		m_units.push_back(pCell);
	}
}
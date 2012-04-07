#include "GameOfLife.h"
#include "IRenderer.h"
#include "Game.h"
#include "FileManager.h"
#include "GameOfLifeFactory.h"
#include "StateUpdater.h"
#include <sstream>

using namespace std;

const RECT SCREEN_R = {50,50,700,550};

class Unit : public ISpatialObject
{
public:

	virtual const KEY& GetPos() const
	{
		return m_pos;
	}
	virtual Type GetType() const
	{
		return Type::Unit;
	}
	void SetPos(const KEY& P)
	{
		m_pos = P;
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

	KEY m_pos;

};

GameOfLife::GameOfLife() : m_quadTree(SCREEN_R)
{
}

void GameOfLife::BuildMenu(Game* pGame)
{
	{
		//Menu* pMenu = new Menu();
		//m_ui.SetMenu(pMenu);

		/*Text name = {"Back",{300,300}};
		Delegate<void,int> buttonCallback(pGame,&Game::SetState);
		RECT R = {300,300,400,400};
		DxPolygon* pPolygon = new DxSquare(R);

		IUIElement* pButton = new GenericButton<int>(name,buttonCallback,MENU,pPolygon);
		pMenu->AddElement(pButton);*/
	}
}

void GameOfLife::Init(Game* pGame)
{
	BuildMenu(pGame);

	//pRenderer->ToggleFullscreen();
	//pGame->GetRenderer()->SetDisplayMode(0);
	//D3DXVECTOR2 m_VEC[4];

	/*const RECT& R = pGame->GetWindow()->GetRECT();
	m_VEC[0] = D3DXVECTOR2((R.right)/2,(R.bottom)/2);
	m_VEC[1] = D3DXVECTOR2((R.right)/2+40,(R.bottom)/2+40);
	m_VEC[2] = D3DXVECTOR2((R.right)/2-80,(R.bottom)/2);

	IKMInput* pInput = pGame->GetInput();
	POINT Pos = pInput->MousePos();
	m_VEC[3] = D3DXVECTOR2(Pos.x,Pos.y);*/
}
void GameOfLife::Destroy(Game* pGame)
{
	// clear screen
	IRenderer* pRenderer = pGame->GetRenderer();

	//pRenderer->ToggleFullscreen();
	//pRenderer->ClearScreen();
	//pRenderer->Present();

	for(unsigned int i = 0; i < m_units.size(); ++i)
	{
		delete m_units[i];
	}
}
void GameOfLife::Update(Game* pGame)
{
	StateUpdater su(m_state,UPDATING);

	IKMInput* pInput = pGame->GetInput();
	
	if(pInput->MouseClick(0))
	{
		POINT Pos = pInput->MousePos();
		Unit* pUnit = new Unit();
		pUnit->SetPos(Pos);

		//m_ui.Update(pInput);

		if(m_quadTree.Insert(pUnit) == false)
		{
			delete pUnit;
		}
		else
		{
			m_units.push_back(pUnit);
		}
	}
	//m_VEC[3] = D3DXVECTOR2(Pos.x,Pos.y);

	/*if(m_timer.GetTimeInSeconds() > 1.0)
	{
		pGame->SetState(0);
	}*/
}
void GameOfLife::Draw(Game* pGame)
{
	StateUpdater su(m_state,RENDERING);

	ostringstream out;
	POINT P = {0,0};
	out << "Time: "<< m_timer.GetTimeInSeconds() << endl;

	IRenderer* pRenderer = pGame->GetRenderer();

	pRenderer->Begin();

	//m_ui.Render(pRenderer);

	m_quadTree.Render(pRenderer);

	pRenderer->DrawLine(m_VEC,4,0xdeadc0de);
	pRenderer->DrawString(out.str().c_str(),P,0xffffffff);

	pRenderer->End();
	pRenderer->Present();
}
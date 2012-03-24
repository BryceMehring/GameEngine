#include "GameOfLifeMenu.h"
#include "Game.h"

GameOfLifeMenu::GameOfLifeMenu()
{
}

void GameOfLifeMenu::Init(Game* pGame)
{
	RECT R = {10,10,500,500};

	RECT R2 = {200,200,0,0};
	pGame->GetRenderer()->GetStringRec("Options",R2);

	RECT R3 = {300,100,0,0};
	pGame->GetRenderer()->GetStringRec("State",R3);

	POINT mainPoint = {15,15};

	Text text = { "Options",{R2.left,R2.top}};
	Text text2 = { "State",{R3.left,R3.top}};

	// Create Squares for ui
	DxSquare* pSquare = new DxSquare();
	pSquare->ConstructFromRect(R);

	DxSquare* pButtonPolygon = new DxSquare();
	pButtonPolygon->ConstructFromRect(R2);

	DxSquare* pStateButtonSquare = new DxSquare();
	pStateButtonSquare->ConstructFromRect(R3);

	::DxTriangle* pTriangle = new DxTriangle();
	::D3DXVECTOR2 VEC[4];
	VEC[0] = D3DXVECTOR2(270,50);
	VEC[1] = D3DXVECTOR2(290,100);
	VEC[2] = D3DXVECTOR2(240,100);
	VEC[3] = D3DXVECTOR2(270,50);
	pTriangle->ConstructFromArray(VEC,4);

	// Delegates
	GenericButton<Menu*>::DELEGATE d;
	d.Bind(&m_ui,&UI::SetMenu);

	GenericButton<int>::DELEGATE dState;
	dState.Bind(pGame,&Game::SetState);

	// Create Menu Objects
	Menu* pMenu = new Menu;
	Menu* pOptions = new Menu;

	pMenu->SetMenuTitle("Main Menu",mainPoint);
	pOptions->SetMenuTitle("Options",mainPoint);

	// Create Buttons
	GenericButton<Menu*>* pButton = new GenericButton<Menu*>(text,d,pOptions,pButtonPolygon);
	GenericButton<Menu*>* pButton2 = new GenericButton<Menu*>(text,d,pMenu,pTriangle);
	GenericButton<int>* pStateButton = new GenericButton<int>(text2,dState,1,pStateButtonSquare);

	// Associate square with menu
	pMenu->SetPolygon(pSquare);

	// Build ui tree
	pMenu->AddMenu(pOptions);

	// Add Elements to the menu
	pOptions->AddElement(pButton2);
	pMenu->AddElement(pButton);
	pMenu->AddElement(pStateButton);

	m_ui.SetMenu(pMenu);
}
void GameOfLifeMenu::Destroy(Game* pGame)
{
	
}
void GameOfLifeMenu::Update(Game* pGame)
{
	m_ui.Update(pGame->GetInput());
	//m_pUI->Update();

	/*if(m_timer.GetTimeInSeconds() > 1.0)
	{
		pGame->SetState(1);
	}*/
}
void GameOfLifeMenu::Draw(Game* pGame)
{
	IRenderer* pRenderer = pGame->GetRenderer();
	pRenderer->Begin();

	m_ui.Render(pGame->GetRenderer());

	pRenderer->End();
	pRenderer->Present();
}

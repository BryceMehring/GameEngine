#include "GameOfLifeMenu.h"
#include "Game.h"
#include "WindowManager.h"
#include "FileManager.h"
#include "StateUpdater.h"

GameOfLifeMenu::GameOfLifeMenu() : IGameState()
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
	Text text3 = { "Back",{250,85}};

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
	GenericButton<Menu*>::DELEGATE d(&m_ui,&UI::SetMenu);
	GenericButton<int>::DELEGATE dState(pGame,&Game::SetState);

	// Create Menu Objects
	Menu* pMenu = new Menu;
	Menu* pOptions = new Menu;

	pMenu->SetMenuTitle("Main Menu",mainPoint);
	pOptions->SetMenuTitle("Options",mainPoint);

	// Create Buttons
	GenericButton<Menu*>* pButton = new SquareButton<Menu*>(R2,"Options");
	pButton->SetCallback(d,pOptions);

	GenericButton<Menu*>* pButton2 = new GenericButton<Menu*>(text3,d,pMenu,pTriangle);
	GenericButton<int>* pStateButton = new GenericButton<int>(text2,dState,1,pStateButtonSquare);

	// Associate square with menu
	pMenu->SetPolygon(pSquare);

	// Build ui tree
	pMenu->AddMenu(pOptions);

	// Add Elements to the menu
	pOptions->AddElement(pButton2);
	pMenu->AddElement(pButton);
	pMenu->AddElement(pStateButton);

	BuildResolutionMenu(pGame,pOptions);

	m_ui.SetMenu(pMenu);
}

void GameOfLifeMenu::BuildResolutionMenu(Game* pGame, Menu* pMenu)
{
	IRenderer* pRenderer = pGame->GetRenderer();
	
	RECT R = {10,10,500,500};
	RECT R2 = {200,300,0,0};

	Menu* pResMenu = new Menu();
	POINT menuTitlePoint = {15,15};
	pResMenu->SetMenuTitle("Resolution Menu",menuTitlePoint);
	pResMenu->SetPolygon(new DxSquare(R));

	GenericButton<UINT>::DELEGATE setDisplayModeCallback(pRenderer,&IRenderer::SetDisplayMode);
	GenericButton<Menu*>::DELEGATE setMenuCallback(&m_ui,&UI::SetMenu);

	pRenderer->GetStringRec("Resolution Options",R2);
	GenericButton<Menu*>* pResMenuButton = new SquareButton<Menu*>(R2,"Resolution Options");
	pResMenuButton->SetCallback(setMenuCallback,pResMenu);

	pMenu->AddElement(pResMenuButton);
	pMenu->AddMenu(pResMenu);

	UINT uiDisplayModes = pRenderer->GetNumDisplayAdaptors();
	for(UINT i = 0; i < uiDisplayModes; ++i)
	{
		RECT R3 = {100,100 + 50*i,0,0};
		pRenderer->GetStringRec(pRenderer->GetDisplayModeStr(i).c_str(),R3);

		GenericButton<UINT>* pStateButton = new SquareButton<UINT>(R3,pRenderer->GetDisplayModeStr(i));
		pStateButton->SetCallback(setDisplayModeCallback,i);

		pResMenu->AddElement(pStateButton);
	}
}

void GameOfLifeMenu::Destroy(Game* pGame)
{
}
void GameOfLifeMenu::Update(Game* pGame)
{
	StateUpdater su(m_state,UPDATING);

	m_ui.Update(pGame->GetInput());
	//m_pUI->Update();

	/*if(m_timer.GetTimeInSeconds() > 1.0)
	{
		pGame->SetState(1);
	}*/
}
void GameOfLifeMenu::Draw(Game* pGame)
{
	StateUpdater su(m_state,RENDERING);

	IRenderer* pRenderer = pGame->GetRenderer();
	pRenderer->Begin();

	char buffer[64];
	POINT P = {500,20};
	sprintf_s(buffer,"Time between frames: %f",pGame->GetDt());

	pRenderer->DrawString(buffer,P,0xffffffff);

	m_ui.Render(pGame->GetRenderer());

	pRenderer->End();
	pRenderer->Present();
}

#include "GameOfLifeMenu.h"
#include "Game.h"
#include "WindowManager.h"
#include "FileManager.h"
#include "BNew.h"

using namespace std;

RTTI_IMPL(GameOfLifeMenu);

void Quit()
{
	PostQuitMessage(0);
}

GameOfLifeMenu::GameOfLifeMenu()
{
	
}

void GameOfLifeMenu::Init(Game& game)
{
	/*if(m_gui.GetMenu() != nullptr)
		return;*/

	IRenderer& renderer = game.GetRenderer();

	RECT R = {10,10,500,500};

	RECT R2 = {200,100,0,0};
	renderer.Get2DRenderer().GetStringRec("Options",R2);

	RECT R3 = {300,100,0,0};
	renderer.Get2DRenderer().GetStringRec("State",R3);

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

	/*VEC[0] = D3DXVECTOR2(270,50);
	VEC[1] = D3DXVECTOR2(290,100);
	VEC[2] = D3DXVECTOR2(240,100);
	VEC[3] = D3DXVECTOR2(270,50);*/

	VEC[0] = D3DXVECTOR2(-0.8f,0.8f);
	VEC[1] = D3DXVECTOR2(0.0f,1.0f);
	VEC[2] = D3DXVECTOR2(0.8f,0.8f);
	VEC[3] = D3DXVECTOR2(-0.8f,0.8f);
	pTriangle->ConstructFromArray(VEC,4);

	// Delegates
	GenericButton<Menu*>::DELEGATE dPush(&m_gui,&GUI::SetMenu);
	GenericButton<const std::string&>::DELEGATE dState(&game,&Game::SetNextState);

	// Create Menu Objects
	//m_pGame->SetState(MENU);
	Menu* pMenu = new Menu;
	Menu* pOptions = new Menu;

	pMenu->SetMenuTitle("Main Menu",mainPoint);
	pOptions->SetMenuTitle("Options",mainPoint);

	// Create Buttons
	GenericButton<Menu*>* pButton = new SquareButton<Menu*>(R2,"Options");
	pButton->SetCallback(dPush);
	pButton->SetArg(pOptions);

	GenericButton<Menu*>* pButton2 = new GenericButton<Menu*>(text3,dPush,pMenu,pTriangle);

	
	//todo: loop over all states, and create buttons for them
	GenericButton<const std::string&>* pStateButton = new GenericButton<const std::string&>(text2,dState,"Pong",pStateButtonSquare);

	// Associate square with menu
	pMenu->SetPolygon(pSquare);

	// Build ui tree
	pMenu->AddMenu(pOptions);

	// Add Elements to the menu
	pOptions->AddElement(pButton2);
	pMenu->AddElement(pButton);
	pMenu->AddElement(pStateButton);

	BuildResolutionMenu(game,pOptions);
	BuildQuitButton(game,pMenu);
	BuildTextBoxButton(game,pOptions);
	BuildPluginViewMenu(game,pOptions);

	m_gui.SetMenu(pMenu);
}

/*auto& keys = GameStateFactory::Instance().GetKeys();
	for(unsigned int i = 0; i < keys.size(); ++i)
	{

	}
	*/
void GameOfLifeMenu::BuildResolutionMenu(Game& game, Menu* pMenu)
{
	IRenderer& renderer = game.GetRenderer();
	
	RECT R = {10,10,500,500};
	RECT R2 = {200,300};

	Menu* pResMenu = new Menu();
	POINT menuTitlePoint = {15,15};
	pResMenu->SetMenuTitle("Resolution Menu",menuTitlePoint);
	pResMenu->SetPolygon(new DxSquare(R));

	GenericButton<UINT>::DELEGATE setDisplayModeCallback(&renderer,&IRenderer::SetDisplayMode);
	GenericButton<Menu*>::DELEGATE setMenuCallback = m_gui.CreateCallback();

	renderer.Get2DRenderer().GetStringRec("Resolution Options",R2);
	GenericButton<Menu*>* pResMenuButton = new SquareButton<Menu*>(R2,"Resolution Options");
	pResMenuButton->SetCallback(setMenuCallback);
	pResMenuButton->SetArg(pResMenu);

	pMenu->AddElement(pResMenuButton);
	pMenu->AddMenu(pResMenu);

	// build toggle fullscreen button
	RECT tfR = {400,400,0,0};
	renderer.Get2DRenderer().GetStringRec("Toggle Fullscreen",tfR);
	GenericButton<void>* pTFButton = new SquareButton<void>(R2,"Toggle Fullscreen");
	pTFButton->SetCallback(GenericButton<void>::DELEGATE(&renderer,&IRenderer::ToggleFullscreen));
	
	pResMenu->AddElement(pTFButton);

	UINT uiDisplayModes = renderer.GetNumDisplayAdaptors();
	UINT j = 0;
	UINT k = 0;
	for(UINT i = 0; i < uiDisplayModes; ++i)
	{
		// the problem, here is that % resets to 0, not what I want it to do
		RECT R3 = {50 + 150 * j,(40 + 50 * k),0,0};

		renderer.Get2DRenderer().GetStringRec(renderer.GetDisplayModeStr(i).c_str(),R3);

		GenericButton<UINT>* pStateButton = new SquareButton<UINT>(R3,renderer.GetDisplayModeStr(i));
		pStateButton->SetCallback(setDisplayModeCallback);
		pStateButton->SetArg(i);

		pResMenu->AddElement(pStateButton);

		if((R3.bottom + 3*(R3.bottom - R3.top)) >= R.bottom)
		{
			 //i = 0;
			 ++j;
			 k = 0;
		}
		else
		{
			++k;
		}

	}
}

void GameOfLifeMenu::BuildQuitButton(Game& game, Menu* pMenu)
{
	const char* pStr = "Quit";

	GenericButton<void>::DELEGATE callback(Quit);

	::RECT R = { 400,100,0,0};
	game.GetRenderer().Get2DRenderer().GetStringRec(pStr,R);

	SquareButton<void>* pButton = new SquareButton<void>(R,pStr);
	pButton->SetCallback(callback);
	pButton->SetTextColor(0xffff0000);

	pMenu->AddElement(pButton);
}

void GameOfLifeMenu::BuildTextBoxButton(Game& game, Menu* pMenu)
{
	Menu* pTextMenu = new Menu;
	pMenu->AddMenu(pTextMenu);

	RECT R = {400,400,450,470};
	SquareButton<Menu*>* pButton = new SquareButton<Menu*>(R,"Text Box");
	pButton->SetCallback(m_gui.CreateCallback());
	pButton->SetArg(pTextMenu);

	pMenu->AddElement(pButton);

	SetRect(&R,10,10,500,500);
	TextBox* pTextBox = new TextBox("Hello World",R);
	pTextMenu->AddElement(pTextBox);
}

void GameOfLifeMenu::BuildPluginViewMenu(Game& game, Menu* pMenu)
{
	// Create the Menu
	Menu* pPluginMenu = new Menu;
	pMenu->AddMenu(pPluginMenu);

	// Create the button
	const char* pName = "View Plugins";
	RECT R = {100,100};
	game.GetRenderer().Get2DRenderer().GetStringRec(pName,R);

	SquareButton<Menu*>* pButton = new SquareButton<Menu*>(R,pName);
	pButton->SetCallback(m_gui.CreateCallback());
	pButton->SetArg(pPluginMenu);

	pMenu->AddElement(pButton);

	// Define the Menu
	const PluginManager* pPluginManager = game.GetPluginManager();
	const std::vector<DLLType>& PluginKeys = pPluginManager->GetPluginKeys();

	for(unsigned int i = 0; i < PluginKeys.size(); ++i)
	{
		RECT R = {100,100 + 50*i,0,0};
		const IPlugin* pPlugin = pPluginManager->GetPlugin(PluginKeys[i]);

		char text[64];
		sprintf_s(text,"%i. %s",i+1,pPlugin->GetName());

		game.GetRenderer().Get2DRenderer().GetStringRec(text,R);

		SquareButton<void>* pButton = new SquareButton<void>(R,text);
		pButton->SetCallback(GenericButton<void>::DELEGATE(pPlugin,&IPlugin::About));

		pPluginMenu->AddElement(pButton);
	}
}

void GameOfLifeMenu::Destroy(Game& game)
{
}
void GameOfLifeMenu::Update(Game& game)
{
	m_gui.Update(game.GetInput(),game.GetDt());
}
void GameOfLifeMenu::Draw(Game& game)
{
	/*::D3DXVECTOR3 VEC[4];
	VEC[0] = D3DXVECTOR3(0.0f,0.0f,1.0f);
	VEC[1] = D3DXVECTOR3(1.0f,-1.0f,1.0f);

	game.GetRenderer().DrawLine(VEC,2,0xffffffff);*/

	m_gui.Render(game.GetRenderer());
}

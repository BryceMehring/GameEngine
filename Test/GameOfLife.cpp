#include "GameOfLife.h"
#include "IRenderer.h"
#include "Game.h"
#include <sstream>

using namespace std;

GameOfLife::GameOfLife()
{
}
GameOfLife::~GameOfLife()
{
}

void GameOfLife::Init(Game* pGame)
{
	IRenderer* pRenderer = pGame->GetRenderer();
	//pRenderer->ToggleFullscreen();
	//pGame->GetRenderer()->SetDisplayMode(0);
	//D3DXVECTOR2 m_VEC[4];

	const RECT& R = pGame->GetWindow()->GetRECT();
	m_VEC[0] = D3DXVECTOR2((R.right)/2,(R.bottom)/2);
	m_VEC[1] = D3DXVECTOR2((R.right)/2+40,(R.bottom)/2+40);
	m_VEC[2] = D3DXVECTOR2((R.right)/2-80,(R.bottom)/2);

	IKMInput* pInput = pGame->GetInput();
	POINT Pos = pInput->MousePos();
	m_VEC[3] = D3DXVECTOR2(Pos.x,Pos.y);
}
void GameOfLife::Destroy(Game* pGame)
{
	// clear screen
	IRenderer* pRenderer = pGame->GetRenderer();
	//pRenderer->ToggleFullscreen();
	pRenderer->ClearScreen();
	pRenderer->Present();
}
void GameOfLife::Update(Game* pGame)
{
	IKMInput* pInput = pGame->GetInput();
	POINT Pos = pInput->MousePos();
	m_VEC[3] = D3DXVECTOR2(Pos.x,Pos.y);

	if(m_timer.GetTimeInSeconds() > 1.0)
	{
		pGame->SetState(0);
	}
}
void GameOfLife::Draw(Game* pGame)
{
	ostringstream out;
	POINT P = {0,0};
	out << "Time: "<< m_timer.GetTimeInSeconds() << endl;

	IRenderer* pRenderer = pGame->GetRenderer();

	pRenderer->Begin();

	pRenderer->DrawLine(m_VEC,4,0xdeadc0de);
	pRenderer->DrawString(out.str().c_str(),P,0xffffffff);

	pRenderer->End();
	pRenderer->Present();
}
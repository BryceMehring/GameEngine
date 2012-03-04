#include "GameOfLife.h"
#include "IRenderer.h"
#include "Game.h"

GameOfLife::GameOfLife()
{
}
GameOfLife::~GameOfLife()
{
}

void GameOfLife::Init(Game* pGame)
{
	//pGame->GetRenderer()->SetDisplayMode(0);

	const RECT& R = pGame->GetWindow()->GetRECT();
	m_VEC[0] = D3DXVECTOR2((R.right)/2,(R.bottom)/2);
	m_VEC[1] = D3DXVECTOR2((R.right)/2+40,(R.bottom)/2+40);
	m_VEC[2] = D3DXVECTOR2((R.right)/2-80,(R.bottom)/2);
}
void GameOfLife::Destroy(Game* pGame)
{
	// clear screen
}
void GameOfLife::Update(Game* pGame)
{
	IKMInput* pInput = pGame->GetInput();
	POINT Pos = pInput->MousePos();
	m_VEC[3] = D3DXVECTOR2(Pos.x,Pos.y);

	
	if(pInput->MouseClick(0))
	{
		pGame->SetState(0);
	}
}
void GameOfLife::Draw(Game* pGame)
{
	IRenderer* pRenderer = pGame->GetRenderer();

	pRenderer->Begin();

	pRenderer->DrawLine(m_VEC,4,0xffff0fff);

	pRenderer->End();
	pRenderer->Present();
}
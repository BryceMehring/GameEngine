#include "GameOfLifeMenu.h"
#include "Game.h"


void GameOfLifeMenu::Init(Game* pGame)
{
	//pGame->
}
void GameOfLifeMenu::Destroy(Game* pGame)
{

}
void GameOfLifeMenu::Update(Game* pGame)
{
	IKMInput* pInput = pGame->GetInput();

	if(pInput->MouseClick(1))
	{
		pGame->SetState(1);
	}
}
void GameOfLifeMenu::Draw(Game* pGame)
{
	//pGame->
}

#define PLUGIN_EXPORTS
#define GLFW_DLL
#include "MineSweeper.h"
#include "Timer.h"
#include "Game.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>
#include <sstream>


RTTI_IMPL(MineSweeper);

extern "C" PLUGINDECL IPlugin*  CreatePlugin()
{
	return new MineSweeper();
}


Grid::Grid(float width, float height, unsigned int tileWidth, unsigned int tileHeight) :
	m_uiMineCount(0), m_uiMarkedCount(0), m_uiMarkedCorrectlyCount(0), IGrid(width,height,tileWidth,tileHeight)
{
	BuildGrid();
}

void Grid::BuildGrid()
{
	for(unsigned int i = 0; i < m_tiles.size(); ++i)
	{
		m_tiles[i].mine = rand() % 7 == 0;
		if(m_tiles[i].mine)
		{
			m_uiMineCount++;
		}
	}

	for(unsigned int i = 0; i < m_tiles.size(); ++i)
	{
		if(!m_tiles[i].mine)
		{
			unsigned int uiMineCount = 0;
			unsigned int x = i % m_uiNumTilesWidth;
			unsigned int y = i / m_uiNumTilesWidth;
			
			for(int j = -1; j <= 1; ++j)
			{
				for(int k = -1; k <= 1; ++k)
				{
					if(k == 0 && j == 0)
						continue;

					unsigned int newX = x + j;
					unsigned int newY = y + k;
				
					if(newX < m_uiNumTilesWidth && newY < m_uiNumTilesiHeight)
					{
						unsigned int newIndex = m_uiNumTilesWidth * newY + newX;
						if(m_tiles[newIndex].mine)
						{
							++uiMineCount;
						}
					}
				}
			}

			m_tiles[i].minesNearby = uiMineCount;
		}
	}
}

void Grid::Reset()
{
	m_tiles.clear();
	m_tiles.resize(m_tiles.capacity());

	m_uiMineCount = 0;
	m_uiMarkedCount = 0;
	m_uiMarkedCorrectlyCount = 0;

	BuildGrid();

}

void Grid::Expand(const glm::ivec2& pos)
{
	Tile& tile = m_tiles[m_uiNumTilesWidth*pos.y + pos.x];

	for(int y = -1; y <= 1; ++y)
	{
		for(int x = -1; x <= 1; ++x)
		{
			glm::ivec2 newPos(pos.x + x,pos.y + y);

			if(newPos.x < m_uiNumTilesWidth && newPos.x >= 0 && newPos.y < m_uiNumTilesiHeight && newPos.y >= 0)
			{
				unsigned int newIndex = m_uiNumTilesWidth*newPos.y + newPos.x;

				if(!m_tiles[newIndex].marked)
				{
					if(m_tiles[newIndex].minesNearby != 0 && m_tiles[newIndex].mine == false && m_tiles[newIndex].selsected == false)
					{
						m_tiles[newIndex].selsected = true;
					}

					if(m_tiles[newIndex].minesNearby == 0 && m_tiles[newIndex].selsected == false && m_tiles[newIndex].mine == false)
					{
						m_tiles[newIndex].selsected = true;
						Expand(newPos);
					}
				}
			}
		}
	}
}

int Grid::Update(IKMInput& input)
{
	bool bMouse1 = input.MouseClick(GLFW_MOUSE_BUTTON_1);
	bool bMouse2 = !bMouse1 && input.MouseClick(GLFW_MOUSE_BUTTON_2);
	bool bMine = false;
	if(bMouse1 || bMouse2)
	{
		glm::ivec2 arrayPos;
		Tile* pTile = nullptr;
		bool success = WorldSpaceToTile(input.GetTransformedMousePos(),&pTile,&arrayPos);

		if(success)
		{
			if(bMouse1 && !pTile->marked)
			{
				pTile->selsected = true;
				if(pTile->mine)
				{
					bMine = true;
				}
				else
				{
					if(pTile->minesNearby == 0)
					{
						Expand(arrayPos);
					}
				}
			}
			else if(bMouse2 && !pTile->selsected)
			{
				pTile->marked = !pTile->marked;

				if(pTile->marked)
				{
					m_uiMarkedCount++;

					if(pTile->mine)
					{
						m_uiMarkedCorrectlyCount++;
					}
				}
				else
				{
					m_uiMarkedCount--;

					if(pTile->mine)
					{
						m_uiMarkedCorrectlyCount--;
					}
				}
			}
		}
	}

	if(m_uiMarkedCorrectlyCount == m_uiMineCount)
		return (int)GameStatus::Won;

	if(bMine)
		return (int)GameStatus::Lost;

	return (int)GameStatus::Playing;
}

void Grid::Render(IRenderer& renderer)
{
	std::ostringstream stream;
	stream << "Mines: "<< m_uiMineCount - m_uiMarkedCount;

	renderer.DrawString(stream.str().c_str(),glm::vec2(0.0f,90.0f),glm::vec2(1.5f),glm::vec3(0.0f),0,FontAlignment::Center);

	IGrid::Render(renderer);
}

void Grid::RenderTileCallback(IRenderer& renderer, const Tile& tile, const glm::mat4& T) const
{
	std::ostringstream stream;
	glm::vec2 pos(T[3].x,T[3].y);

	if(tile.selsected)
	{
		if(tile.mine)
		{
			stream << "M";
		}
		else if(tile.minesNearby != 0)
		{
			stream << tile.minesNearby;
		}

		renderer.DrawString(stream.str().c_str(),pos,glm::vec2(1.5f),glm::vec3(0.0f),0,FontAlignment::Center);
		renderer.DrawSprite("selected",T);
	}
	else
	{
		if(tile.marked)
		{
			renderer.DrawString("X",pos,glm::vec2(1.5f),glm::vec3(0.0f),0,FontAlignment::Center);
		}

		renderer.DrawSprite("unselected",T);
	}
}

MineSweeper::MineSweeper() : m_grid(190.0f,150.0f,20,10), m_gameState(GameStatus::Playing), m_fTime(0.0)
{
}

void MineSweeper::Init(Game& game)
{
	CreatePlayingMenu(game);
	CreateMainMenu(game);
	

	m_timer.Reset();
	m_timer.Start();
}

void MineSweeper::CreatePlayingMenu(Game& game)
{
	UI::Menu* pMenu = new UI::Menu();

	const char* pResetButtonText = "Reset";

	Math::FRECT resetRect;
	game.GetRenderer().GetStringRec(pResetButtonText,glm::vec2(1.5f),resetRect);

	resetRect.SetCenter(glm::vec2(80.0f,92.0f));

	UI::GenericButton<void>* pButton = new UI::GenericButton<void>(resetRect,glm::vec3(0.0f),glm::vec3(1.0f,0.0f,0.0f),glm::vec2(1.5f),pResetButtonText);
	pButton->SetCallback(UI::GenericButton<void>::DELEGATE(this,&MineSweeper::Reset));

	pMenu->AddElement(pButton);

	m_gui.SetMenu(pMenu);
}

void MineSweeper::CreateMainMenu(Game& game)
{
	UI::Menu* pMenu = new UI::Menu();

	const char* pResetButtonText = "Play Again";

	Math::FRECT resetRect;
	game.GetRenderer().GetStringRec(pResetButtonText,glm::vec2(3.5f,4.0f),resetRect);

	resetRect.SetCenter(glm::vec2(0.0f,20.0f));

	UI::GenericButton<void>* pButton = new UI::GenericButton<void>(resetRect,glm::vec3(0.0f),glm::vec3(1.0f,0.0f,0.0f),glm::vec2(3.5f,4.0f),pResetButtonText);
	pButton->SetCallback(UI::GenericButton<void>::DELEGATE(this,&MineSweeper::Reset));

	pMenu->AddElement(pButton);

	m_mainMenu.SetMenu(pMenu);
}

void MineSweeper::Destroy(Game& game)
{

}

void MineSweeper::Reset()
{
	m_gameState = GameStatus::Playing;
	m_grid.Reset();
}

void MineSweeper::Update(Game& game)
{
	IKMInput& input = game.GetInput();

	m_fTime += game.GetDt();

	if(m_gameState == GameStatus::Playing)
	{
		m_gameState = (GameStatus)m_grid.Update(input);
		m_gui.Update(input,game.GetDt());
	}
	else
	{
		m_mainMenu.Update(input,game.GetDt());
	}
}

void MineSweeper::Draw(Game& game)
{
	IRenderer& renderer = game.GetRenderer();

	std::ostringstream stream;
	glm::vec2 drawPos(0.0f,-80.0f);

	if(m_gameState == GameStatus::Playing)
	{
		stream << "Time: " << (int)m_fTime << std::endl;
		stream << game.GetInput().GetTransformedMousePos().x;

		//drawPos = glm::vec2(0.0f,-80.0f);

		
		m_gui.Render(renderer);
	}
	else
	{
		if(m_gameState == GameStatus::Lost)
		{
			stream << "You lost";
		}
		else
		{
			stream << "You won";
		}

		m_mainMenu.Render(renderer);
	}

	m_grid.Render(renderer);
	renderer.DrawString(stream.str().c_str(),glm::vec2(0.0f,-80.0f),glm::vec2(1.5f),glm::vec3(0.0f),0,FontAlignment::Center);
}
#define PLUGIN_EXPORTS
#include "MineSweeper.h"
#include "Timer.h"
#include "Game.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <sstream>

extern "C" PLUGINDECL IPlugin*  CreatePlugin()
{
	return new MineSweeper();
}

std::ostream& operator <<(std::ostream& stream, const Tile& t)
{
	stream << t.marked <<" "<< t.mine <<" "<< t.minesNearby <<" "<< t.selsected;
	return stream;
}
std::istream& operator >>(std::istream& stream, Tile& t)
{
	stream >> t.marked >> t.mine >> t.minesNearby >> t.selsected;
	return stream;
}

Grid::Grid() : m_uiMineCount(0), m_uiMarkedCount(0), m_uiMarkedCorrectlyCount(0)
{
}

int Grid::Update(IKMInput& input)
{
	bool bMouse1 = input.MouseClick(0);
	bool bMouse2 = !bMouse1 && input.MouseClick(1);
	bool bMine = false;
	if(bMouse1 || bMouse2)
	{
		glm::ivec2 arrayPos;
		Tile* pTile = nullptr;
		bool success = WorldSpaceToTile(input.GetCursorPos(),&pTile,&arrayPos);

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

void Grid::Render(IRenderer& renderer) const
{
	std::ostringstream stream;
	stream << "Mines: "<< m_uiMineCount - m_uiMarkedCount;

	int width, height;
	renderer.GetDisplayMode(width,height);
	renderer.DrawString(stream.str().c_str(),glm::vec3(0.0f,height - 50.0f,0),0.5f);

	IGrid::Render(renderer);
}

bool Grid::Load(std::ifstream& stream)
{
	if(!stream.is_open())
			return false;

	stream >> m_uiMineCount >> m_uiMarkedCount >> m_uiMarkedCorrectlyCount;

	return IGrid<Tile>::Load(stream);
}

bool Grid::Save(std::ofstream& stream) const
{
	if(!stream.is_open())
			return false;

	stream << m_uiMineCount << " " << m_uiMarkedCount << " " << m_uiMarkedCorrectlyCount << std::endl;

	return IGrid<Tile>::Save(stream);
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

void Grid::RenderTileCallback(IRenderer& renderer, const Tile& tile, const glm::mat4& T) const
{
	glm::vec3 pos(T[3].x,T[3].y,T[3].z);
	glm::vec2 scale(T[0].x,T[1].y);
	glm::vec3 tileSize = glm::vec3(GetTileSize(),0.0f); // todo: this calculation should be moved elsewhere
	pos.x -= tileSize.x / 4.0f;
	pos.y += tileSize.x / 4.0f;

	if(tile.selsected)
	{
		std::ostringstream stream;

		if(tile.mine)
		{
			stream << "M";
		}
		else if(tile.minesNearby != 0)
		{
			stream << tile.minesNearby;
		}

		renderer.DrawString(stream.str().c_str(),pos,0.3f,glm::vec4(0.0f,0.5f,0.0f,1.0f));
		renderer.DrawSprite("selected",T);
	}
	else
	{
		if(tile.marked)
		{
			renderer.DrawString("X",pos,0.4f);
		}

		renderer.DrawSprite("unselected",T);
	}
}

void Grid::Expand(const glm::ivec2& pos)
{
	for(int y = -1; y <= 1; ++y)
	{
		for(int x = -1; x <= 1; ++x)
		{
			if(x == 0 && y == 0)
				continue;

			glm::ivec2 newPos(pos.x + x,pos.y + y);

			if(newPos.x < m_numTiles.x && newPos.x >= 0 && newPos.y < m_numTiles.y && newPos.y >= 0)
			{
				int newIndex = m_numTiles.x*newPos.y + newPos.x;

				if(!m_tiles[newIndex].marked && !m_tiles[newIndex].mine && !m_tiles[newIndex].selsected)
				{
					m_tiles[newIndex].selsected = true;

					if(m_tiles[newIndex].minesNearby == 0)
					{
						Expand(newPos);
					}
				}
			}
		}
	}
}

void Grid::BuildGrid()
{
	for(unsigned int i = 0; i < m_tiles.size(); ++i)
	{
		m_tiles[i].mine = rand() % 8 == 0;
		if(m_tiles[i].mine)
		{
			m_uiMineCount++;
		}
	}

	for(unsigned int i = 0; i < m_tiles.size(); ++i)
	{
		if(m_tiles[i].mine)
		{
			unsigned int uiMineCount = 0;
			int x = i % m_numTiles.x;
			int y = i / m_numTiles.x;
			
			for(int j = -1; j <= 1; ++j)
			{
				for(int k = -1; k <= 1; ++k)
				{
					if(k == 0 && j == 0)
						continue;

					int newX = x + j;
					int newY = y + k;
				
					if(newX < m_numTiles.x && newX >= 0 && newY < m_numTiles.y && newY >= 0)
					{
						int newIndex = m_numTiles.x * newY + newX;
						if(!m_tiles[newIndex].mine)
						{
							m_tiles[newIndex].minesNearby++;
						}
					}
				}
			}
		}
	}
}

MineSweeper::MineSweeper() : m_gameState(GameStatus::Playing), m_fTime(0.0)
{
}

void MineSweeper::Init(Game& game)
{
	CreatePlayingMenu(game);
	CreateMainMenu(game);

	IRenderer& renderer = game.GetRenderer();

	int width, height;
	renderer.GetDisplayMode(width,height);

	m_grid.SetGridSize(glm::vec2(width / 1.1f,height / 1.1f));
	m_grid.SetNumTiles(glm::ivec2(30,20));
	m_grid.SetCenter(glm::vec3(width / 2, height / 2,-2.0f));
	m_grid.Reset();

	renderer.SetRenderSpace(RenderSpace::Screen);

	IKMInput& input = game.GetInput();

	input.SetCursorSensitivity(1.5f);
	input.SetCursorPos(glm::vec2(width / 2, height / 2));
}

void MineSweeper::Destroy(Game& game)
{
	std::ofstream stream("gridSave.txt");
	m_grid.Save(stream);
}

void MineSweeper::Reset()
{
	m_gameState = GameStatus::Playing;
	m_fTime = 0;
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
	renderer.DrawString(stream.str().c_str(),glm::vec3(0.0f,-80.0f,0.0f),1.0f);

	glm::vec2 cursorPos = game.GetInput().GetCursorPos();
	glm::mat4 T = glm::translate(cursorPos.x + 0.5f,cursorPos.y + 0.5f,0.0f);
	T = glm::scale(T,40.0f,40.0f,1.0f);

	renderer.DrawSprite("cursor",T);
}

void MineSweeper::CreatePlayingMenu(Game& game)
{
	UI::Menu* pMenu = new UI::Menu();

	const char* pResetButtonText = "Reset";

	Math::FRECT resetRect;
	game.GetRenderer().GetStringRec(pResetButtonText,glm::vec2(6.0f),resetRect);

	resetRect.SetCenter(glm::vec2(80.0f,92.0f));

	UI::GenericButton<void>* pButton = new UI::GenericButton<void>(resetRect,glm::vec3(0.0f),glm::vec3(1.0f,0.0f,0.0f),1.0f,pResetButtonText);
	pButton->SetCallback(UI::GenericButton<void>::DELEGATE(this,&MineSweeper::Reset));

	pMenu->AddElement(pButton);

	m_gui.SetMenu(pMenu);
}

void MineSweeper::CreateMainMenu(Game& game)
{
	UI::Menu* pMenu = new UI::Menu();

	const char* pResetButtonText = "Play Again";

	Math::FRECT resetRect;
	game.GetRenderer().GetStringRec(pResetButtonText,glm::vec2(10.0f),resetRect);

	resetRect.SetCenter(glm::vec2(0.0f,20.0f));

	UI::GenericButton<void>* pButton = new UI::GenericButton<void>(resetRect,glm::vec3(0.0f),glm::vec3(1.0f,0.0f,0.0f),1.0f,pResetButtonText);
	pButton->SetCallback(UI::GenericButton<void>::DELEGATE(this,&MineSweeper::Reset));

	pMenu->AddElement(pButton);

	m_mainMenu.SetMenu(pMenu);
}
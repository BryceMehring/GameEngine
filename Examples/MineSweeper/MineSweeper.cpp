#include "MineSweeper.h"
#include "Timer.h"
#include "Game.h"
#include "GUIFactory.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <sstream>
#include <functional>

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

const int Grid::s_adjacentTiles[][2] =
{
	{ -1, -1 }, { 0, -1 }, { 1, -1 },
	{ -1, 0 }, { 1, 0 }, { -1, 1 },
	{ 0, 1 }, { 1, 1 }
};

Grid::Grid() : m_uiMineCount(0), m_uiMarkedCount(0), m_uiMarkedCorrectlyCount(0)
{
}

int Grid::Update(IInput& input)
{
	bool bMouse1 = input.MouseClick(0);
	bool bMouse2 = !bMouse1 && input.MouseClick(1);
	bool bMine = false;
	if(bMouse1 || bMouse2)
	{
		glm::uvec2 arrayPos;
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
				if (m_uiMarkedCount < m_uiMineCount || pTile->marked)
				{
					pTile->marked = !pTile->marked;

					if (pTile->marked)
					{
						m_uiMarkedCount++;

						if (pTile->mine)
						{
							m_uiMarkedCorrectlyCount++;
						}
					}
					else
					{
						m_uiMarkedCount--;

						if (pTile->mine)
						{
							m_uiMarkedCorrectlyCount--;
						}
					}
				}
			}
		}
	}

	if(m_uiMarkedCorrectlyCount == m_uiMineCount)
		return static_cast<int>(GameStatus::Won);

	if(bMine)
		return static_cast<int>(GameStatus::Lost);

	return static_cast<int>(GameStatus::Playing);
}

void Grid::Render(IRenderer& renderer) const
{
	std::ostringstream stream;
	stream << "Mines: "<< m_uiMineCount - m_uiMarkedCount;

	int height;
	renderer.GetDisplayMode(nullptr,&height);
	renderer.DrawString(stream.str().c_str(),glm::vec3(0.0f,height - 50.0f,0));

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
	glm::vec2 scale(T[0].x,T[1].y);
	glm::vec3 pos(T[3].x, T[3].y + scale.y / 2.0f, T[3].z);

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

		renderer.DrawString(stream.str().c_str(),
			pos, scale.y, glm::vec4(1.0f),
			nullptr, FontAlignment::Center);
	}
	else
	{
		if(tile.marked)
		{
			renderer.DrawString("X", pos, scale.y, glm::vec4(1.0f),
				nullptr, FontAlignment::Center);
		}

		renderer.DrawSprite("tile",T,glm::vec4(glm::vec3(1.0f),0.8f));
	}
}

void Grid::Expand(const glm::uvec2& pos)
{
	for (auto iter : s_adjacentTiles)
	{
		glm::uvec2 newPos(pos.x + iter[0],pos.y + iter[1]);

		if(newPos.x < m_numTiles.x && newPos.y < m_numTiles.y)
		{
			unsigned int newIndex = m_numTiles.x*newPos.y + newPos.x;

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

void Grid::BuildGrid()
{
	for(unsigned int i = 0; i < m_tiles.size(); ++i)
	{
		m_tiles[i].mine = rand() % 8 == 0;

		if(m_tiles[i].mine)
		{
			unsigned int x = i % m_numTiles.x;
			unsigned int y = i / m_numTiles.x;

			for (auto iter : s_adjacentTiles)
			{
				unsigned int newX = x + iter[0];
				unsigned int newY = y + iter[1];

				if (newX < m_numTiles.x && newY < m_numTiles.y)
				{
					int newIndex = m_numTiles.x * newY + newX;
					if (!m_tiles[newIndex].mine)
					{
						m_tiles[newIndex].minesNearby++;
					}
				}
			}

			++m_uiMineCount;
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
	renderer.GetDisplayMode(&width,&height);

	m_grid.SetGridSize(glm::vec2(width / 1.1f,height / 1.1f));
	m_grid.SetNumTiles(glm::uvec2(30,20));
	m_grid.SetCenter(glm::vec3(width / 2, height / 2,-2.0f));
	m_grid.Reset();

	renderer.SetRenderSpace(RenderSpace::Screen);

	IInput& input = game.GetInput();

	input.SetCursorSensitivity(1.0f);
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
	IInput& input = game.GetInput();

	m_fTime += game.GetDt();

	ClampMouse(input, game.GetRenderer());

	if(m_gameState == GameStatus::Playing)
	{
		m_gameState = static_cast<GameStatus>(m_grid.Update(input));
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
		stream << "Time: " << static_cast<int>(m_fTime);
		
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

	int width, height;
	game.GetRenderer().GetDisplayMode(&width, &height);

	renderer.DrawString(stream.str().c_str(),
		glm::vec3(width - 200, height - 5, 0.0f),
		40.0f,glm::vec4(1.0f),nullptr,FontAlignment::Center);

	// Draw cursor
	glm::vec2 cursorPos = game.GetInput().GetCursorPos();
	glm::mat4 T = glm::translate(glm::vec3(cursorPos.x + 20.0f,cursorPos.y - 20.0f,1.0f));
	T = glm::scale(T,glm::vec3(40.0f,40.0f,1.0f));

	renderer.DrawSprite("cursor",T);

	// Draw Animated background
	static float xPos = width / 2.0f;

	xPos += 0.1f * game.GetDt();

	T = glm::translate(glm::vec3(width / 2.0f, height / 2.0f, -50.0f));
	T = glm::scale(T, glm::vec3(m_grid.GetGridSize(),1.0f));

	renderer.SetShaderValue("landTech", "offset", glm::vec2(cos(xPos), 0.0f));
	renderer.DrawSprite("land", T, glm::vec4(1.0f), glm::vec2(2.0f),0,"landTech");
}

void MineSweeper::CreatePlayingMenu(Game& game)
{
	int width, height;
	game.GetRenderer().GetDisplayMode(&width, &height);

	UI::Menu* pMenu = new UI::Menu();

	auto pButton = UI::GUIFactory<UI::Button>::CreateElement(game, glm::vec2(width / 2, height),
		glm::vec3(1.0f), glm::vec3(1.0f, 0.0f, 0.0f), 40.0f, "Reset", std::bind(&MineSweeper::Reset, this));

	pMenu->AddElement(pButton);

	m_gui.SetMenu(pMenu);
}

void MineSweeper::CreateMainMenu(Game& game)
{
	int width, height;
	game.GetRenderer().GetDisplayMode(&width, &height);

	UI::Menu* pMenu = new UI::Menu();

	auto pButton = UI::GUIFactory<UI::Button>::CreateElement(game, glm::vec2(width / 2, height),
		glm::vec3(1.0f), glm::vec3(1.0f, 0.0f, 0.0f), 40.0f, "Play Again", std::bind(&MineSweeper::Reset, this));

	pMenu->AddElement(pButton);

	m_mainMenu.SetMenu(pMenu);
}

void MineSweeper::ClampMouse(IInput& input, IRenderer& renderer) const
{
	int width, height;
	renderer.GetDisplayMode(&width, &height);

	glm::vec2 cursorPos = input.GetCursorPos();

	if (cursorPos.x < 0 || cursorPos.x > width || cursorPos.y < 0 || cursorPos.y > height)
	{
		if (cursorPos.x < 0)
		{
			cursorPos.x = 0.0f;
		}
		else if (cursorPos.x > width)
		{
			cursorPos.x = static_cast<float>(width);
		}
		if (cursorPos.y < 0)
		{
			cursorPos.y = 0.0f;
		}
		else if (cursorPos.y > height)
		{
			cursorPos.y = static_cast<float>(height);
		}

		input.SetCursorPos(cursorPos);
	}
}

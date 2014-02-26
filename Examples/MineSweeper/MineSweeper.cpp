#include "MineSweeper.h"
#include "Timer.h"
#include "Game.h"
#include "GUIFactory.h"
#include "Slider.h"
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
		bool success = WorldSpaceToTile(glm::vec2(input.GetCursorPos()),&pTile,&arrayPos);

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
					else
					{
						// Count the number of tiles that are marked
						unsigned int markedCounter = 0;
						for (auto iter : s_adjacentTiles)
						{
							glm::uvec2 newPos(arrayPos.x + iter[0],arrayPos.y + iter[1]);

							if(newPos.x < m_numTiles.x && newPos.y < m_numTiles.y)
							{
								unsigned int newIndex = m_numTiles.x*newPos.y + newPos.x;
								if(m_tiles[newIndex].marked)
								{
									markedCounter++;
								}
							}
						}

						if(markedCounter == pTile->minesNearby)
						{
							for (auto iter : s_adjacentTiles)
							{
								glm::uvec2 newPos(arrayPos.x + iter[0],arrayPos.y + iter[1]);

								if(newPos.x < m_numTiles.x && newPos.y < m_numTiles.y)
								{
									unsigned int newIndex = m_numTiles.x*newPos.y + newPos.x;
									if(!m_tiles[newIndex].marked)
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

MineSweeper::MineSweeper() : m_gameState(GameStatus::Playing), m_fTime(0.0), m_uiPlayingMenu(0), m_uiMainMenu(0)
{
}

void MineSweeper::Init(Game& game)
{
	// Build the menus
	CreatePlayingMenu(game);
	CreateMainMenu(game);

	IRenderer& renderer = game.GetRenderer();

	// Render everything in screen space
	renderer.SetRenderSpace(RenderSpace::Screen);

	int width, height;
	renderer.GetDisplayMode(&width,&height);

	// Set up the grid
	m_grid.SetGridSize(glm::vec2(width / 1.1f,height / 1.1f));
	m_grid.SetNumTiles(glm::uvec2(30,20));
	m_grid.SetCenter(glm::vec3(width / 2, height / 2,-2.0f));
	m_grid.Reset();

	IInput& input = game.GetInput();
	input.SetCursorPos(glm::ivec2(width / 2, height / 2));
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
	m_gui.SetNode(m_uiPlayingMenu);
	m_grid.Reset();
}

void MineSweeper::Update(Game& game)
{
	IInput& input = game.GetInput();

	m_fTime += game.GetDt();

	if(m_gameState == GameStatus::Playing)
	{
		m_gameState = static_cast<GameStatus>(m_grid.Update(input));

		if(m_gameState != GameStatus::Playing)
		{
			m_gui.SetNode(m_uiMainMenu);
		}
	}

	m_gui.Update(input,game.GetDt());
}

void MineSweeper::Draw(Game& game)
{
	IRenderer& renderer = game.GetRenderer();

	std::ostringstream stream;
	glm::vec2 drawPos(0.0f,-80.0f);

	if(m_gameState == GameStatus::Playing)
	{
		stream << "Time: " << static_cast<int>(m_fTime);
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
	}

	m_gui.Render(renderer);
	m_grid.Render(renderer);

	int width, height;
	game.GetRenderer().GetDisplayMode(&width, &height);

	renderer.DrawString(stream.str().c_str(),
		glm::vec3(width - 200, height - 5, 0.0f),
		40.0f,glm::vec4(1.0f),nullptr,FontAlignment::Center);

	// Draw Animated background
	static float xPos = width / 2.0f;

	xPos += 0.1f * game.GetDt();

	glm::mat4 T = glm::translate(glm::vec3(width / 2.0f, height / 2.0f, -50.0f));
	T = glm::scale(T, glm::vec3(m_grid.GetGridSize(),1.0f));

	renderer.SetShaderValue("landTech", "offset", glm::vec2(cos(xPos), 0.0f));
	renderer.DrawSprite("land", T, glm::vec4(1.0f), glm::vec2(2.0f),0,"landTech");
}

void MineSweeper::CreatePlayingMenu(Game& game)
{
	int width, height;
	game.GetRenderer().GetDisplayMode(&width, &height);

	m_uiPlayingMenu = m_gui.CreateNode();

	auto pButton = UI::GUIFactory<UI::Button>::CreateElement(game, glm::vec2(width / 2, height),
		glm::vec3(1.0f), glm::vec3(1.0f, 0.0f, 0.0f), 40.0f, "Reset", std::bind(&MineSweeper::Reset, this));

	m_gui.AddElement(m_uiPlayingMenu, pButton);
	m_gui.SetNode(m_uiPlayingMenu);
}

void MineSweeper::SliderCallback(float v, bool bWidth)
{
	glm::uvec2 numTiles = m_grid.GetNumTiles();
	numTiles = bWidth ? glm::uvec2(v, numTiles.y) : glm::uvec2(numTiles.x, v);

	m_grid.SetNumTiles(numTiles);
}

void MineSweeper::CreateMainMenu(Game& game)
{
	using namespace std::placeholders;

	int width, height;
	game.GetRenderer().GetDisplayMode(&width, &height);

	m_uiMainMenu = m_gui.CreateNode();

	auto pButton = UI::GUIFactory<UI::Button>::CreateElement(game, glm::vec2(width / 2, height),
		glm::vec3(1.0f), glm::vec3(1.0f, 0.0f, 0.0f), 40.0f, "Play Again", std::bind(&MineSweeper::Reset, this));

	auto pWidthSlider = UI::GUIFactory<UI::Slider>::CreateElement(game,glm::vec2(width / 4, height / 2),glm::vec2(3 * width / 4, height / 2),10.0f,100.0f,std::string("button"),std::bind(&MineSweeper::SliderCallback,this,_1,true));
	auto pHeightSlider = UI::GUIFactory<UI::Slider>::CreateElement(game,glm::vec2(width / 4, height / 2 + 50),glm::vec2(3 * width / 4, height / 2 + 50),10.0f,100.0f,std::string("button"),std::bind(&MineSweeper::SliderCallback,this,_1,false));

	m_gui.AddElement(m_uiMainMenu, pButton);
	m_gui.AddElement(m_uiMainMenu, pWidthSlider);
	m_gui.AddElement(m_uiMainMenu, pHeightSlider);
	m_gui.LinkNodes(m_uiMainMenu,m_uiPlayingMenu);
}

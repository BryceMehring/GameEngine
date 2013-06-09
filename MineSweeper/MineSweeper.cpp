#define PLUGIN_EXPORTS
#define GLFW_DLL
#include "MineSweeper.h"
#include "Game.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

RTTI_IMPL(MineSweeper);

extern "C" PLUGINDECL IPlugin*  CreatePlugin()
{
	return new MineSweeper();
}

Grid::Grid() : m_tiles(20*20)
{
	for(unsigned int i = 0; i < m_tiles.size(); ++i)
	{
		m_tiles[i].mine = rand() % 2;
	}
}

void Grid::Render(IRenderer& renderer)
{
	for(int i = -50; i < 50; i += 5)
	{
		for(int j = -50; j < 50; j += 5)
		{
			glm::mat4 T = glm::translate((float)i,(float)j,-1.0f);
			T = glm::scale(T,5.0f,5.0f,1.0f);

			bool bMine = m_tiles[(i+50)+(j+50)].mine;

			if(bMine)
			{
				renderer.DrawString("M",glm::vec2(i,j),glm::vec2(1.5f),glm::vec3(0.0f),0,FontAlignment::Center);
			}

			renderer.DrawSprite("selected",T);
		}
	}
}

void MineSweeper::Init(Game& game)
{
	//UI::Menu* pMenu = new UI::Menu();

	//UI::GenericButton<void>* pButton = new UI::GenericButton<void>(
}

void MineSweeper::Destroy(Game& game)
{

}

void MineSweeper::Update(Game& game)
{

}

void MineSweeper::Draw(Game& game)
{
	IRenderer& renderer = game.GetRenderer();

	m_grid.Render(renderer);

}
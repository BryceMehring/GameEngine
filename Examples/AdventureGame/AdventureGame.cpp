#define PLUGIN_EXPORTS

#include "AdventureGame.h"
#include "PluginManager.h"
#include "IKMInput.h"
#include "Game.h"
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>
#include <sstream>

extern "C" PLUGINDECL IPlugin* CreatePlugin()
{
	return new AdventureGame();
}

Player::Player(Camera* pCam) : m_pos(0.0f), m_iCell(0), m_pCamera(pCam)
{
}

Direction Player::Update(IKMInput& input, double dt)
{
	static double tim = 0.0;
	tim += dt;

	if(input.KeyDown(GLFW_KEY_UP,false) || input.KeyDown('W',false))
	{
		m_pos.y += 20.0f * dt;

		if(tim > 1/5.0f)
		{
			tim = 0.0;
			m_iCell++;

			if(m_iCell > 11 || m_iCell < 9)
			{
				m_iCell = 9;
			}
		}
	}
	if(input.KeyDown(GLFW_KEY_DOWN,false) || input.KeyDown('S',false))
	{
		m_pos.y -= 20.0f * dt;

		if(tim > 1/5.0f)
		{
			tim = 0.0;
			m_iCell++;

			if(m_iCell >= 3)
			{
				m_iCell = 0;
			}
		}
	}
	if(input.KeyDown(GLFW_KEY_LEFT,false) || input.KeyDown('A',false))
	{
		m_pos.x -= 20.0f * dt;

		if(tim > 1/5.0f)
		{
			tim = 0.0;
			m_iCell++;

			if(m_iCell < 3 || m_iCell > 5)
			{
				m_iCell = 3;
			}
		}
	}
	else if(input.KeyDown(GLFW_KEY_RIGHT,false) || input.KeyDown('D',false))
	{
		m_pos.x += 20.0f * dt;

		if(tim > 1/5.0f)
		{
			tim = 0.0;
			m_iCell++;

			if(m_iCell < 6 || m_iCell > 8)
			{
				m_iCell = 6;
			}
		}
	}

	Direction dirToMove = Direction::NONE;
	if(m_pos.x > 100.0f)
	{
		dirToMove = Direction::EAST;
		m_pos.x = -100.0f;
	}
	else if(m_pos.x < -100.0f)
	{
		dirToMove = Direction::WEST;
		m_pos.x = 100.0f;
	}
	else if (m_pos.y > 100.0f)
	{
		dirToMove = Direction::NORTH;
		m_pos.y = -100.0f;
	}
	else if (m_pos.y < -100.0f)
	{
		dirToMove = Direction::SOUTH;
		m_pos.y = 100.0f;
	}

	return dirToMove;

}

void Player::Render(IRenderer& renderer)
{
	glm::mat4 T = glm::translate(m_pos.x,m_pos.y,0.0f);
	T = glm::scale(T,6.0f,12.0f,1.0f);

	renderer.DrawSprite("robot",T,glm::vec3(1.0f),glm::vec2(1.0f),m_iCell);
	renderer.DrawString("HealthBar",glm::vec3(0.0f),glm::vec2(15.0f),glm::vec3(0.0f));

	m_pCamera->lookAt(glm::vec3(m_pos,1.0f),glm::vec3(m_pos,0.0f),glm::vec3(0.0f,1.0f,0.0f));
	m_pCamera->update();
}

AdventureGame::AdventureGame() : m_pPlayer(nullptr), m_pos(0,0)
{
	
}

void AdventureGame::Init(Game& game)
{
	Camera* pCam = CreateCamera();
	pCam->setLens(200.0f,200.0f,0.0f,5000.0f);
	pCam->lookAt(glm::vec3(0.0f,0.0f,1.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
	pCam->update();

	m_pPlayer = new Player(pCam);
	game.GetRenderer().SetCamera(pCam);
}
void AdventureGame::Destroy(Game& game)
{
	delete m_pPlayer;
}
void AdventureGame::Update(Game& game)
{
	Direction dir = m_pPlayer->Update(game.GetInput(),game.GetDt());

	if(dir == Direction::NORTH)
	{
		m_pos.y++;
	}
	else if(dir == Direction::SOUTH)
	{
		m_pos.y--;
	}
	else if (dir == Direction::WEST)
	{
		m_pos.x--;
	}
	else if(dir == Direction::EAST)
	{
		m_pos.x++;
	}

}
void AdventureGame::Draw(Game& game)
{
	std::ostringstream stream;
	stream << "World Pos:("<< m_pos.x<<","<<m_pos.y<<")";

	IRenderer& renderer = game.GetRenderer();

	//renderer.DrawSprite("bush",glm::translate(40.0f,40.0f,0.0f)*glm::scale(5.0f,5.0f,1.0f));
	glm::mat4 T = glm::translate(0.0f,0.0f,-1.0f);
	T = glm::scale(T,400.0f,400.0f,0.0f);
	renderer.DrawSprite("grass",T,glm::vec3(20.0f,20.0f,-10.0f));
	// game.GetInput().GetTransformedMousePos()
	renderer.DrawString(stream.str().c_str(),glm::vec3(0.0f,0.0f,-10.0f),glm::vec2(1.5f),glm::vec3(1.0f),0,FontAlignment::Right);
	this->m_pPlayer->Render(renderer);


}
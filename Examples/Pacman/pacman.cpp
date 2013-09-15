#include "pacman.h"
#include "SpatialHashTable.h"
#include "Game.h"
#include <sstream>

#pragma comment(lib,"Game Engine.lib")

RTTI_IMPL(Pacman);

PLUGINDECL IPlugin* CreatePlugin(PluginManager& pm)
{
	return new Pacman(pm);
}

Pacman::Pacman(PluginManager&) : m_iSelectedSprite(0)
{
}

Pacman::~Pacman() 
{
}

void Pacman::Init(Game& game)
{
	m_tiles.resize(20*20);

	for(int i = 0; i < m_tiles.size(); ++i)
	{
		if(i % 2 == 0)
		{
			m_tiles[i] = "bdot";
		}
	}
}
void Pacman::Destroy(Game& game)
{

}

void Pacman::Update(Game& game)
{
	if(game.GetInput().KeyDown(KeyCode::RIGHT))
	{
		m_iSelectedSprite = (m_iSelectedSprite + 1);
	}
	else if(game.GetInput().KeyDown(KeyCode::LEFT))
	{
		m_iSelectedSprite = (m_iSelectedSprite - 1);
	}
}
void Pacman::Draw(Game& game)
{
	::D3DXVECTOR2 pos(-45.0f,45.0f);

	::D3DXMATRIX S;
	::D3DXMatrixScaling(&S,5.0f,5.0f,1.0f);

	for(unsigned int i = 0; i < 10; ++i)
	{
		for(unsigned int j = 0; j < 10; ++j)
		{
			if(!m_tiles[j + i*20].empty())
			{
				::D3DXMATRIX T;
				::D3DXMatrixTranslation(&T,pos.x,pos.y,0.0f);

				game.GetRenderer().Get2DRenderer().DrawSprite(S*T,m_tiles[j + i*20],0);
			}
			pos.x += 6;
		}

		pos.x = -45.0f;
		pos.y -= 6;
	}

	pos = ::D3DXVECTOR2(0.0f,0.0f);

	::D3DXMATRIX T;
	D3DXMatrixScaling(&S,100.0f,100.0f,1.0f);
	::D3DXMatrixTranslation(&T,pos.x,pos.y,5.0f);

	game.GetRenderer().Get2DRenderer().DrawSprite(S*T,"space",0,1.0f,1.0f);
	

	//::std::stringstream stream;
	//stream << "Selected Sprite: "<< m_iSelectedSprite<<endl;


	//game.GetRenderer().Get2DRenderer().DrawString(stream.str().c_str(),::D3DXVECTOR2(-15.0f,10.0f));
	
}
#include "Astroids.h"
#include "DxPolygon.h"
#include "IKMInput.h"
#include "IRenderer.h"
#include "Game.h"
#include "VecMath.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

using namespace boost::geometry;

RTTI_IMPL(Astroids);


void ClampWrap(D3DXVECTOR2& pos)
{
	RECT R;
	GetClientRect(::GetActiveWindow(),&R);

	if(pos.x > R.right)
	{
		pos.x = 0.0f;
	}
	else if(pos.x < 0.0f)
	{
		pos.x = R.right;
	}

	if(pos.y > R.bottom)
	{
		pos.y = 0.0f;
	}
	else if(pos.y < 0.0f)
	{
		pos.y = R.bottom;
	}
}


ASpaceShip::ASpaceShip(const D3DXVECTOR2& pos) : m_pPolygon(nullptr), m_pos(pos), m_dir(3.14f), m_acceleration(1000.0f)
{
	D3DXVECTOR2 shipArray[4];
	BuildShipLines(shipArray);
	m_pPolygon = new DxTriangle(shipArray,4);
}

ASpaceShip::~ASpaceShip()
{
	delete m_pPolygon;
}

void ASpaceShip::Update(IKMInput& input, double dt)
{
	if(input.KeyDown(KeyCode::W,false))
	{
		m_acceleration += 2000.0f * dt;
	}
	if(input.KeyDown(KeyCode::A,false))
	{
		m_dir -= 5.0f * dt;
	}
	if(input.KeyDown(KeyCode::D,false))
	{
		m_dir += 5.0f * dt;
	}
	if(input.KeyDown(KeyCode::SPACE))
	{
		m_beams.push_back(ASpaceShip::Beam((*m_pPolygon)[1],m_dir));
	}

	for(auto iter = m_beams.begin(); iter != m_beams.end(); )
	{
		iter->age += dt;

		if(iter->age > 0.7f)
		{
			iter = m_beams.erase(m_beams.begin());
		}
		else
		{
			iter->pos.x -= sin(iter->dir) * 500 * dt;
			iter->pos.y += cos(iter->dir) * 500 * dt;

			ClampWrap(iter->pos);
			++iter;
		}
	}
	
	m_pos.x -= sin(m_dir) * m_acceleration * dt;
	m_pos.y += cos(m_dir) * m_acceleration * dt;
	m_acceleration -= 400*dt;

	if(m_acceleration < 0.0f)
	{
		m_acceleration = 0.0f;
	}

	ClampWrap(m_pos);

	if(abs(m_dir) > 6.283f)
	{
		m_dir = 0.0f;
	}
}

void ASpaceShip::Render(IRenderer& renderer)
{
	D3DXVECTOR2 shipArray[4];
	BuildShipLines(shipArray);

	m_pPolygon->ConstructFromArray(shipArray,4);
	m_pPolygon->Render(renderer);

	for(auto iter = m_beams.begin(); iter != m_beams.end(); ++iter)
	{
		::D3DXMATRIX T, S;
		::D3DXMatrixTranslation(&T,iter->pos.x,iter->pos.y,0.0f);
		::D3DXMatrixScaling(&S,0.2f,0.2f,1.0f);
		renderer.Get2DRenderer().DrawSprite(S*T,"point",4);
	}
}

void ASpaceShip::BuildShipLines(D3DXVECTOR2* pArray)
{
	//overlaps(1,2);
	pArray[0] = D3DXVECTOR2(m_pos.x,m_pos.y);
	pArray[1] = D3DXVECTOR2(m_pos.x + 10.0f,m_pos.y + 35.0f);
	pArray[2] = D3DXVECTOR2(m_pos.x + 20.0f,m_pos.y);
	pArray[3] = D3DXVECTOR2(m_pos.x,m_pos.y);

	D3DXMATRIX R;
	D3DXVECTOR2 center(pArray[1].x,pArray[0].y);
	D3DXMatrixTransformation2D(&R,0,0,0,&center,m_dir,0);
	for(int i = 0; i < 4; ++i)
	{
		D3DXVec2TransformCoord(pArray + i,pArray + i,&R);
	}
}

Astroid::Astroid(DxPolygon* pPolygon, const D3DXVECTOR2& pos, float a) : m_pPolygon(pPolygon), m_pos(pos), m_angle(a)
{
}
Astroid::~Astroid()
{
	delete m_pPolygon;
}

void Astroid::Update(double dt)
{
	m_anglet = m_angle * dt;
}

void Astroid::Render(IRenderer& renderer)
{
	//std::vector<D3DXVECTOR2> vert(m_vertex);

	D3DXMATRIX R;
	D3DXVECTOR2 center(m_pos.x,m_pos.y);
	D3DXMatrixTransformation2D(&R,0,0,0,&center,m_anglet,0);
	
	for(int i = 0; i < m_pPolygon->GetSize(); ++i)
	{
		D3DXVec2TransformCoord(&(*m_pPolygon)[i],&(*m_pPolygon)[i],&R);
	}

	m_pPolygon->Render(renderer);
}

Astroids::Astroids() : m_ship(D3DXVECTOR2(100.0f,100.0f))
{
	BuildAstroids();
	//RECT R;
	//GetWindowRect(R);

	//m_pQuadtree = new QuadTree(Math::FRECT(D3DXVECTOR2(R.left,R.top),D3DXVECTOR2(R.right,R.bottom)));
	//m_pQuadtree->Insert(
}

Astroids::~Astroids()
{
	
}

//void Astroids::Init(class Game&) {}
//void Astroids::Destroy(class Game&) {}
void Astroids::BuildAstroids()
{
	RECT R;
	GetClientRect(::GetActiveWindow(),&R);

	for(unsigned int n = 0; n < 15; ++n)
	{
		float r = Math::GetRandFloat(20.0f,50.0f);
		float x = Math::GetRandFloat(0.0f,R.right - 100.0f);
		float y = Math::GetRandFloat(0.0f,R.bottom - 100.0f);

		unsigned int size = 5 + rand() % 7;
		float a = 0.0f; 
		float b = 360.0f / size;

		std::vector<D3DXVECTOR2> pos;
		pos.resize(size+1);
		
		for(int i = 0; i < size; ++i)
		{
			float angle = Math::GetRandFloat(a,b);
			pos[i] = D3DXVECTOR2(r * sin(angle * 0.01745f) + r + x,r * cos(angle * 0.01745f) + r + y);

			a += 360.0f / size;
			b += 360.0f / size;
		}

		pos[size] = pos[0];
		
		m_Astroids.push_back(Astroid(new DxPolygon(pos),D3DXVECTOR2(x,y),Math::GetRandFloat(-1.0f,2.0f)));
	}
	
}
void Astroids::Update(class Game& game)
{
	this->m_ship.Update(game.GetInput(),game.GetDt());

	for(auto iter = m_Astroids.begin(); iter != m_Astroids.end(); ++iter)
	{
		iter->Update(game.GetDt());
	}
}
void Astroids::Draw(class Game& game)
{
	//POINT P;
	//game.GetRenderer().GetWinSize(P);

	//RECT R = {P.x,P.y,P.x+100,P.y+100};
	//RECT R = {100,100,200,200};
	
	//R.left = 
	m_ship.Render(game.GetRenderer());
	for(auto iter = m_Astroids.begin(); iter != m_Astroids.end(); ++iter)
	{
		iter->Render(game.GetRenderer());
	}
	//game.GetRenderer().Get2DRenderer().DrawString("Astroids",game.GetInput().MousePos(),0xffffffff);
}
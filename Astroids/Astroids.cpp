#include "Astroids.h"
#include "DxPolygon.h"
#include "IRenderer.h"
#include "Game.h"
#include "VecMath.h"
#include "BNew.h"

#include <sstream>

RTTI_IMPL(Astroids);

void GetWindowRect(RECT& R)
{
	GetClientRect(::GetActiveWindow(),&R);
}

const D3DXVECTOR2 normal[4] = 
{
	D3DXVECTOR2(1.0f,0.0f),
	D3DXVECTOR2(-1.0f,0.0f),
	D3DXVECTOR2(0.0f,1.0f),
	D3DXVECTOR2(0.0f,-1.0f)
};

PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr)
{
	return new Astroids(mgr);
}

void ClampWrap(D3DXVECTOR3& pos)
{
	if(pos.x < -20.0f)
	{
		pos.x = 20.0f;
	}
	else if(pos.x > 20.0f)
	{
		pos.x = -20.0f;
	}

	if(pos.y < -20.0f)
	{
		pos.y = 20.0f;
	}
	else if(pos.y > 20.0f)
	{
		pos.y = -20.0f;
	}
}


ASpaceShip::ASpaceShip(const D3DXVECTOR3& pos) : m_pPolygon(nullptr), m_pos(pos), m_dir(0.0f), m_acceleration(1000.0f), m_inputTime(0.0)
{
	D3DXVECTOR3 shipArray[4];
	shipArray[0] = D3DXVECTOR3(m_pos.x,m_pos.y,0.0f);
	shipArray[1] = D3DXVECTOR3(m_pos.x + 0.05f,m_pos.y + 0.2f,0.0f);
	shipArray[2] = D3DXVECTOR3(m_pos.x + 0.1f,m_pos.y,0.0f);
	shipArray[3] = D3DXVECTOR3(m_pos.x,m_pos.y,0.0f);

	m_pPolygon = new DxTriangle(shipArray,4);
}

ASpaceShip::~ASpaceShip()
{
	delete m_pPolygon;
}

void ASpaceShip::Update(IKMInput& input, double dt)
{ 
	if(input.KeyDown(KeyCode::W,false) || input.KeyDown(KeyCode::UP,false))
	{
		m_acceleration += 2000.0f * dt;
	}
	if(input.KeyDown(KeyCode::A,false) || input.KeyDown(KeyCode::LEFT,false))
	{
		m_dir -= 5.0f * dt;
	}
	if(input.KeyDown(KeyCode::D,false) || input.KeyDown(KeyCode::RIGHT,false))
	{
		m_dir += 5.0f * dt;
	}
	if(input.KeyDown(KeyCode::SPACE,true))
	{
		m_inputTime += dt;

		//if(m_inputTime > 0.5)
		{
			float z = Math::GetRandFloat(0.0f,50.0f);
			m_beams.push_back(ASpaceShip::Beam(D3DXVECTOR3(0.0f,0.0f,z),m_dir,m_acceleration + 2.0f));

			m_inputTime = 0.0;
		}
	}

	for(auto iter = m_beams.begin(); iter != m_beams.end(); )
	{
		iter->age += dt;

		if(iter->age > 10.0f)
		{
			iter = m_beams.erase(m_beams.begin());
		}
		else
		{
			D3DXVECTOR3 dir(sin(iter->dir),cos(iter->dir),0.0f);

			iter->pos += dir * iter->speed * dt;

			ClampWrap(iter->pos);
			++iter;
		}
	}

	D3DXVECTOR3 shipDir(sin(m_dir),cos(m_dir),0.0f);

	m_pos += shipDir * m_acceleration * dt;
	m_acceleration -= 400*dt;
	
	m_acceleration = Math::Clamp(m_acceleration,0.0f,700.0f);

	ClampWrap(m_pos);

	if(abs(m_dir) > 6.283f)
	{
		m_dir = 0.0f;
	}
}

void ASpaceShip::Render(IRenderer& renderer)
{
	D3DXVECTOR3 shipArray[4];
	BuildShipLines(shipArray);

	//m_pPolygon->ConstructFromArray(shipArray,4);
	m_pPolygon->SetAngle(-this->m_dir);
	m_pPolygon->Render(renderer);

	D3DXMATRIX S;
	D3DXMatrixScaling(&S,0.2f,0.2f,1.0f);

	for(auto iter = m_beams.begin(); iter != m_beams.end(); ++iter)
	{
		D3DXMATRIX S, T;
		D3DXMatrixTranslation(&T,iter->pos.x,iter->pos.y,0.0f);
		//D3DXMatrixScaling(&S,.1f,.1f,0.0f);
		renderer.Get2DRenderer().DrawSprite(T,"ball");
	}
}

void ASpaceShip::BuildShipLines(D3DXVECTOR3* pArray)
{
	//overlaps(1,2);
	/*pArray[0] = D3DXVECTOR3(m_pos.x,m_pos.y,0.0f);
	pArray[1] = D3DXVECTOR3(m_pos.x + 10.0f,m_pos.y + 35.0f,0.0f);
	pArray[2] = D3DXVECTOR3(m_pos.x + 20.0f,m_pos.y,0.0f);
	pArray[3] = D3DXVECTOR3(m_pos.x,m_pos.y,0.0f);

	D3DXMATRIX R;
	D3DXVECTOR3 center(pArray[1].x,pArray[0].y,0.0f);
	D3DXMatrixTransformation(&R,0,0,0,&center,&D3DXQUATERNION(0,0,m_dir,0),0);
	for(int i = 0; i < 4; ++i)
	{
		D3DXVec3TransformCoord(pArray + i,pArray + i,&R);
	}*/
}

Astroid::Astroid(const DxPolygon& polygon, const D3DXVECTOR3& pos, float a) : m_Polygon(polygon), m_pos(pos), m_angle(a)
{
	//m_pCollisionRect = static_cast<Math::CRectangle*>(Math::CreateCollionPolygon(polygon.GetEdges()));
}
/*Astroid::Astroid(const Astroid& other) : m_Polygon(other.m_Polygon)
{
	m_angle = other.m_angle;
	m_pos = other.m_pos;

	m_pCollisionRect = new ::Math::CRectangle(*other.m_pCollisionRect);
}*/
Astroid::~Astroid()
{
	//delete m_pCollisionRect;
}

void Astroid::Update(double dt)
{
	m_angle += dt;
}

void Astroid::Render(IRenderer& renderer)
{
	//std::vector<D3DXVECTOR2> vert(m_vertex);

	/*D3DXMATRIX R;
	D3DXVECTOR3 center(m_pos.x,m_pos.y,0.0f);
	D3DXMatrixTransformation(&R,0,0,0,&center,&D3DXQUATERNION(0,0,this->m_anglet,0),0);
	
	for(int i = 0; i < m_Polygon.GetSize(); ++i)
	{
		D3DXVec3TransformCoord(&(m_Polygon)[i],&(m_Polygon)[i],&R);
	}*/

	m_Polygon.SetAngle(m_angle);
	m_Polygon.Render(renderer);

	//m_pCollisionRect->Render(renderer);
}

Astroids::Astroids(PluginManager& pm) : m_ship(D3DXVECTOR3(0.0f,0.0f,0.0f)), m_pCamera(nullptr)
{
	BuildAstroids();
	
	//RECT R;
	//GetWindowRect(R);

	//m_pQuadtree = new QuadTree(Math::FRECT(D3DXVECTOR2(R.left,R.top),D3DXVECTOR2(R.right,R.bottom)));
	//m_pQuadtree->Insert(
}

Astroids::~Astroids()
{
	m_pCamera->Release();
}

void Astroids::BuildAstroids()
{
	RECT R;
	GetClientRect(::GetActiveWindow(),&R);

	for(unsigned int n = 0; n < 15; ++n)
	{
		float r = Math::GetRandFloat(0.1f,0.2f);
		float x = Math::GetRandFloat(-1.0f,1.0f);
		float y = Math::GetRandFloat(-1.0f,1.0f);

		unsigned int size = 5 + rand() % 7;
		float a = 0.0f; 
		float b = 360.0f / size;

		std::vector<D3DXVECTOR3> pos;
		pos.resize(size+1);
		
		for(int i = 0; i < size; ++i)
		{
			float angle = Math::GetRandFloat(a,b);
			pos[i] = D3DXVECTOR3(r * sin(angle * 0.01745f) + x + r,r * cos(angle * 0.01745f) + y + r,0.0f);

			a += 360.0f / size;
			b += 360.0f / size;
		}

		pos[size] = pos[0];
		
		m_Astroids.push_back(Astroid(DxPolygon(pos),D3DXVECTOR3(x,y,0.0f),Math::GetRandFloat(-1.0f,2.0f)));
	}
}

void Astroids::Init(Game& game)
{
	game.GetRenderer().SetCamera(m_pCamera);
}
void Astroids::Destroy(Game& game)
{
}

void Astroids::Update(class Game& game)
{
	this->m_ship.Update(game.GetInput(),game.GetDt());

	bool collide = false;

	for(auto iter = m_Astroids.begin(); iter != m_Astroids.end(); ++iter)
	{
		iter->Update(game.GetDt());

		if(m_ship.m_pPolygon->Overlaps(iter->m_Polygon))
		{
			collide = true;
		}
	}

	if(collide)
	{
		m_ship.m_pPolygon->SetColor(0xffff0000);
	}
	else
	{
		m_ship.m_pPolygon->SetColor(0xffffffff);
	}

	if(game.GetInput().KeyDown(KeyCode::F))
	{
		game.GetRenderer().ToggleFullscreen();
	}

	/*static ::D3DXVECTOR3 pos(0.0f,0.0f,-10.0f);
	::D3DXVECTOR3 target(0.0f,0.0f,0.0f);
	::D3DXVECTOR3 up(0.0f,1.0f,0.0f);

	if(game.GetInput().MouseClick(0,false))
	{
		pos.z += game.GetInput().MouseY()*0.3f;

		m_pCamera->lookAt(pos,target,up);

		m_pCamera->update(0.0f);

	}*/

	

	//char* pBytes = new char[64];
	/*char* pBytes = (char*)::Bnew(64);
	memset(pBytes,0,64);
	sprintf(pBytes,"Hello World");

	//delete[] pBytes;
	BDelete(pBytes);*/

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

	POINT P = {0,0};

	std::stringstream buffer;
	buffer << "Time: "<<this->m_timer.GetTime()<<std::endl;

	//game.GetRenderer().Get2DRenderer().DrawString(buffer.str().c_str(),P,0xffffffff);
	//game.GetRenderer().Get2DRenderer().DrawString("Astroids",game.GetInput().MousePos(),0xffffffff);
}
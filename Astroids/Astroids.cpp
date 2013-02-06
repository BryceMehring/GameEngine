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
	if(pos.x < -50.0f)
	{
		pos.x = 50.0f;
	}
	else if(pos.x > 50.0f)
	{
		pos.x = -50.0f;
	}

	if(pos.y < -50.0f)
	{
		pos.y = 50.0f;
	}
	else if(pos.y > 50.0f)
	{
		pos.y = -50.0f;
	}
}

void ClampWrap(D3DXVECTOR2& pos)
{
	if(pos.x < -50.0f)
	{
		pos.x = 50.0f;
	}
	else if(pos.x > 50.0f)
	{
		pos.x = -50.0f;
	}

	if(pos.y < -50.0f)
	{
		pos.y = 50.0f;
	}
	else if(pos.y > 50.0f)
	{
		pos.y = -50.0f;
	}
}

ASpaceShipBeam::ASpaceShipBeam(const D3DXVECTOR3& pos, float dir, float speed) :
m_pos(pos), m_dir(dir), m_speed(speed), m_age(0), m_collisionPoly(Math::Circle(D3DXVECTOR2(pos.x,pos.y),1.0f)), m_bDestroyed(false)
{
}

// ISpatialObject
D3DXVECTOR2 ASpaceShipBeam::GetPos() const
{
	return D3DXVECTOR2(m_pos.x,m_pos.y);
}
D3DXVECTOR2 ASpaceShipBeam::GetDir() const
{
	return D3DXVECTOR2(0.0f,0.0f);
}
float ASpaceShipBeam::GetSpeed() const
{
	return m_speed;
}

void* ASpaceShipBeam::QueryInterface(unsigned int i) const
{
	if(i == INTERFACE_BEAM)
		return (void*)this;

	if(i == IDestroyable::INTERFACE_DESTROY)
		return (void*)((IDestroyable*)this);

	return nullptr;
}

const Math::ICollisionPolygon& ASpaceShipBeam::GetCollisionPolygon() const
{
	return m_collisionPoly;
}

// IDestroyable
void ASpaceShipBeam::Destroy()
{
	m_bDestroyed = true;
}

bool ASpaceShipBeam::Update(QuadTree& tree, double dt)
{
	D3DXVECTOR3 dir(sin(m_dir),cos(m_dir),0.0f);
	m_pos += dir * m_speed * dt;
	ClampWrap(m_pos);
	m_age += dt;

	tree.Erase(*this);
	m_collisionPoly.GetCircle() = D3DXVECTOR2(m_pos.x,m_pos.y);
	tree.Insert(*this);

	std::vector<ISpatialObject*> nearObj;
	tree.QueryNearObjects(GetCollisionPolygon(),nearObj);

	for(unsigned int i = 0; i < nearObj.size(); ++i)
	{
		if(nearObj[i] != this)
		{
			void* pInterface = nearObj[i]->QueryInterface(IDestroyable::INTERFACE_DESTROY);
			void* pShipInterface = nearObj[i]->QueryInterface(ASpaceShip::INTERFACE_SHIP);

			if(pInterface != nullptr && pShipInterface == nullptr)
			{
				IDestroyable* pDestroyable = static_cast<IDestroyable*>(pInterface);
				pDestroyable->Destroy();
				//Destroy();
			}
		}
	}

	return m_bDestroyed || m_age > .7f;
}

void ASpaceShipBeam::Render(IRenderer& renderer)
{
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,m_pos.z);
	renderer.Get2DRenderer().DrawSprite(T,"ball");
}

DWORD WINAPI PlayGameSound(void* p)
{
	PlaySound((char*)p,0,SND_FILENAME);
	return 0;
}


ASpaceShip::ASpaceShip(const D3DXVECTOR3& pos) : m_pos(pos), m_dir(0.0f), m_acceleration(1000.0f), m_inputTime(0.0), m_bDestroyed(false)
{
	m_Polygon.resize(4);

	m_Polygon[0] = D3DXVECTOR3(-3.0f,0.0f,0.0f);
	m_Polygon[1] = D3DXVECTOR3(0.0f,10.0f,0.0f);
	m_Polygon[2] = D3DXVECTOR3(3.0f,0.0f,0.0f);
	m_Polygon[3] = D3DXVECTOR3(-3.0f,0.0f,0.0f);
}

ASpaceShip::~ASpaceShip()
{
}

bool ASpaceShip::Update(IKMInput& input, QuadTree& tree, double dt)
{
	if(input.KeyDown(KeyCode::W,false) || input.KeyDown(KeyCode::UP,false))
	{
		m_acceleration += 500.0f * dt;
	}

	if(input.KeyDown(KeyCode::A,false) || input.KeyDown(KeyCode::LEFT,false))
	{
		m_dir -= 2.0f*dt;
	}

	if(input.KeyDown(KeyCode::D,false) || input.KeyDown(KeyCode::RIGHT,false))
	{
		m_dir += 2.0f*dt;
	}
	
	//m_dir += 5.0f*input.MouseX() * dt;
	//m_pos.x += 50.0f * input.MouseX() * dt;
	//m_pos.y += 50.0f * input.MouseY() * dt;


	/*const D3DXVECTOR2& mousePos = input.GetTransformedMousePos();
	D3DXVECTOR2 dir = mousePos - D3DXVECTOR2(m_pos.x,m_pos.y);

	m_dir = atan2(dir.x,dir.y);*/

	m_inputTime += dt;
	if(input.KeyDown(KeyCode::SPACE,false) && m_inputTime > 0.3f)
	{
		D3DXVECTOR3 shipDir(10.0f*sin(m_dir),10.0f*cos(m_dir),0.0f);
		D3DXVECTOR3 pos = m_pos + shipDir;
		m_beams.push_back(ASpaceShipBeam(pos,m_dir,m_acceleration + 50.0f));

		m_soundHandles.push_back(CreateThread(0,0,PlayGameSound,"..\\sounds\\fire.wav",0,0));
		//m_soundHandles.push_back(CreateThread(0,0,PlayGameSound,"..\\sounds\\explode1.wav",0,0));

		tree.Insert(m_beams.back());

		m_inputTime = 0.0;
		
	}

	for(auto iter = m_soundHandles.begin(); iter != m_soundHandles.end(); )
	{
		if(WaitForSingleObject(*iter,0) == WAIT_OBJECT_0)
		{
			CloseHandle(*iter);
			iter = m_soundHandles.erase(iter);
		}
		else
		{
			 ++iter;
		}
	}

	for(auto iter = m_beams.begin(); iter != m_beams.end(); )
	{
		bool destroyed = iter->Update(tree,dt);
		if(destroyed)
		{
			tree.Erase(*iter);
			iter = m_beams.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	D3DXVECTOR3 shipDir(sin(m_dir),cos(m_dir),0.0f);

	m_pos += shipDir * m_acceleration * dt;
	m_acceleration -= 200*dt;
	
	m_acceleration = Math::Clamp(m_acceleration,0.0f,700.0f);

	ClampWrap(m_pos);

	if(abs(m_dir) > 6.283f)
	{
		m_dir = 0.0f;
	}

	::D3DXMATRIX R, T;
	::D3DXMatrixRotationZ(&R,-m_dir);
	::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,m_pos.z);

	m_transformation = R * T;

	std::vector<::D3DXVECTOR3> verts(m_Polygon);
	D3DXVec3TransformCoordArray(&verts.front(),sizeof(D3DXVECTOR3),&verts.front(),sizeof(D3DXVECTOR3),&m_transformation,verts.size());

	Math::FRECT Rect;
	Math::CreateCollionPolygon(verts,Rect);

	tree.Erase(*this);
	m_CollisionRect = Rect;
	tree.Insert(*this);

	/*std::vector<::ISpatialObject*> nearObjs;
	tree.FindNearObjects(&this->GetCollisionPolygon(),nearObjs);

	if(nearObjs.size() >= 2)
	{
		Destroy();
	}*/


	return false;
}

void ASpaceShip::Render(IRenderer& renderer)
{
	I2DRenderer& renderer2d = renderer.Get2DRenderer();
	::D3DXVECTOR4 color(1.0f,1.0f,1.0f,1.0f);

	renderer2d.DrawLine(&m_Polygon.front(),m_Polygon.size(),color,m_transformation);

	for(auto iter = m_beams.begin(); iter != m_beams.end(); ++iter)
	{
		iter->Render(renderer);
	}
}

void ASpaceShip::BuildShipLines(D3DXVECTOR3* pArray)
{

}

Astroid::Astroid(const std::vector<D3DXVECTOR3>& polygon, const D3DXVECTOR3& pos, const D3DXVECTOR2& dir, float a) : 
m_Polygon(polygon), m_pos(pos), m_dir(dir), m_angle(a), m_color(1.0f,1.0f,1.0f,1.0f), m_iHealth(1)
{
	UpdateCollisionPolygon();
}
Astroid::~Astroid()
{
	//delete m_pCollisionRect;
}

bool Astroid::Update(QuadTree& tree, double dt)
{
	// update pos and angle
	m_angle += dt;
	m_pos += m_dir * dt * 5; 

	ClampWrap(m_pos);

	// build matrices
	D3DXMATRIX R, T;
	D3DXMatrixRotationZ(&R,m_angle);
	D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);

	m_Transformation = R * T;

	tree.Erase(*this);
	UpdateCollisionPolygon();
	tree.Insert(*this);

	return (m_iHealth < 0);
}

void Astroid::UpdateCollisionPolygon()
{
	// build collision polygon
	std::vector<::D3DXVECTOR3> verts(m_Polygon);
	D3DXVec3TransformCoordArray(&verts.front(),sizeof(D3DXVECTOR3),&verts.front(),sizeof(D3DXVECTOR3),&m_Transformation,verts.size());

	Math::FRECT Rect;
	Math::CreateCollionPolygon(verts,Rect);

	m_CollisionRect = Rect;
}

void Astroid::Render(IRenderer& renderer)
{
	I2DRenderer& renderer2D = renderer.Get2DRenderer();

	// render asteroid
	renderer2D.DrawLine(&(m_Polygon.front()),m_Polygon.size(),m_color,m_Transformation);
}

Astroids::Astroids(PluginManager& pm) : m_ship(D3DXVECTOR3(-40.0f,40.0f,0.0f)), m_iScore(0)
{
	m_pQuadtree = new QuadTree(Math::FRECT(D3DXVECTOR2(-50.0f,50.0f),D3DXVECTOR2(50.0f,-50.0f)));
	m_pQuadtree->Insert(m_ship);

	BuildAstroids();
}

Astroids::~Astroids()
{
	delete m_pQuadtree;
}

void Astroids::BuildAstroidsAt(const ::D3DXVECTOR2& pos)
{
	unsigned int size = 5 + rand() % 10;
	float r = Math::GetRandFloat(3.0f,5.0f);
	float a = 0.0f; 
	float b = 360.0f / size;

	std::vector<D3DXVECTOR3> lines;
	lines.resize(size+1);
		
	for(int i = 0; i < size; ++i)
	{
		float angle = Math::GetRandFloat(a,b);
		lines[i] = D3DXVECTOR3(r * sin(angle * 0.01745f),r * cos(angle * 0.01745f),0.0f);

		a += 360.0f / size;
		b += 360.0f / size;
	}

	lines[size] = lines[0];

	float angle = Math::GetRandFloat(0.0f,2.0f*D3DX_PI);

	D3DXVECTOR2 dir(cos(angle),sin(angle));

	m_Astroids.push_back(Astroid(lines,D3DXVECTOR3(pos.x,pos.y,0.0f),dir,Math::GetRandFloat(-1.0f,2.0f)));
	m_pQuadtree->Insert(m_Astroids.back());
}

void Astroids::BuildAstroids()
{
	// this algorithm should build asteroids in local space rather than in world space
	for(unsigned int n = 0; n < 10; ++n)
	{
		BuildAstroidsAt(D3DXVECTOR2(Math::GetRandFloat(-40.0f,40.0f),Math::GetRandFloat(-40.0f,40.0f)));
	}
}

/*void Astroids::Init(Game& game)
{
}
void Astroids::Destroy(Game& game)
{
}*/

void Astroids::Update(Game& game)
{
	IKMInput& input = game.GetInput();

	if(input.MouseClick(0))
	{
		BuildAstroidsAt(input.GetTransformedMousePos());
	}

	for(auto iter = m_Astroids.begin(); iter != m_Astroids.end(); )
	{
		bool bDestroy = iter->Update(*m_pQuadtree,game.GetDt());

		if(bDestroy)
		{
			m_pQuadtree->Erase(*iter);
			iter = m_Astroids.erase(iter);
			++m_iScore;
		}
		else
		{
			++iter;
		}
	}
	m_ship.Update(game.GetInput(),*m_pQuadtree,game.GetDt());

	/*Math::FRECT R;
	if(game.GetInput().GetSelectedRect(R))
	{
		for(unsigned int i = 0; i < m_selectedAstroids.size(); ++i)
		{
			m_selectedAstroids[i]->Deselect();
		}

		m_selectedAstroids.clear();

		Math::CRectangle cRect = R;

		std::vector<::ISpatialObject*> objs;
		m_pQuadtree->FindNearObjects(&cRect,objs);

		for(unsigned int i = 0; i < objs.size(); ++i)
		{
			void* pInterface = objs[i]->QueryInterface(Astroid::INTERFACE_ASTROID);

			if(pInterface != nullptr)
			{
				Astroid* pAstroid = static_cast<Astroid*>(pInterface);
				
				pAstroid->Select();
				m_selectedAstroids.push_back(pAstroid);
			}
		}
	}*/

	/*for(auto iter = m_selectedAstroids.begin(); iter != m_selectedAstroids.end(); ++iter)
	{
		Astroid* pAstroid = (Astroid*)*iter;

		pAstroid->Update(*m_pQuadtree,game.GetDt());
	}*/

	if(game.GetInput().KeyDown(KeyCode::F))
	{
		game.GetRenderer().ToggleFullscreen();
	}
}
void Astroids::Draw(Game& game)
{
	IRenderer& renderer = game.GetRenderer();

	m_ship.Render(game.GetRenderer());
	for(auto iter = m_Astroids.begin(); iter != m_Astroids.end(); ++iter)
	{
		iter->Render(game.GetRenderer());
	}

	std::ostringstream stream;
	stream << "Asteroids Destroyed: "<<m_iScore << endl;
	stream << "Time played: "<<m_timer.GetTime()<<endl;

	D3DXVECTOR2 pos(0.0f,45.0f);
	Math::FRECT R;

	//renderer.Get2DRenderer().GetStringRec(stream.str().c_str(),pos,D3DXVECTOR2(1.0f,1.5f),R);

	pos.x -= R.Width() / 2.0f;

	//renderer.Get2DRenderer().DrawString(stream.str().c_str(),pos,"segoe",::D3DXVECTOR4(0.0f,1.0f,0.0f,1.0f),::D3DXVECTOR2(1.0f,1.5f));


	stream.clear();
	stream << m_Astroids.size() << endl;
	renderer.Get2DRenderer().DrawString(stream.str().c_str(),D3DXVECTOR2(0.0f,20.0f));
	m_pQuadtree->Render(game.GetRenderer());
}
#pragma once

#define PLUGIN_EXPORTS
#include "PluginManager.h"
#include "IKMInput.h"
#include "GameStateScript.h"
#include "RTTI.h"
#include "QuadTree.h"
#include "Camera.h"

class IDestroyable
{
public:

	virtual ~IDestroyable() {}

	virtual void Destroy() = 0;

	static const unsigned int INTERFACE_DESTROY = 924490;
};

class ASpaceShipBeam : public ISpatialObject, public IDestroyable
{
public:

	ASpaceShipBeam(const D3DXVECTOR3& pos, float dir, float speed);

	// ISpatialObject
	virtual D3DXVECTOR2 GetPos() const;
	virtual D3DXVECTOR2 GetDir() const;
	virtual float GetSpeed() const;

	virtual void* QueryInterface(unsigned int) const;

	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const;

	// IDestroyable
	virtual void Destroy();

	virtual bool Update(QuadTree& tree, double dt);
	virtual void Render(IRenderer& renderer);

	static const unsigned int INTERFACE_BEAM = 538356732;

private:

	D3DXVECTOR3 m_pos;
	Math::CCircle m_collisionPoly;
	float m_speed;
	float m_dir;
	float m_age;
	bool m_bDestroyed;
};

class ASpaceShip : public ISpatialObject, public IDestroyable
{
public:

	ASpaceShip(const D3DXVECTOR3& pos);

	virtual ~ASpaceShip();

	// ISpatialObject
	virtual D3DXVECTOR2 GetPos() const { return m_pos; }
	virtual D3DXVECTOR2 GetDir() const { return D3DXVECTOR2(cos(m_dir),sin(m_dir)); }
	virtual float GetSpeed() const { return 0.0f; }
	virtual void* QueryInterface(unsigned int i) const
	{ 
		if(i == INTERFACE_SHIP)
			return (void*)this;

		if(i == IDestroyable::INTERFACE_DESTROY)
			return (void*)((IDestroyable*)this);

		return nullptr;
	}
	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const { return m_CollisionRect; }

	virtual bool Update(IKMInput& input, QuadTree& tree, double dt);
	virtual void Render(IRenderer& renderer);

	virtual void Destroy()
	{
		m_pos = ::D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_acceleration = 0.0f;
	}

	static const unsigned int INTERFACE_SHIP = 924493;

private:

	std::vector<D3DXVECTOR3> m_Polygon;
	
	D3DXVECTOR3 m_pos;
	Math::CRectangle m_CollisionRect;
	D3DXMATRIX m_transformation;
	float m_acceleration;
	float m_dir;
	double m_inputTime;
	bool m_bDestroyed;

	std::list<ASpaceShipBeam> m_beams;
	std::list<HANDLE> m_soundHandles;

	void BuildShipLines(D3DXVECTOR3*);
};
//CRectangle
class Astroid : public ISpatialObject, public IDestroyable
{
public:

	Astroid(const std::vector<D3DXVECTOR3>& polygon, const D3DXVECTOR3& pos, const D3DXVECTOR2& dir , float a);
	~Astroid();

	// ISpatialObject
	virtual D3DXVECTOR2 GetPos() const { return m_pos; }
	virtual D3DXVECTOR2 GetDir() const { return D3DXVECTOR2(0.0f,0.0f); }
	virtual float GetSpeed() const { return 0.0f; }
	virtual void* QueryInterface(unsigned int i) const
	{
		if(i == INTERFACE_ASTROID)
			return (void*)this;

		if(i == IDestroyable::INTERFACE_DESTROY)
			return (void*)((IDestroyable*)this);

		return nullptr;
	}
	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const { return m_CollisionRect; }

	virtual bool CanSendCollisionMsg() const { return false; }
	virtual void SendCollisionMsg(const ISpatialObject& other)
	{
		void* pInterface = other.QueryInterface(IDestroyable::INTERFACE_DESTROY);

		if(pInterface != nullptr)
		{
			IDestroyable* pDestroyable = static_cast<IDestroyable*>(pInterface);
			pDestroyable->Destroy();
		}
	}


	// Asteroid
	bool Update(QuadTree&, double dt);
	void Render(IRenderer& renderer);

	virtual void Destroy()
	{
		m_iHealth -= 1;
		m_color = ::D3DXVECTOR4(1.0f,0.0f,0.0f,1.0f);
	}

	static const unsigned int INTERFACE_ASTROID = 874065941;

private:

	float m_angle;
	D3DXVECTOR2 m_dir;
	D3DXVECTOR2 m_pos;
	D3DXVECTOR4 m_color;
	D3DXMATRIX m_Transformation;
	std::vector<D3DXVECTOR3> m_Polygon;
	Math::CRectangle m_CollisionRect;
	int m_iHealth;

	void UpdateCollisionPolygon();
};

class Astroids : public GameStateScript//, public SerializedState
{
public:

	RTTI_DECL;

	// returns the name of the plug-in
	virtual DLLType GetPluginType() const { return DLLType::GamePlugin; }
	virtual const char* GetName() const { return "Astroids"; }
	virtual int GetVersion() const { return 0; }

	// displays a info box about the plug-in
	virtual void About() const {}

	/*virtual void Init(class Game& game);
	virtual void Destroy(class Game& game);*/

	virtual void Update(class Game&);
	virtual void Draw(class Game&);
	//virtual const class RTTI* GetType() const;

private:

	Astroids(PluginManager&);
	virtual ~Astroids();

	void BuildAstroids();
	void BuildAstroidsAt(const D3DXVECTOR2& pos);

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager&); 

	ASpaceShip m_ship;
	std::vector<::Astroid*> m_selectedAstroids;
	std::list<Astroid> m_Astroids;
	QuadTree* m_pQuadtree;
	int m_iScore;

};
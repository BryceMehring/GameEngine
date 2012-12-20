#pragma once

#define PLUGIN_EXPORTS
#include "PluginManager.h"
#include "IKMInput.h"
#include "GameStateScript.h"
#include "RTTI.h"
#include "QuadTree.h"
#include "Camera.h"

class ASpaceShip
{
public:

	ASpaceShip(const D3DXVECTOR3& pos);

	virtual ~ASpaceShip();

	virtual void Update(IKMInput& input, double dt);

	virtual void Render(IRenderer& renderer);

	float GetDir() const { return m_dir; }

	DxPolygon* m_pPolygon;

private:

	struct Beam
	{
		Beam() {}
		Beam(const D3DXVECTOR3& pos, float dir, float speed) : pos(pos), dir(dir), speed(speed), age(0) {}

		D3DXVECTOR3 pos;
		float speed;
		float dir;
		float age;
	};

	
	D3DXVECTOR3 m_pos;
	float m_acceleration;
	float m_dir;
	double m_inputTime;

	std::list<Beam> m_beams;

	void BuildShipLines(D3DXVECTOR3*);
};

class Astroid
{
public:

	Astroid(const DxPolygon& polygon, const D3DXVECTOR3& pos, float a);
	//Astroid(const Astroid& other);
	~Astroid();

	void Update(double dt);
	void Render(IRenderer& renderer);

	DxPolygon m_Polygon;

private:

	float m_angle;
	float m_anglet;
	D3DXVECTOR2 m_pos;
	::Math::CRectangle* m_pCollisionRect;
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

	virtual void Init(class Game& game);
	virtual void Destroy(class Game& game);

	virtual void Update(class Game&);
	virtual void Draw(class Game&);
	//virtual const class RTTI* GetType() const;

private:

	Astroids(PluginManager&);
	virtual ~Astroids();

	void BuildAstroids();

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager&); 

	ASpaceShip m_ship;
	std::list<Astroid> m_Astroids;
	QuadTree* m_pQuadtree;

	Camera* m_pCamera;

};
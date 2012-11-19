#pragma once

#define PLUGIN_EXPORTS
#include "PluginManager.h"
#include "IKMInput.h"
#include "GameStateScript.h"
#include "RTTI.h"
#include "QuadTree.h"

class ASpaceShip
{
public:

	ASpaceShip(const D3DXVECTOR2& pos);

	virtual ~ASpaceShip();

	virtual void Update(IKMInput& input, double dt);

	virtual void Render(IRenderer& renderer);

	float GetDir() const { return m_dir; }

private:

	struct Beam
	{
		Beam() {}
		Beam(const D3DXVECTOR2& pos, float dir) : pos(pos), dir(dir), age(0) {}

		D3DXVECTOR2 pos;
		float dir;
		float age;
	};

	DxPolygon* m_pPolygon;
	D3DXVECTOR2 m_pos;
	float m_acceleration;
	float m_dir;

	std::list<Beam> m_beams;

	void BuildShipLines(D3DXVECTOR2*);
};

class Astroid
{
public:

	Astroid(const DxPolygon& polygon, const D3DXVECTOR2& pos, float a);
	~Astroid();

	void Update(double dt);
	void Render(IRenderer& renderer);

private:

	DxPolygon m_Polygon;
	float m_angle;
	float m_anglet;
	D3DXVECTOR2 m_pos;
};

class Astroids : public GameStateScript//, public SerializedState
{
public:

	RTTI_DECL;

	Astroids(PluginManager&);
	virtual ~Astroids();

	// returns the name of the plug-in
	virtual DLLType GetPluginType() const { return DLLType::GamePlugin; }
	virtual const char* GetName() const { return "Astroids"; }
	virtual int GetVersion() const { return 0; }

	// displays a info box about the plug-in
	virtual void About() const {}

	virtual void Update(class Game&);
	virtual void Draw(class Game&);
	//virtual const class RTTI* GetType() const;

private:

	ASpaceShip m_ship;
	std::list<Astroid> m_Astroids;
	QuadTree* m_pQuadtree;

	void BuildAstroids();

};
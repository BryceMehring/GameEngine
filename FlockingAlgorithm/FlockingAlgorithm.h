#ifndef _FLOCKINGALGORITHM_
#define _FLOCKINGALGORITHM_

#define PLUGIN_EXPORTS
#include "PluginManager.h"
#include "IGameState.h"
#include "RTTI.h"
#include "QuadTree.h"
#include "Menu.h"

class Bird : public ISpatialObject
{
public:

	Bird();
	Bird(const glm::vec2& pos);

	virtual glm::vec2 GetPos() const { return glm::vec2(m_pos.x,m_pos.y); }
	virtual glm::vec2 GetDir() const { return glm::vec2(-sin(m_fAngle),cos(m_fAngle)); }
	virtual float GetSpeed() const { return 0; }

	virtual void* QueryInterface(unsigned int) const { return nullptr; }

	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const { return m_CollisionCircle; }

	void Update(QuadTree&, const glm::vec2& avgPos, float v, double dt);
	void Render(IRenderer& renderer);

private:

	glm::vec3 m_pos;
	glm::vec4 m_color;
	float m_fAngle;
	Math::CCircle m_CollisionCircle;

};

class FlockingAlgorithm : public IGameState
{
public:

	RTTI_DECL;

	FlockingAlgorithm();

	virtual DLLType GetPluginType() const { return GamePlugin; }
	virtual const char* GetName() const { return "FlockingAlgorithm"; }
	virtual int GetVersion() const { return 0; }

	// displays a info box about the plug-in
	virtual void About() const {}

	// IGameState
	virtual void Init(class Game& game);
	virtual void Destroy(class Game& game);
	virtual void Update(class Game& game);
	virtual void Draw(class Game& game);

	virtual void Init(class asIScriptEngine*) {}
	virtual void Destroy(class asIScriptEngine*) {}

	void ClearBirds();
	void SliderCallback(float);
	void ProgressBarCallback();
	void ToggleQuadtree();

private:

	std::vector<Bird*> m_birds;

	QuadTree* m_pQuadtree;

	UI::GUI m_gui;
	UI::ProgressBar* m_pProgressBar;

	float m_fSliderValue;
	double m_Time;
	bool m_bEnableQuadtree;

	void BuildUI(Game&);


};

#endif // _FLOCKINGALGORITHM_

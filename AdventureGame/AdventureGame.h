#ifndef _ADVENTUREGAME_
#define _ADVENTUREGAME_

#include "IGameState.h"
#include "RTTI.h"
#include "VecMath.h"
#include "Camera.h"

enum class Direction
{
	NONE,
	NORTH,
	SOUTH,
	EAST,
	WEST
};

class Player
{
public:

	Player(Camera*);

	Direction Update(class IKMInput&,double);
	void Render(class IRenderer&);

private:

	glm::vec2 m_pos;
	int m_iCell;
	Camera* m_pCamera;

};

class AdventureGame : public IGameState
{
public:

	RTTI_DECL;

	AdventureGame();

	virtual DLLType GetPluginType() const { return DLLType::Game; }
	virtual const char* GetName() const { return "AdventureGame"; }
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
private:

	Player* m_pPlayer;
	glm::ivec2 m_pos;
};

#endif // _ADVENTUREGAME_
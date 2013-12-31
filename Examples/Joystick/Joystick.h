#ifndef _JOYSTICK_
#define _JOYSTICK_

#include "IGameState.h"
#include "../common/Camera.h"

class Joystick : public IGameState
{
public:

	Joystick();

	// IPlugin

	// returns the type of the plug-in
	virtual DLLType GetPluginType() const { return DLLType::Game; }

	// returns the name of the plug-in
	virtual const char* GetName() const { return "Joystick"; }

	// returns the version number of the plugin
	virtual int GetVersion() const { return 0; }

	virtual void Init(class asIScriptEngine*) {}
	virtual void Destroy(class asIScriptEngine*) {}

	// IGameState

	// Called only once when the plugin is created
	virtual void Init(class Game& game);

	// Called only once when the plugin is destroyed
	virtual void Destroy(class Game& game);

	// Called every frame to update the state of the game
	virtual void Update(class Game& game);

	// Called every frame to render the game
	virtual void Draw(class Game& game);

private:

	Camera m_camera;
	glm::vec2 m_posLS;
	glm::vec2 m_posRS;
};

#endif // _JOYSTICK_
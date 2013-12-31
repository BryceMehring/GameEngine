// Programmed By Bryce Mehring

#ifndef _GAME_
#define _GAME_

#include "PluginManager.h"
#include "GameStateMachine.h"
#include "GLFWInit.h"
#include "interfaces/IInput.h"
#include "interfaces/IRenderer.h"
#include "asVM.h"
#include <string>

#ifdef _WIN32
#ifdef GAME_ENGINE_EXPORT
#ifndef GAME_ENGINE_API
#define GAME_ENGINE_API __declspec(dllexport)
#endif
#else
#ifndef GAME_ENGINE_API
#define GAME_ENGINE_API __declspec(dllimport)
#endif
#endif
#else
#define GAME_ENGINE_API
#endif

// The Game class holds the main game loop and manages the successful creation of all the plugins
class Game
{
public:

	// Init GLFW, load dlls, load the base resource file
	GAME_ENGINE_API Game();
	GAME_ENGINE_API ~Game();

	// Get the current state
	GAME_ENGINE_API std::string GetCurrentStateName() const;

	// Change the current state
	GAME_ENGINE_API void SetNextState(const std::string& state);

	// Quit the game during the beginning of the next frame
	GAME_ENGINE_API void Quit() const;

	// Reload component plugins
	// todo: should add a feature that allows the user to select which plugin is loaded
	GAME_ENGINE_API void LoadPlugins();

	// Get Functions
	// todo: provide const overloaded versions?
	GAME_ENGINE_API IRenderer& GetRenderer();
	GAME_ENGINE_API IInput& GetInput();
	GAME_ENGINE_API PluginManager& GetPM();
	GAME_ENGINE_API asVM& GetAs();

	// time differential between frames in seconds
	GAME_ENGINE_API double GetDt() const;

private:

	GLFWInit m_glfwInit;

	asVM m_vm;
	PluginManager m_plugins;

	GameStateMachine m_StateMachine;

	double m_fDT;
	double m_fTimeElapsed;
	unsigned int m_uiFrameCounter;
	unsigned int m_uiFPS;

	// Component Interfaces
	IRenderer* m_pRenderer;
	IInput* m_pInput;

	std::string m_NextState;

	bool m_bDrawFPS;

private:

	// play the game set by SetNextState
	// this method should only be called once in the GameLauncher
	GAME_ENGINE_API int Run();
	friend int main(int n, char**);

	void Update();
	void UpdateFPS();

	void Draw();
	void DrawFPS();

	void RegisterScript();

	// Prevent copying
	Game(const Game&) = delete;
	Game& operator =(const Game&) = delete;

};

#endif //_GAME_

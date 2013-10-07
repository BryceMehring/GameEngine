// Programmed By Bryce Mehring

#ifndef _GAME_
#define _GAME_

#define GLFW_DLL

#include "PluginManager.h"
#include "GameStateMachine.h"
#include "GLFWInit.h"
#include "interfaces/IKMInput.h"
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

class GameInfo
{
public:

	GameInfo();
	void Update(double dt);

	unsigned int GetFPS() const;

private:

	void UpdateFPS(double dt);

	double m_fTimeElapsed;
	unsigned int m_uiFrames;
	unsigned int m_uiFPS;

};


// The Game class holds the main game loop and manages the successful creation of all the plugins
class Game
{
public:

	// Init GLFW, load dlls, load the base resource file
	GAME_ENGINE_API Game();
	GAME_ENGINE_API ~Game();

	// Get the current state
	GAME_ENGINE_API std::string GetCurrentStateName() const;

	// change the current state
	GAME_ENGINE_API void SetNextState(const std::string& state);

	// ignore, working on this
	GAME_ENGINE_API void LoadPlugins();

	// Get Functions
	// todo: provide const overloaded versions?
	GAME_ENGINE_API IRenderer& GetRenderer();
	GAME_ENGINE_API IKMInput& GetInput();
	GAME_ENGINE_API PluginManager& GetPM();
	GAME_ENGINE_API asVM& GetAs();

	// time diff between frames in seconds
	GAME_ENGINE_API double GetDt() const;
	GAME_ENGINE_API unsigned int GetFps() const;

	// play the game set by SetNextState
	// this method should only be called once in the GameLauncher
	GAME_ENGINE_API int Run();

private:

	GLFWInit m_glfwInit;

	// Data members
	asVM m_vm;
	PluginManager m_plugins;

	class ScriptingConsole* m_pConsole; // ignore right now, working on this
	bool m_bConsoleEnabled;/// same as above

	GameStateMachine m_StateMachine;

	double m_fDT;
	GameInfo m_info;

	// Component Interfaces
	IRenderer* m_pRenderer;
	IKMInput* m_pInput;

private:

	std::string m_NextState;

	// helper functions

	void RegisterScript();

	void Update();
	void UpdateFPS();

	void Draw();
	void DrawSelectionRect();

	// Prevent copying
	Game(const Game&);
	Game& operator =(const Game&);

};

#endif //_GAME_

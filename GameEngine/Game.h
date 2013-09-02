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
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
#else
#define EXPORT
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

class Game
{
public:

	// Init GLFW, load dlls, load the base resource file
	EXPORT Game();

	// Destroy everything
	EXPORT ~Game();

	// Get the current state
	EXPORT std::string GetCurrentStateName() const;

	// change the current state
	EXPORT void SetNextState(const std::string& state);

	// ignore, working on this
	EXPORT void LoadPlugins();
	
	// Get Functions
	// todo: provide const overloaded versions
	EXPORT IRenderer& GetRenderer();
	EXPORT IKMInput& GetInput();
	EXPORT PluginManager& GetPM();
	EXPORT asVM& GetAs();

	EXPORT double GetDt() const; // time diff between frames in seconds
	EXPORT unsigned int GetFps() const;

	// play the game set by SetNextState
	EXPORT int Run();

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
	void DrawCursor();
	void DrawSelectionRect();
	void DrawFPS();


	// Prevent copying
	Game(const Game&);
	Game& operator =(const Game&);

};

#endif //_GAME_

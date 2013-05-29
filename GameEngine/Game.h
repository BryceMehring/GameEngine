// Programmed By Bryce Mehring

#ifndef _GAME_
#define _GAME_

#define GLFW_DLL

#include "PluginManager.h"
#include "Timer.h"
#include "GameStateMachine.h"
#include "IKMInput.h"
#include "IRenderer.h"
#include "asVM.h"
#include <string>

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
	Game();

	// Destroy everything
	~Game();

	// Get the current state
	const std::string& GetCurrentState() const;

	// change the current state
	void SetNextState(const std::string& state);

	// ignore, working on this
	void ReloadPlugins();
	
	// Get Functions
	// todo: provide const overloaded versions
	IRenderer& GetRenderer();
	IKMInput& GetInput();
	PluginManager& GetPM();
	asVM& GetAs();

	double GetDt() const; // time diff between frames in seconds
	unsigned int GetFps() const;

	// play the game set by SetNextState
	int Run();

protected:

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

	Timer m_timer;

	// helper functions
	void StartTimer();
	void EndTimer();

	void LoadAllDLL();

	void RegisterScript();

	void ReloadPlugins(const std::string& file);

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

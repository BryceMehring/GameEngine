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

	Game();
	~Game();

	const std::string& GetCurrentState() const;
	void SetNextState(const std::string& state);

	void ReloadPlugins();
	
	// Get Functions
	// todo: provide const overloaded versions
	IRenderer& GetRenderer();
	IKMInput& GetInput();
	PluginManager& GetPM();
	asVM& GetAs();

	// todo: I could just use this with handles instead of 
	// todo: having IPlugin interfaces lying around here 
	double GetDt() const;
	unsigned int GetFps() const;

	int Run();

protected:

	// Data members
	asVM m_vm;
	PluginManager m_plugins;

	class ScriptingConsole* m_pConsole;
	bool m_bConsoleEnabled;

	GameStateMachine m_StateMachine;

	double m_fDT;
	double m_fEscTime;
	GameInfo m_info;

	// Interfaces
	IRenderer* m_pRenderer;
	IKMInput* m_pInput;

private:

	::std::string m_NextState;

	Timer m_timer;

	int m_iEventId;

	// helper functions
	void StartTimer();
	void EndTimer();

	// helper functions
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

#endif _GAME_
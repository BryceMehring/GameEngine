// Programmed By Bryce Mehring

#ifndef _GAME_
#define _GAME_

#include "WindowManager.h"
#include "Timer.h"
#include "GameStateFactory.h"
#include "GameStateMachine.h"
#include "IKMInput.h"
#include "IRenderer.h"
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

	Game(HINSTANCE hInstance);
	virtual ~Game();

	void SetNextState(const std::string& name);
	
	void ReloadPlugins();
	void ReloadPluginsFromUserFolder();

	// Get Functions
	IRenderer* GetRenderer();
	IKMInput* GetInput();
	WindowManager* GetWindow();
	const PluginManager* GetPluginManager() const;
	double GetDt() const;
	unsigned int GetFps() const;

	virtual int PlayGame();

protected:

	// Data members
	WindowManager m_window;
	PluginManager m_plugins;

	GameStateMachine m_StateMachine;

	// todo: this is where I could add the asVM in the future...

	double m_fDT;
	GameInfo m_info;
	// Interfaces
	IRenderer* m_pRenderer;
	IKMInput* m_pInput;

private:

	IGameState* m_pNextState;

	Timer m_timer;

	int m_iEventId;

	// helper functions
	void StartTimer();
	void EndTimer();

	// helper functions
	void LoadAllDLL();

	void MsgProc(const struct MsgProcData& data);

	void ReloadPlugins(const std::string& file);

	void Update();
	void UpdateFPS();

	void Draw();
	void DrawFPS();

	// Prevent copying
	Game(const Game&);
	Game& operator =(const Game&);

};

#endif _GAME_
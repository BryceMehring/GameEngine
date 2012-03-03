
#ifndef _BENGINE_
#define _BENGINE_
#pragma once

//#include "asConsole.h"
//#include "asVM.h"
#include "PluginManager.h"
#include "Delegates.h"
#include "asVM.h"
#include "IKMInput.h"
#include "Timer.h"



// ************************
// This really should just manage everything, do not implement it here
// All of the different sub parts of the engine should be independent, they should be their own dll.
// Then this class would load the dll's dynamically and be the leader of the game without knowing its implementation details.
// ***********************

/*class IBEngine 
{
public:

	virtual ~IBEngine() {}

	virtual string OpenFileName() = 0;
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;

	virtual void Update(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual int Run() = 0;

	// todo: add more methods?
	
};

class IBEngineWin : public IBEngine
{
public:



};

class BEngineScriped : public IBEngine, public IScripted
{
public:

	BEngineScriped();
	virtual ~BEngineScriped();

protected:

	asVM* m_pVM;

};
*/
 
// this class should not be a singleton, I should create another class that manages 
// game engines
// todo: add RefCounting? I should name this class something else. 
//  Need to subclass these methods
// into more interfaces

struct MsgProcData
{
	UINT msg;
	WPARAM wParam;
	LPARAM lparam;
};

class IEvent
{
public:

	virtual ~IEvent() {}
	virtual void Notify(const MsgProcData&) const = 0;
};

// This class should be renamed to WindowManager

class WindowManager//, public RefCounting
{
public:
	
	// todo: I'm going to implement these for the msg process.
	// This should work out better because then I would not have to use the 
	// one fsm manager anymore.
	// should I split these up into more specific events?

	Event<void,const MsgProcData&> m_MsgProcEvent;

	WindowManager(HINSTANCE hInstance,const std::string& winCaption);
	virtual ~WindowManager();

	// todo: can these be private members?
	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual void MsgProc(UINT msg, WPARAM wPraram, LPARAM lparam);

	HINSTANCE GetHINSTANCE() const { return m_hInstance; }
	HWND GetWindowHandle() const { return m_hWindowHandle; }

	// todo: need to implement these functions
	virtual void RegisterScript();

	bool Update();

protected: //todo: I should make some of these members private

	// ====== data members ======

	// win32 window
	HWND m_hWindowHandle;
	HINSTANCE m_hInstance;

	RECT m_rect;

	bool m_bPaused;

protected:

	// ====== helper functions ======

	virtual void InitializeWindows(HINSTANCE hInstance, const std::string& winCaption);

private:

	// for win32 msg process
	static WindowManager* s_pThis;

	// Helper functions
	
	void Quit();

	// http://www.cplusplus.com/reference/iostream/ios/rdbuf/
	void RedirectIOToConsole(); // todo: try to phase out using a console window. Display to the win32 window or dump to a file: logging, try to use clog
};


#endif //_BENGINE_

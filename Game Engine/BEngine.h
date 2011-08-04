
#ifndef _BENGINE_
#define _BENGINE_
#pragma once

#include "asConsole.h"
#include "asVM.h"


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

};*/

 
// this class should not be a singleton, I should create another class that manages 
// game engines
// todo: add RefCounting? I should name this class something else. 
//  Need to subclass these methods
// into more interfaces

class BEngine : public IScripted//, public RefCounting
{
public:

	BEngine(HINSTANCE hInstance,const std::string& winCaption);
	virtual ~BEngine();

	// todo: can these be private members?
	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual void MsgProc(UINT msg, WPARAM wPraram, LPARAM lparam);

	std::string OpenFileName() const; 

	void GetStringRec(const char* str, RECT& out);
	HINSTANCE GetHINSTANCE() const { return m_hInstance; }
	HWND GetWindowHandle() const { return m_hWindowHandle; }

	// Interface Access
	asVM& GetScriptVM() const;
	IKMInput* GetInput() const; // todo: should I get rid of this? I could

	// Rendering
	void AddObjectToRenderList(IRender*);

	// todo: need to implement these functions
	virtual void RegisterScript();

	virtual int Run() = 0;

protected: //todo: I should make some of these members private

	// ====== data members ======
	// todo: ref counting?
	boost::scoped_ptr<asVM> m_vm;
	boost::scoped_ptr<UI> m_pUI;
	boost::scoped_ptr<asConsole> m_console; // todo: should we get rid of this? Or should asConsole manage the console overlay to the win32 window?
	boost::scoped_ptr<PluginManager> m_pm;

	// plugins
	IRenderingPlugin* m_pRenderer;
	std::list<IRender*> m_toRender;

	IKMInput* m_pInput;
	
	// win32 window
	HWND m_hWindowHandle;
	HINSTANCE m_hInstance;
	
	float m_fDT;
	float m_fStartCount;
	float m_fSecsPerCount;

	RECT m_rect;

	bool m_bPaused;

protected:

	// ====== helper functions ======

	void StartCounter();
	void EndCounter();

	bool Update();

	virtual void InitializePlugins();
	virtual void InitializeConsole();
	virtual void InitializeUI();
	virtual void InitializeWindows(HINSTANCE hInstance, const std::string& winCaption);

private:

	// for win32 msg process
	static BEngine* s_pThis;

	// Helper functions
	
	void Quit();

	// http://www.cplusplus.com/reference/iostream/ios/rdbuf/
	void RedirectIOToConsole(); // todo: try to phase out using a console window. Display to the win32 window or dump to a file: logging, try to use clog
	void InitializeTimer();
};


#endif //_BENGINE_




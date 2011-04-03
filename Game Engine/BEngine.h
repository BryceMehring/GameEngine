
#pragma once

#include "Interfaces.h"
#include "Singleton.h"
#include <Windows.h>

using namespace std;


// ************************
// This really should just manage everything, do not implement it here
// All of the different sub parts of the engine should be independent, they should be their own dll.
// Then this class would load the dll's dynamically and be the leader of the game without knowing its implementation details.
// ***********************

class IBaseEngine
{
public:

	static IBaseEngine* Instance();

	template< class T >
	static void Initialize(HINSTANCE hInstance,const string& winCaption)
	{
		// If there is no instance
		if(s_pInstance == NULL)
		{
			// create an instance
			new T(hInstance,winCaption);
		}
	}

	static void Delete();

	void MsgProc(UINT msg, WPARAM wPraram, LPARAM lparam);

	HINSTANCE GetHINSTANCE() const;
	HWND GetWindowHandle() const;

	virtual int Run() = 0;

protected:

	// ====== data members ======

	static IBaseEngine* s_pInstance;
	
	// win32 window
	HWND m_hWindowHandle;
	HINSTANCE m_hInstance;
	
	float m_fDT;
	float m_fStartCount;
	float m_fSecsPerCount;

	RECT m_rect;

	bool m_bPaused;

	// ====== constructor/destructor ======
	IBaseEngine(HINSTANCE hInstance,const string& winCaption);
	virtual ~IBaseEngine();

	// ====== helper functions ======

	/*bool Begin();
	bool End();
	void Present
	bool IsDeviceLost();
	void OnLostDevice();
	void OnResetDevice();
	*/

	void StartCounter();
	void EndCounter();

	bool Update();

	void RedirectIOToConsole();

private:

	void InitializeWindows(HINSTANCE hInstance, const string& winCaption);
};




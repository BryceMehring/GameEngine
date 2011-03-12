
#pragma once
#include "StdAfx.h"

#include "Interfaces.h"

using namespace std;
using namespace stdext;


// ************************
// This really should just manage everything, do not implement it here
// All of the different sub parts of the engine should be independent, they should be their own dll.
// Then this class would load the dll's dynamically and be the leader of the game without knowing its implementation details.
// ***********************

class IBaseEngine : public RefCounting
{
public:

	// Initializes Windows
	IBaseEngine(HINSTANCE hInstance,const string& winCaption);

	void MsgProc(UINT msg, WPARAM wPraram, LPARAM lparam);

	HINSTANCE GetHINSTANCE() const;
	HWND GetWindowHandle() const;

	virtual int Run() = 0;

protected:

	// ====== data members ======
	HWND m_hWindowHandle;
	HINSTANCE m_hInstance;

	bool m_bPaused;

	float m_fDT;
	float m_fStartCount;
	float m_fSecsPerCount;

	// ====== constructor/destructor ======
	~IBaseEngine();

	// ====== helper functions ======
	/*bool Begin();
	bool End();
	void Present
	bool IsDeviceLost();
	void OnLostDevice();
	void OnResetDevice();
	*/

	bool Update();
	

private:

	void InitializeWindows(HINSTANCE hInstance, const string& winCaption);

};

extern IBaseEngine* g_pEngine;




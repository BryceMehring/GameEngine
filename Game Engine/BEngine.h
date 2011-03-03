
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
class BEngine
{
public:

	static BEngine* GetInstance();

	// Initializes DirextX along with Windows
	static void Initialize(HINSTANCE hInstance,const string& winCaption);
	
	// Deletes everything from memory
	static void Delete();
	
	bool Update();
	void Present();

	void MsgProc(UINT msg, WPARAM wPraram, LPARAM lparam);

	HINSTANCE GetHINSTANCE() const;
	HWND GetWindowHandle() const;

private:

	// Single Instance of the class.
	static BEngine* s_pInstance;

	HWND m_hWindowHandle;
	HINSTANCE m_hInstance;

	bool m_bPaused;

	float m_fDT;
	float m_fStartCount;
	float m_fSecsPerCount;

	BEngine(HINSTANCE hInstance,const string& winCaption);
	~BEngine();

	// Prevent copies from being made.
	BEngine(const BEngine&);
	BEngine operator =(const BEngine&);

	void InitializeWindows(HINSTANCE hInstance, const string& winCaption);
	

	bool Begin();
	bool End();

	
	bool IsDeviceLost();
	void OnLostDevice();
	void OnResetDevice();

	

};


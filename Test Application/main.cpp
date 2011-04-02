
#include "main.h"
#include "AngelScript.h"
#include "scriptstdstring.h"
#include "scriptbuilder.h"
#include "asVM.h"
#include <assert.h>


InputTestApp::InputTestApp(HINSTANCE hInstance,const string& winCaption) : IBaseEngine(hInstance,winCaption)
{
	m_pInput = NULL;
	m_pRendering = NULL;
	ZeroMemory(buffer,32);

	LoadDLLS();

	asVM* pVM = asVM::Instance();
	pVM->BuildScriptFromFile("C:\\users\\Bryce\\documents\\visual Studio 2010\\projects\\game engine\\test application\\Test.as");
	pVM->ExecuteScript(0);


	/*Load();
	int r;

	asIScriptEngine *engine = 

	r = engine->RegisterGlobalFunction("int Test(int)", asFUNCTION(print), asCALL_CDECL);

	CScriptBuilder builder;
	r = builder.StartNewModule(engine,"TestModule"); 
	r = builder.AddSectionFromFile("Test.as"); 
	r = builder.BuildModule();

	asIScriptModule* pMod = engine->GetModule("TestModule");
	int id = pMod->GetFunctionIdByDecl("void main()");

	// Create our context, prepare it, and then execute
	asIScriptContext *ctx = engine->CreateContext();
	r = ctx->Prepare(id);
	r = ctx->Execute();

	ctx->Release();
	engine->Release();

	// Fill out the CheckBoxData structure
	/*CheckBoxData checkBox1;
	checkBox1.m_checked = false;
	checkBox1.m_Callback = &InputTestApp::LuaConsole;
		
	checkBox1.m_pos[0].x = 200;
	checkBox1.m_pos[0].y = 200;

	checkBox1.m_pos[1].x = 300;
	checkBox1.m_pos[1].y = 300;

	checkBox1.m_str = "Open Lua Console";*/
}
InputTestApp::~InputTestApp()
{
	asVM::Delete();
}

int InputTestApp::Run()
{
	UIManager* pUI = UIManager::Instance();
	while(Update())
	{
		StartCounter();
		m_pInput->Poll();

		pUI->Update(this->m_fDT);
	
		m_pRendering->Begin();
			
		pUI->Render();
		if(m_pInput->MouseClick(0))
		{
			POINT p;
			m_pInput->MousePos(p);
			wsprintf(buffer,"Mouse X: %d\nMouse Y:%d",p.x,p.y);
			m_pRendering->DrawString(buffer,p,0xffffffff);
		}

		m_pRendering->End();
		m_pRendering->Present();
		EndCounter();
	}

	return 0;
}

void InputTestApp::LoadDLLS()
{
	PluginManager* pPluginManager = PluginManager::Instance();
	m_pInput  = static_cast<IKMInput*>(pPluginManager->LoadDLL("DirectX Input DLL.dll"));
	m_pRendering = static_cast<IRenderingPlugin*>(pPluginManager->LoadDLL("DX9 Rendering.dll"));
}


// The DLL files do not like the singletons. This is why when we pass a singleton
// instance into a dll, it appears null. I need to check this out more...

// the current fix gets rid of using a singleton and uses Reference Counting.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// I want to put all of this Initialization into a dll
	IBaseEngine::Initialize<InputTestApp>(hInstance,"AngelScript Console");
	IBaseEngine* pEngine = InputTestApp::Instance();

	int returnCode = pEngine->Run();

	PluginManager::Delete();
	IBaseEngine::Delete();

	return returnCode;
}
				
																								  
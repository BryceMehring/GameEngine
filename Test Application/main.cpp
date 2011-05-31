#define DEBUG

#include "main.h"
#include "AngelScript.h"
#include "scriptstdstring.h"
#include "scriptbuilder.h"
#include "asVM.h"
#include "Factory.h"
#include <assert.h>

using namespace AngelScript;

// And then, as we go up in recursion, we display the rest of the numbers
template< int n >
struct LoopClass
{
	static void Loop()
	{
		LoopClass<n-1>::Loop();
		cout<<n<<" ";
	}
};

// This Loop gets called first
template<>
struct LoopClass<1>
{
	static void Loop()
	{
		cout<<"1 ";
	}
};


InputTestApp::InputTestApp(HINSTANCE hInstance,const string& winCaption) : IBaseEngine(hInstance,winCaption)
{
	// todo: I need to move initialization into smaller helper functions

	// todo: need to fix this logic
	g_pEngine = this;

	m_pInput = NULL;
	m_pRendering = NULL;
	ZeroMemory(buffer,32);

	LoadDLLS();

	//asVM* pVM = asVM::Instance();
	//pVM->Test();

	//pVM->RegisterScript("Config.txt");

	// registering 

	Factory<IUIElement>* pUIInstance = Factory<IUIElement>::Instance();
	pUIInstance->Register("CheckBox",new Creator<CheckBox,IUIElement>);
	//pUIInstance->Register("CheckBox",new Creator<CheckBox,IUIElement>());

	// UI
	m_pUI = new UI();
	m_pUI->RegisterScript();

	// AS
	AngelScript::asVM* pVM = AngelScript::asVM::Instance();
	unsigned int id = pVM->BuildScriptFromFile("Test.as");
	pVM->ExecuteScript(id);
	//pVM->


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

/*void InputTestApp::Open(bool b)
{

}*/

int InputTestApp::Run()
{
	while(Update())
	{
		StartCounter();
		m_pInput->Poll();

		/*if(m_pInput->KeyDown(0x1C)) // Enter
		{
			//ScriptingConsole(true);
		}*/

		m_pUI->Update(this->m_fDT);
	
		m_pRendering->Begin();

		m_pUI->Render();
			
		//pUI->Render();
		//if(m_pInput->MouseClick(0))
		{
			POINT p;
			m_pInput->MousePos(p);

			RECT R = {0,0,0,0};

			wsprintf(buffer,"Mouse X: %d\nMouse Y:%d",p.x,p.y);
			m_pRendering->DrawString(buffer,R,0xffffffff);
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
	g_pEngine = new InputTestApp(hInstance,"AngelScript");

	int returnCode = g_pEngine->Run();

	// todo: this should be moved elsewhere
	PluginManager::Delete();

	delete g_pEngine;
	g_pEngine = NULL;

	return returnCode;
}
				
																								  
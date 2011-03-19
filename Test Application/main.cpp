
#include "main.h"
#include "AngelScript.h"
#include "scriptstdstring.h"
#include "scriptbuilder.h"
#include <assert.h>

#pragma comment(lib,"AngelScript.lib")

void print(const string& str)
{
	printf(str.c_str());
}

// Implement a simple message callback function
void MessageCallback(const asSMessageInfo *msg, void *param)
{
  const char *type = "ERR ";
  if( msg->type == asMSGTYPE_WARNING ) 
    type = "WARN";
  else if( msg->type == asMSGTYPE_INFORMATION ) 
    type = "INFO";
  printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

InputTestApp::InputTestApp(HINSTANCE hInstance,const string& winCaption) : IBaseEngine(hInstance,winCaption)
{
	ZeroMemory(buffer,32);
}
InputTestApp::~InputTestApp()
{
	
}

int InputTestApp::Run()
{
	Load();
	int r;

	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	RegisterStdString(engine);

	r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); 
	r = engine->RegisterGlobalFunction("void print(const string& in)", asFUNCTION(print), asCALL_CDECL);

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

	while(Update())
	{
		StartCounter();
		/*m_pInput->Poll();

		pUI->Update(this->m_fDT);
	
		m_pRendering->Begin();
			
		pUI->Render();
		if(m_pInput->MouseClick(0))
		{
			POINT p;
			m_pInput->MousePos(p);
			wsprintf(buffer,"Mouse X: %d\nMouse Y:%d",p.x,p.y);
			m_pRendering->DrawString(buffer);
		}*/

		m_pRendering->End();
		m_pRendering->Present();
		EndCounter();
	}

	return 0;
}

void InputTestApp::Load()
{
	PluginManager& pluginManager = PluginManager::Instance();
	m_pInput  = static_cast<IKMInput*>(pluginManager.LoadDLL("DirectX Input DLL.dll"));
	m_pRendering = static_cast<IRenderingPlugin*>(pluginManager.LoadDLL("DX9 Rendering.dll"));
}


// The DLL files do not like the singletons. This is why when we pass a singleton
// instance into a dll, it appears null. I need to check this out more...

// the current fix gets rid of using a singleton and uses Reference Counting.

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// I want to put all of this Initialization into a dll
	g_pEngine = new InputTestApp(hInstance,"Lua Console");

	int returnCode = g_pEngine->Run();

	PluginManager::Delete();
	g_pEngine->Release();

	return returnCode;
}
				
																								  
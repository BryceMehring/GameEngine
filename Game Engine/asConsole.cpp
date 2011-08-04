// Programmed By Bryce Mehring


#include "StdAfx.h"
#include "BEngine.h"
#include "asConsole.h"

using namespace std;

asConsole* asConsole::s_pThis = NULL;

asConsole::asConsole(UIData* pData, UI* pUI) : m_pEngine(pUI->GetEngine()), TextBox(pData,pUI)
{
	//buffer << "AngelScript console: " << asGetLibraryVersion() 
	//SetConsoleTitle( buffer.str().c_str() );
	RegisterScript();

	s_pThis = this;
}
asConsole::~asConsole()
{
	m_pEngine = NULL;
}

void asConsole::Open()
{
	m_pUI->SetFocus(this);
}

void asConsole::Close()
{
	m_pUI->SetFocus(NULL);
}

void asConsole::Run()
{
	//const string& str = m_pData->str;
	//m_pEngine->GetScriptVM().ExecuteScript(str.c_str());
}

void asConsole::Enter()
{
	Build();

	TextBox::Enter();
}

void asConsole::MessageCallback(const asSMessageInfo *msg, void *param)
{
	assert(s_pThis != nullptr);

	const char *type = nullptr;
	DWORD color = 0xffffffff;

	if(msg->type == asMSGTYPE_ERROR)
	{
		color = D3DCOLOR_XRGB(255,0,0);
		type = "ERR ";
	}
	else if( msg->type == asMSGTYPE_WARNING ) 
	{
		color = D3DCOLOR_XRGB(255,255,0);
		type = "WARN";
	}
	else if( msg->type == asMSGTYPE_INFORMATION ) 
	{
		type = "INFO";
	}

	float c = clock() / (float)CLOCKS_PER_SEC;

	char buffer[200];
	sprintf(buffer,"%5.3f(s) %s (%d, %d) : %s : %s\n",c, msg->section, msg->row, msg->col, type, msg->message);

	// Write line to console
	s_pThis->Write(buffer,color);
}
void asConsole::Build()
{
	// todo: need to optimize
	string temp = m_text.back().line;
	temp.erase(temp.begin());
	m_pEngine->GetScriptVM().ExecuteScript(temp);
}

void asConsole::RegisterScript()
{
	asIScriptEngine& scriptEngine = m_pEngine->GetScriptVM().GetScriptEngine();

	m_pEngine->GetScriptVM().SetTextBox(this);

	DBAS(scriptEngine.RegisterObjectType("asConsole",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(scriptEngine.RegisterObjectMethod("asConsole","void open()",asMETHOD(asConsole,Open),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterObjectMethod("asConsole","void close()",asMETHOD(asConsole,Close),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterObjectMethod("asConsole","void cls()",asMETHOD(asConsole,CLS),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterObjectMethod("asConsole","void write(const string& in, uint)",asMETHOD(asConsole,Write),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterObjectMethod("asConsole","void build()",asMETHOD(asConsole,Build),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterGlobalProperty("asConsole console",this));
	
	scriptEngine.Release();
}
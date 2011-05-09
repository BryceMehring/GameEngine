// Programmed by Bryce Mehring
#include "asVM.h"
#include <scriptstdstring.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <Windows.h>

using namespace std;

#pragma comment(lib,"AngelScript.lib")
#pragma comment(lib,"AngelScriptAddons.lib")

class Script
{
public:

	Script() : id(0), pCtx(NULL) {}
	//~Script() { pCtx->Release(); }

	// copy constructor
	//Script(const Script& s) { pCtx->AddRef(); }

	int id;
	asIScriptContext* pCtx;
};

// Implement a simple message callback function
void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";

	if( msg->type == asMSGTYPE_WARNING ) 
	{
		type = "WARN";
	}
	else if( msg->type == asMSGTYPE_INFORMATION ) 
	{
		type = "INFO";
	}

	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
	system("pause");
}

// The global functions that the script calls
void print(const string& d)
{
	cout<<d;
}
void print(int d)
{
	cout<<d;
}
void print(double d)
{
	cout<<d;
}


asVM::asVM()
{
	m_pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	RegisterStdString(m_pEngine);
	RegisterFunctions();

}
asVM::~asVM()
{
	m_pEngine->Release();
}

unsigned int asVM::BuildScriptFromFile(const string& str)
{
	DBAS(m_builder.AddSectionFromFile(str.c_str()));
	DBAS(m_builder.BuildModule());

	asIScriptModule* pMod = m_pEngine->GetModule("Application");

	// fill structure
	Script s;
	s.id = pMod->GetFunctionIdByDecl("void main()");
	s.pCtx = m_pEngine->CreateContext();

	// add script
	m_scripts.push_back(s);

	// return index
	return (m_scripts.size() - 1);
}

void asVM::RemoveScript(unsigned int id)
{
	// Release Context
	m_scripts[id].pCtx->Release();

	// Remove from vector
	m_scripts.erase(m_scripts.begin() + id);
}

asIScriptEngine* asVM::GetScriptEngine() const
{
	m_pEngine->AddRef();
	return m_pEngine;
}

void asVM::ExecuteScript(unsigned int i)
{
	assert(i < m_scripts.size());

	asIScriptContext* ptx = m_scripts[i].pCtx;
	
	DBAS(ptx->Prepare(m_scripts[i].id));
	
	DBAS(ptx->Execute());
	DBAS(ptx->Unprepare());
}


asETokenClass asVM::GetToken(string& token, const string& text, unsigned int& pos)
{
	int len;
	asETokenClass t = m_pEngine->ParseToken(&text[pos], text.length() - pos, &len);
	while( (t == asTC_WHITESPACE || t == asTC_COMMENT) && pos < text.length() )
	{
		pos += len;
		t = m_pEngine->ParseToken(&text[pos], text.length() - pos, &len);
	}

	token.assign(&text[pos], len);

	pos += len;

	return t;
}

/*void asVM::AddFunction(string& arg)
{
	asIScriptModule* mod = m_pEngine->GetModule("console", asGM_CREATE_IF_NOT_EXISTS);

	asIScriptFunction* func = 0;
	int r = mod->CompileFunction("addfunc", arg.c_str(), 0, asCOMP_ADD_TO_MODULE, &func);
	if( r < 0 )
	{
		// TODO: Add better description of error (invalid declaration, name conflict, etc)
		cout << "Failed to add function. " << endl;
	}
	else
	{
		cout <<"Function added. " << endl;
	}

	// We must release the function object
	if( func )
	{
		func->Release();
	}
}*/

void asVM::RegisterFunctions()
{
	DBAS(m_pEngine->SetMessageCallback(asFUNCTION(MessageCallback),0,asCALL_CDECL));

	// Print Function
	DBAS(m_pEngine->RegisterGlobalFunction("void print(const string& in)",asFUNCTIONPR(print,(const string&),void),asCALL_CDECL));
	
	// asVM
	DBAS(m_pEngine->RegisterObjectType("asVM",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","uint BuildScriptFromFile(const string& in)",asMETHOD(asVM,BuildScriptFromFile),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","void ExecuteScript(uint)",asMETHOD(asVM,ExecuteScript),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterGlobalProperty("asVM as",this));

	// POINT
	DBAS(m_pEngine->RegisterObjectType("POINT",sizeof(POINT),asOBJ_VALUE | asOBJ_POD));
	DBAS(m_pEngine->RegisterObjectProperty("POINT","int x",offsetof(POINT,x)));
	DBAS(m_pEngine->RegisterObjectProperty("POINT","int y",offsetof(POINT,y)));

	// RECT
	DBAS(m_pEngine->RegisterObjectType("RECT",sizeof(RECT),asOBJ_VALUE | asOBJ_POD));
	DBAS(m_pEngine->RegisterObjectProperty("RECT","int left",offsetof(RECT,left)));
	DBAS(m_pEngine->RegisterObjectProperty("RECT","int top",offsetof(RECT,top)));
	DBAS(m_pEngine->RegisterObjectProperty("RECT","int right",offsetof(RECT,right)));
	DBAS(m_pEngine->RegisterObjectProperty("RECT","int bottom",offsetof(RECT,bottom)));

	DBAS(m_builder.StartNewModule(m_pEngine,"Application"));
}

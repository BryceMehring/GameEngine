// Programmed by Bryce Mehring

using namespace std;

#include "asVM.h"

#include <iostream>

#pragma comment(lib,"AngelScript.lib")
#pragma comment(lib,"AngelScriptAddons.lib")

using namespace std;

namespace AngelScript
{

DWORD WINAPI ScriptThread(void* p)
{
	asVM* pVM = (asVM*)p;
	return 0;
}

struct Script
{
	Script() : id(0), pCtx(NULL) {}

	int id; // void main() id
	asIScriptContext* pCtx;
	HANDLE thread;
};

// ===== Global functions that are registered with AngelScript =====
void print(const string& d)
{
	cout<<d;
}
void print(int d)
{
	cout<<d;
}

// Returns the Script Function Id
int GetId(asIScriptFunction *func)
{
	int id = -1;

	if( func ) 
	{
		// Do something with the function
		id = func->GetId();

		// Release it when done
		func->Release();
	}

	return id;
}

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
}


asVM::asVM() : m_iExeScript(0)
{
	m_pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	RegisterStdString(m_pEngine);
	RegisterScript();

	DBAS(m_builder.StartNewModule(m_pEngine,"Application"));
}
asVM::~asVM()
{
	// todo: need to release the 
	m_pEngine->Release();
}

unsigned int asVM::BuildScriptFromFile(const string& str)
{
	
	unsigned int index;

	// todo: When a file is selected in the window, its full path is included in the string
	// I could remove everything to the left of the last '\' to improve performance in 
	// searching the m_scriptIndex data structure.
	//str.find_last_of();

	// we first see if the script is already loaded in memory
	ScriptIndexType::iterator iter = m_scriptIndex.find(str);

	if(iter != m_scriptIndex.end())
	{
		index = iter->second;
	}
	else
	{
		DBAS(m_builder.AddSectionFromFile(str.c_str()));

		/*if(R >= 0)
		{
			// todo: when the file cannot compile, the program should not crash.
			// remove some of the DBAS.
		}*/

		DBAS(m_builder.BuildModule());

		asIScriptModule* pMod = m_pEngine->GetModule("Application");
		int mainId = pMod->GetFunctionIdByDecl("void main()");

		// add script
		Script s;
		s.id = mainId;
		s.pCtx = m_pEngine->CreateContext();
		s.thread = CreateThread(NULL,NULL,ScriptThread,this,CREATE_SUSPENDED,NULL);

		m_scripts.push_back(s);

		index = (m_scripts.size() - 1);
		m_scriptIndex.insert(make_pair(str,index));
	}

	return index;
}

void asVM::RemoveScript(unsigned int id)
{
	// Remove from vector. not the most efficient data structure for this task. 

	vector<Script>::iterator iter = m_scripts.begin() + id;
	
	iter->pCtx->Release();
	
	m_scripts.erase(iter);
}

asIScriptEngine& asVM::GetScriptEngine() const
{
	m_pEngine->AddRef();
	return *m_pEngine;
}

void asVM::ExecuteScript(unsigned int scriptId)
{
	assert(scriptId < m_scripts.size());

	Script s = m_scripts[scriptId];
	m_iExeScript = scriptId;

	//ResumeThread(s.thread);

	DBAS(s.pCtx->Prepare(s.id));
	
	DBAS(s.pCtx->Execute());
	DBAS(s.pCtx->Unprepare());
}
void asVM::ExecuteScriptFunction(unsigned int scriptId, int funcId)
{
	assert(scriptId < m_scripts.size());

	Script s = m_scripts[scriptId];
	m_iExeScript = scriptId;
	
	DBAS(s.pCtx->Prepare(funcId));
	DBAS(s.pCtx->Execute());
	DBAS(s.pCtx->Unprepare());
}

void asVM::ExecuteScriptFunction(unsigned int scriptId, int funcId, char param)
{
	assert(scriptId < m_scripts.size());

	Script s = m_scripts[scriptId];
	m_iExeScript = scriptId;
	
	DBAS(s.pCtx->Prepare(funcId));
	
	DBAS(s.pCtx->SetArgByte(0,param));
	
	DBAS(s.pCtx->Execute());
	DBAS(s.pCtx->Unprepare());
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

void asVM::RegisterScript()
{
	DBAS(m_pEngine->SetMessageCallback(asFUNCTION(MessageCallback),0,asCALL_CDECL));
	
	// Funcdefs
	DBAS(m_pEngine->RegisterFuncdef("void AppCallback(bool)"));
	//DBAS(m_pEngine->RegisterFuncdef("void AppCallback()"));

	// Global Properties
	DBAS(m_pEngine->RegisterGlobalProperty("const uint scriptId",&m_iExeScript));

	// Global Functions
	DBAS(m_pEngine->RegisterGlobalFunction("void print(const string& in)",asFUNCTIONPR(print,(const string&),void),asCALL_CDECL));
	DBAS(m_pEngine->RegisterGlobalFunction("void print(int)",asFUNCTIONPR(print,(int),void),asCALL_CDECL));
	DBAS(m_pEngine->RegisterGlobalFunction("int GetId(AppCallback @)", asFUNCTION(GetId), asCALL_CDECL));

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
}

}; // AngelScript

// Programmed by Bryce Mehring

#include "StdAfx.h"
#include "asVM.h"
#include "EngineHelper.h"

#pragma comment(lib,"AngelScript.lib")
#pragma comment(lib,"AngelScriptAddons.lib")

using namespace std;

namespace AngelScript
{

// constants, todo: maybe this is not the best location for these
const WORD BLACK = 0;
const WORD BLUE = 9;
const WORD GREEN = 10;
const WORD CYAN = 11;
const WORD RED = 12;
const WORD MAGENTA = 13;
const WORD ORANGE = 6;
const WORD YELLOW = 14;
const WORD WHITE = 15;


struct Script
{
	Script() : id(0), pCtx(nullptr) {}

	int id; // void main() id
	asIScriptContext* pCtx;
};

// ===== Global functions that are registered with AngelScript =====
void cls()
{
	system("cls");
}
void print(const string& d)
{
	cout<<d;
}
void print(int d)
{
	cout<<d;
}

// Implement a simple message callback function. todo: need to move this into the asConsole class?
// todo: need to move the text output to the logging class
void MessageCallback(const asSMessageInfo *msg, void *param)
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);

	const char *type = nullptr;

	if(msg->type == asMSGTYPE_ERROR)
	{
		SetConsoleTextAttribute(hCon,RED);
		type = "ERR ";
	}
	else if( msg->type == asMSGTYPE_WARNING ) 
	{
		SetConsoleTextAttribute(hCon,ORANGE);
		type = "WARN";
	}
	else if( msg->type == asMSGTYPE_INFORMATION ) 
	{
		SetConsoleTextAttribute(hCon,WHITE);
		type = "INFO";
	}

	float c = clock() / (float)CLOCKS_PER_SEC;

	printf("%5.3f(s) %s (%d, %d) : %s : %s\n",c, msg->section, msg->row, msg->col, type, msg->message);

	SetConsoleTextAttribute(hCon,WHITE);
}


asVM::asVM() : m_iExeScript(0)
{
	m_pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	RegisterStdString(m_pEngine);
	RegisterScript();
}
asVM::~asVM()
{
	// Release all contexts
	for(unsigned int i = 0; i < m_scripts.size(); ++i)
	{
		m_scripts[i].pCtx->Release();
	}

	// release the Script Engine
	m_pEngine->Release();
}

unsigned int asVM::BuildScriptFromFile(const string& str)
{

	// todo: need to cache scripts if they are marked with "final"?
	// todo: Need to implement Pre-compiled byte code
	
	unsigned int index;

	// todo: When a file is selected in the window, its full path is included in the string
	// I could remove everything to the left of the last '\' to improve performance in 
	// searching the m_scriptIndex data structure.
	//str.find_last_of();

	// we first see if the script is already loaded in memory
	//ScriptIndexType::iterator iter = m_scriptIndex.find(str);

	//if(iter == m_scriptIndex.end())
	{
		int R;

		DBAS(m_builder.StartNewModule(m_pEngine,"Application"));

		R = m_builder.AddSectionFromFile(str.c_str());

		if(R < 0)
		{
			return -1;
		}

		R = m_builder.BuildModule();

		if(R < 0)
		{
			return -1;
		}

		asIScriptModule* pMod = m_pEngine->GetModule("Application");
		int mainId = pMod->GetFunctionIdByDecl("void main()");

		// add script....
		Script s;
		s.id = mainId;
		s.pCtx = m_pEngine->CreateContext();

		m_scripts.push_back(s);

		index = (m_scripts.size() - 1);
		//m_scriptIndex.insert(make_pair(str,index));
	}
	/*else
	{
		index = iter->second;
	}*/

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
	if(GoodScriptId(scriptId))
	{
		Script s = m_scripts[scriptId];
		m_iExeScript = scriptId;


		m_pEngine->WriteMessage("",0,0,asMSGTYPE_INFORMATION,"Script Exec");

		//ResumeThread(s.thread);

		DBAS(s.pCtx->Prepare(s.id));
	
		DBAS(s.pCtx->Execute());
		DBAS(s.pCtx->Unprepare());
	}
}
void asVM::ExecuteScript(const char* script)
{
	ExecuteString(m_pEngine,script);
}
void asVM::ExecuteScriptFunction(unsigned int scriptId, int funcId)
{
	if(GoodScriptId(scriptId))
	{
		Script s = m_scripts[scriptId];
		m_iExeScript = scriptId;
	
		DBAS(s.pCtx->Prepare(funcId));
		DBAS(s.pCtx->Execute());
		DBAS(s.pCtx->Unprepare());
	}
}

void asVM::ExecuteScriptFunction(unsigned int scriptId, int funcId, char param)
{
	if(GoodScriptId(scriptId))
	{
		Script s = m_scripts[scriptId];
		m_iExeScript = scriptId;
	
		DBAS(s.pCtx->Prepare(funcId));
	
		DBAS(s.pCtx->SetArgByte(0,param));
	
		DBAS(s.pCtx->Execute());
		DBAS(s.pCtx->Unprepare());
	}
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

bool asVM::GoodScriptId(unsigned int id) const
{
	return (id < m_scripts.size());
}

// Returns the Script Function Id
ScriptFunction asVM::GetFunc(asIScriptFunction* pFunc) const
{
	ScriptFunction func;

	if( pFunc ) 
	{
		// Do something with the function
		func.ifuncId = pFunc->GetId();
		func.iScriptIndex = m_iExeScript;

		// Release it when done
		pFunc->Release();
	}

	return func;
}

void asVM::ListVariables()
{
	asUINT n;

	// List the application registered variables
	cout << "Application variables:" << endl;
	for( n = 0; n < (asUINT)m_pEngine->GetGlobalPropertyCount(); n++ )
	{
		const char *name;
		int typeId;
		bool isConst;

		m_pEngine->GetGlobalPropertyByIndex(n, &name, &typeId, &isConst);
		string decl = isConst ? " const " : " ";
		decl += m_pEngine->GetTypeDeclaration(typeId);
		decl += " ";
		decl += name;
		cout << decl << endl;
	}
}

void asVM::ListObjects()
{
	// todo: need to clean up this method
	cout << "Registered Objects:" << endl;
	
	ostringstream objs;
	for(int o = 0; o < m_pEngine->GetObjectTypeCount(); ++o)
	{
		asIObjectType* pObjType = m_pEngine->GetObjectTypeByIndex(o);
		const char* pObjName = pObjType->GetName();

		objs << endl << pObjName << endl << "Methods: " << endl;

		for(int m = 0; m < pObjType->GetMethodCount(); ++m)
		{
			asIScriptFunction* pMethod = pObjType->GetMethodDescriptorByIndex(m);
			objs << pMethod->GetName() << endl;
		}

		objs << endl << "Properties: " << endl;

		for(int p = 0; p < pObjType->GetPropertyCount(); ++p)
		{
			const char* pPropertyStr = pObjType->GetPropertyDeclaration(p);
			objs << endl << pPropertyStr << endl;
		}
	}

	cout<<objs.str()<<endl;
}

void asVM::ListFunctions()
{
	asUINT n;
	
	// List the application registered functions
	cout << "Application functions:" << endl;
	for( n = 0; n < (asUINT)m_pEngine->GetGlobalFunctionCount(); n++ )
	{
		int id = m_pEngine->GetGlobalFunctionIdByIndex(n);
		asIScriptFunction *func = m_pEngine->GetFunctionDescriptorById(id);

		// Skip the functions that start with _ as these are not meant to be called explicitly by the user
		if( func->GetName()[0] != '_' )
			cout << " " << func->GetDeclaration() << endl;
	}

	// List the user functions in the module
	asIScriptModule *mod = m_pEngine->GetModule("Application");
	if( mod )
	{
		cout << endl;
		cout << "User functions:" << endl;
		for( n = 0; n < (asUINT)mod->GetFunctionCount(); n++ )
		{
			asIScriptFunction *func = mod->GetFunctionDescriptorByIndex(n);
			cout << " " << func->GetDeclaration() << endl;
		}
	}
}

void asVM::RegisterScript()
{
	// logging output
	DBAS(m_pEngine->SetMessageCallback(asFUNCTION(MessageCallback),0,asCALL_CDECL));

	// ============= POD =============

	// the macro offsetof should only work on pod types(no virtual methods),
	// but it does seem to work on regular classes that use polymorphism. todo: need to look into this.

	// ScriptFunction
	DBAS(m_pEngine->RegisterObjectType("ScriptFunction",sizeof(ScriptFunction),asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS));
	DBAS(m_pEngine->RegisterObjectProperty("ScriptFunction","int funcId",offsetof(ScriptFunction,ifuncId)));
	DBAS(m_pEngine->RegisterObjectProperty("ScriptFunction","uint scriptIndex",offsetof(ScriptFunction,iScriptIndex)));

	// POINT
	DBAS(m_pEngine->RegisterObjectType("POINT",sizeof(POINT),asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS));
	DBAS(m_pEngine->RegisterObjectProperty("POINT","int x",offsetof(POINT,x)));
	DBAS(m_pEngine->RegisterObjectProperty("POINT","int y",offsetof(POINT,y)));

	// RECT
	DBAS(m_pEngine->RegisterObjectType("RECT",sizeof(RECT),asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS));
	DBAS(m_pEngine->RegisterObjectProperty("RECT","int left",offsetof(RECT,left)));
	DBAS(m_pEngine->RegisterObjectProperty("RECT","int top",offsetof(RECT,top)));
	DBAS(m_pEngine->RegisterObjectProperty("RECT","int right",offsetof(RECT,right)));
	DBAS(m_pEngine->RegisterObjectProperty("RECT","int bottom",offsetof(RECT,bottom)));

	// ============= Funcdefs ============= 
	DBAS(m_pEngine->RegisterFuncdef("void AppCallback(bool)"));
	//DBAS(m_pEngine->RegisterFuncdef("void AppCallback()"));

	// ============= Global Functions =============
	DBAS(m_pEngine->RegisterGlobalFunction("void print(const string& in)",asFUNCTIONPR(print,(const string&),void),asCALL_CDECL));
	DBAS(m_pEngine->RegisterGlobalFunction("void print(int)",asFUNCTIONPR(print,(int),void),asCALL_CDECL));
	DBAS(m_pEngine->RegisterGlobalFunction("void cls()",asFUNCTION(cls),asCALL_CDECL));

	// ============= asVM =============
	DBAS(m_pEngine->RegisterObjectType("asVM",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","uint BuildScriptFromFile(const string& in)",asMETHOD(asVM,BuildScriptFromFile),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","void ExecuteScript(uint)",asMETHODPR(asVM,ExecuteScript,(unsigned int),void),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","ScriptFunction GetFunc(AppCallback @) const",asMETHOD(asVM,GetFunc),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","void ListVariables()",asMETHOD(asVM,ListVariables),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","void ListObjects()",asMETHOD(asVM,ListObjects),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","void ListFunctions()",asMETHOD(asVM,ListFunctions),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterGlobalProperty("asVM as",this));
}

}; // AngelScript

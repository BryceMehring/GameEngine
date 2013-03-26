// Programmed by Bryce Mehring

// todo: somehow, I need to register the delegate with AngelScript

#include "asVM.h"
#include "Delegates.h"
#include "scripthelper\scripthelper.h"
#include "scriptarray\scriptarray.h"
#include "scriptmath\scriptmath.h"
#include "FileManager.h"
#include "StringAlgorithms.h" // todo: remove this file
#include "Menu.h"

#include <sstream>
#include <ctime>
#include <cassert>
//#include "asConsole.h"
//#include "EngineHelper.h"

#ifdef _DEBUG
#pragma comment(lib,"angelscriptd.lib")
#else
#pragma comment(lib,"angelscript.lib")
#endif

using namespace std;

struct Script
{
	Script() : pFunction(nullptr), pCtx(nullptr) {}

	asIScriptFunction* pFunction;
	asIScriptContext* pCtx;
	string fileName;

};

// The line callback function is called by the VM for each statement that is executed
void LineCallback(asIScriptContext *ctx, unsigned int *timeOut)
{
	// If the time out is reached we suspend the script
	if( *timeOut < clock() )
	{
		ctx->Suspend();
	}
}

asVM::asVM() : m_iExeScript(0)
{
	m_pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	assert(m_pEngine != nullptr);
	m_pEngine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES,true);
	m_pEngine->SetEngineProperty(asEP_USE_CHARACTER_LITERALS, true);

	RegisterStdString(m_pEngine);

	::FileManager::Instance().RegisterScript(*this);
	::RegisterStrAlgorScript(*this);

	// turn off auto GARBAGE_COLLECT
	//m_pEngine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT,false);
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

void asVM::Save() const
{
	::WriteConfigToFile(m_pEngine,"config.txt");
}

unsigned int asVM::BuildScriptFromFile(const string& file)
{

	// todo: need to cache scripts if they are marked with "final"?
	// todo: Need to implement Pre-compiled byte code
	// todo: When a file is selected in the window, its full path is included in the string
	// I could remove everything to the left of the last '\' to improve performance in 
	// searching the m_scriptIndex data structure.
	//str.find_last_of();

	DBAS(m_builder.StartNewModule(m_pEngine,"Application"));
	int R = m_builder.AddSectionFromFile(file.c_str());

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

	// cache script....
	Script s;
	s.pFunction = pMod->GetFunctionByName("void main()");
	s.pCtx = m_pEngine->CreateContext();
	s.fileName = file;

	m_scripts.push_back(s);

	return (m_scripts.size() - 1);
}

void asVM::RemoveScript(unsigned int id)
{
	// Remove from vector. not the most efficient data structure for this task. 

	if(id != m_iExeScript)
	{
		vector<Script>::iterator iter = m_scripts.begin() + id;
	
		iter->pCtx->Release();
	
		m_scripts.erase(iter);
	}
}

asIScriptEngine* asVM::GetScriptEngine() const
{
	m_pEngine->AddRef();
	return m_pEngine;
}

void asVM::ExecuteScript(unsigned int scriptId)
{
	if(GoodScriptId(scriptId))
	{
		Script s = m_scripts[scriptId];
		m_iExeScript = scriptId;

		char buffer[100];
		sprintf_s(buffer,"%s %s","Script Exec:",s.fileName.c_str());

		DBAS(m_pEngine->WriteMessage("",0,0,asMSGTYPE_INFORMATION,buffer));

		DBAS(s.pCtx->Prepare(s.pFunction));
	
		DBAS(s.pCtx->Execute());
		DBAS(s.pCtx->Unprepare());
	}
}

void asVM::ExecuteScript(const string& script, asDWORD mask)
{
	::asIScriptContext* ctx = m_pEngine->CreateContext();

	// Define the timeout as 1 second
	unsigned int timeOut = clock() + 1000;

	// Set up the line callback that will timout the script
	ctx->SetLineCallback(asFUNCTION(LineCallback), &timeOut, asCALL_CDECL);

	ExecuteString(this->m_pEngine,script.c_str(),nullptr,ctx);

	ctx->Release();
}
void asVM::ExecuteScriptFunction(const ScriptFunctionStruct& func)
{
	if(GoodScriptId(func.iScriptIndex))
	{
		Script s = m_scripts[func.iScriptIndex];
		m_iExeScript = func.ifuncId; // todo: this might be bugged
	
		DBAS(s.pCtx->Prepare(s.pFunction));
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

		DBAS(s.pCtx->Prepare(s.pFunction));
	
		DBAS(s.pCtx->SetArgByte(0,param));
	
		DBAS(s.pCtx->Execute());
		DBAS(s.pCtx->Unprepare());
	}
}


/*asETokenClass asVM::GetToken(string& token, const string& text, unsigned int& pos)
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
}*/

bool asVM::GoodScriptId(unsigned int id) const
{
	return (id < m_scripts.size());
}

// Returns the Script Function Id, this is called from the script
asVM::asDelegate asVM::GetFunc(asIScriptFunction* pFunc)
{
	ScriptFunctionStruct func;

	if( pFunc ) 
	{
		// Do something with the function
		func.ifuncId = pFunc->GetId();
		func.iScriptIndex = m_iExeScript;

		/*
		Bug[FIXED]: In the latest AngelScript Update they added auto garbage collection
		with calling AddRef and Release. Since I didn't update this method, two Release's were
		being called on the function, which brings the counter down to 0 and delete's it.
		To fix the crash, we no longer call Release and AddRef anymore, AngelScript will
		manage this for us. 

		AngelScript Version 2.21.0 - 2011/07/03
		Date Found: 7/24/2011*/
		
		//pFunc->Release();
	}

	asDelegate d;

	// todo: need to fix
	d.Bind(this,func);
	//d.Bind(this,func);

	return d;
}

void asVM::ListVariables()
{
	asUINT n;

	// List the application registered variables
	for( n = 0; n < (asUINT)m_pEngine->GetGlobalPropertyCount(); n++ )
	{
		const char *name;
		int typeId;
		bool isConst;

		m_pEngine->GetGlobalPropertyByIndex(n, &name,nullptr, &typeId, &isConst);

		string decl = isConst ? " const " : " ";
		decl += m_pEngine->GetTypeDeclaration(typeId);
		decl += " ";
		decl += name;
		decl += "\n";

		SendMessage(decl);
	}
}

void asVM::ListObjects()
{
	for(int o = 0; o < m_pEngine->GetObjectTypeCount(); ++o)
	{
		ostringstream os;

		asIObjectType* pObjType = m_pEngine->GetObjectTypeByIndex(o);
		const char* pObjName = pObjType->GetName();

		os << pObjName << endl << "Methods: " << endl;

		for(int m = 0; m < pObjType->GetMethodCount(); ++m)
		{
			asIScriptFunction* pMethod = pObjType->GetMethodByIndex(m);
			os << pMethod->GetName() << ", ";
		}

		/*os << endl << "Properties: " << endl;

		for(int p = 0; p < pObjType->GetPropertyCount(); ++p)
		{
			const char* pPropertyStr = pObjType->GetPropertyDeclaration(p);
			os << endl << pPropertyStr << endl;
		}*/

		os << endl;
		os << endl;

		SendMessage(os.str());
		
	}

	//m_pTextBox->Write(os.str());
}

void asVM::ListFunctions()
{
	asUINT n;

	// List the application registered functions
	
	for( n = 0; n < (asUINT)m_pEngine->GetGlobalFunctionCount(); n++ )
	{
		asIScriptFunction* func = m_pEngine->GetGlobalFunctionByIndex(n);

		// Skip the functions that start with _ as these are not meant to be called explicitly by the user
		if( func->GetName()[0] != '_' )
		{
			SendMessage(func->GetDeclaration());
		}
	}

	// List the user functions in the module
	asIScriptModule *mod = m_pEngine->GetModule("Application");
	if( mod )
	{
		ostringstream os;

		os << "User functions:" << endl;
		for( n = 0; n < (asUINT)mod->GetFunctionCount(); n++ )
		{
			asIScriptFunction *func = mod->GetFunctionByIndex(n);
			os << " " << func->GetDeclaration() << endl;

			SendMessage(func->GetDeclaration());
		}
	}

}

/*void asVM::SetTextBox(TextBox* pTextBox)
{
	m_pTextBox = pTextBox;
}*/

void asVM::SendMessage(const std::string& msg) const
{
	DBAS(m_pEngine->WriteMessage("SendMessage",0,0,asMSGTYPE_INFORMATION,msg.c_str()));
}

void asVM::RegisterScript(ScriptingConsole* pTextBox)
{
	// logging output
	DBAS(m_pEngine->SetMessageCallback(asMETHOD(ScriptingConsole,MessageCallback),pTextBox,asCALL_THISCALL));

	//DBAS(m_pEngine->SetDefaultAccessMask(0xffffffff));

	RegisterScriptMath(m_pEngine);
	Math::RegisterScriptVecMath(m_pEngine);

	// ============= Funcdefs ============= 
	DBAS(m_pEngine->RegisterFuncdef("void AppCallback()"));
	DBAS(m_pEngine->RegisterFuncdef("void UintAppCallback(uint)"));
	//DBAS(m_pEngine->RegisterFuncdef("void AppCallback()"));

	// ============= asVM =============
	DBAS(m_pEngine->RegisterObjectType("asVM",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","uint BuildScriptFromFile(const string& in)",asMETHOD(asVM,BuildScriptFromFile),asCALL_THISCALL));
	//DBAS(m_pEngine->RegisterObjectMethod("asVM","void ExecuteScript(const string& in)",asMETHODPR(asVM,ExecuteScript,(const string&),void),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","void ExecuteScript(uint)",asMETHODPR(asVM,ExecuteScript,(unsigned int),void),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","void RemoveScript(uint)",asMETHOD(asVM,RemoveScript),asCALL_THISCALL));
	//DBAS(m_pEngine->RegisterObjectMethod("asVM","VoidDelegate GetFunc(AppCallback @) const",asMETHOD(asVM,GetFunc),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","void ListVariables()",asMETHOD(asVM,ListVariables),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","void ListObjects()",asMETHOD(asVM,ListObjects),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterObjectMethod("asVM","void ListFunctions()",asMETHOD(asVM,ListFunctions),asCALL_THISCALL));
	DBAS(m_pEngine->RegisterGlobalProperty("asVM as",(void*)this));
}

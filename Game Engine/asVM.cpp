// Programmed by Bryce Mehring

// todo: somehow, I need to register the delegate with AngelScript

#include "asVM.h"
#include "Delegates.h"
#include "scripthelper\scripthelper.h"
#include "scriptarray\scriptarray.h"
#include <sstream>
//#include "asConsole.h"
//#include "EngineHelper.h"

#pragma comment(lib,"AngelScript.lib")
#pragma comment(lib,"AngelScriptAddons.lib")

using namespace std;

struct Script
{
	Script() : id(0), pCtx(nullptr) {}

	int id; // void main() id
	asIScriptContext* pCtx;
	string fileName;

};

asVM::asVM() : m_iExeScript(0), m_pTextBox(nullptr)
{
	m_pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	if(s_pThis == NULL)
	{
		s_pThis = this;
	}
	// turn off GARBAGE_COLLECT
	//m_pEngine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT,false);
	//RegisterScript();
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
	int mainId = pMod->GetFunctionIdByDecl("void main()");

	// cache script....
	Script s;
	s.id = mainId;
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

		m_pEngine->WriteMessage("",0,0,asMSGTYPE_INFORMATION,buffer);

		DBAS(s.pCtx->Prepare(s.id));
	
		DBAS(s.pCtx->Execute());
		DBAS(s.pCtx->Unprepare());
	}
}
void asVM::ExecuteScript(const string& script)
{
	ExecuteString(m_pEngine,script.c_str());
}
void asVM::ExecuteScriptFunction(const ScriptFunctionStruct& func)
{
	if(GoodScriptId(func.iScriptIndex))
	{
		Script s = m_scripts[func.iScriptIndex];
		m_iExeScript = func.ifuncId; // todo: this might be bugged
	
		DBAS(s.pCtx->Prepare(func.ifuncId));
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

/*void asVM::ListVariables()
{
	asUINT n;
	ostringstream os;
	os << "Application variables: " << endl;

	// List the application registered variables
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
		decl += "\n";

		os << name << endl;
	}
	//m_pTextBox->Write(os.str()); // todo: need to fix
}

void asVM::ListObjects()
{
	//ostringstream os;
	//os << "Registered Objects: " << endl;
		
	for(int o = 0; o < m_pEngine->GetObjectTypeCount(); ++o)
	{
		ostringstream os;

		asIObjectType* pObjType = m_pEngine->GetObjectTypeByIndex(o);
		const char* pObjName = pObjType->GetName();

		os << endl << pObjName << endl << "Methods: " << endl;

		for(int m = 0; m < pObjType->GetMethodCount(); ++m)
		{
			asIScriptFunction* pMethod = pObjType->GetMethodDescriptorByIndex(m);
			os << pMethod->GetName() << endl;
		}

		os << endl << "Properties: " << endl;

		for(int p = 0; p < pObjType->GetPropertyCount(); ++p)
		{
			const char* pPropertyStr = pObjType->GetPropertyDeclaration(p);
			os << endl << pPropertyStr << endl;
		}

		m_pTextBox->Write(os.str());
	}

	//m_pTextBox->Write(os.str());
}

void asVM::ListFunctions()
{
	asUINT n;

	ostringstream os;
	os << "Application functions:" << endl;
	// List the application registered functions
	
	for( n = 0; n < (asUINT)m_pEngine->GetGlobalFunctionCount(); n++ )
	{
		int id = m_pEngine->GetGlobalFunctionIdByIndex(n);
		asIScriptFunction *func = m_pEngine->GetFunctionDescriptorById(id);

		// Skip the functions that start with _ as these are not meant to be called explicitly by the user
		if( func->GetName()[0] != '_' )
		{
			os << func->GetDeclaration() << endl;
		}
	}

	os << endl;

	// List the user functions in the module
	asIScriptModule *mod = m_pEngine->GetModule("Application");
	if( mod )
	{
		os << "User functions:" << endl;
		for( n = 0; n < (asUINT)mod->GetFunctionCount(); n++ )
		{
			asIScriptFunction *func = mod->GetFunctionDescriptorByIndex(n);
			os << " " << func->GetDeclaration() << endl;
		}
	}

	m_pTextBox->Write(os.str());
}

void asVM::SetTextBox(TextBox* pTextBox)
{
	m_pTextBox = pTextBox;
}

void asVM::_RegisterScript()
{
	// logging output
	DBAS(s_pScriptEngine->SetMessageCallback(asFUNCTION(asConsole::MessageCallback),0,asCALL_CDECL));

	RegisterStdString(s_pScriptEngine);

	// ============= POD =============

	// the macro offsetof should only work on pod types(no virtual methods),
	// but it does seem to work on regular classes that use polymorphism. todo: need to look into this.

	// ScriptFunction
	DBAS(s_pScriptEngine->RegisterObjectType("ScriptFunction",sizeof(ScriptFunction),asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS));
	DBAS(s_pScriptEngine->RegisterObjectProperty("ScriptFunction","int funcId",offsetof(ScriptFunction,ifuncId)));
	DBAS(s_pScriptEngine->RegisterObjectProperty("ScriptFunction","uint scriptIndex",offsetof(ScriptFunction,iScriptIndex)));

	// Delegate
	DBAS(s_pScriptEngine->RegisterObjectType("Delegate",sizeof(asDelegate),asOBJ_VALUE | asOBJ_APP_CLASS_C));
	DBAS(s_pScriptEngine->RegisterObjectBehaviour("Delegate",asBEHAVE_CONSTRUCT,"void f()",asFUNCTION((EngineHelper::Construct<asDelegate>)),asCALL_CDECL_OBJLAST));
	DBAS(s_pScriptEngine->RegisterObjectBehaviour("Delegate",asBEHAVE_CONSTRUCT,"void f(const VoidDelegate &in)",asFUNCTION((EngineHelper::CopyConstruct<asDelegate>)),asCALL_CDECL_OBJLAST));
	DBAS(s_pScriptEngine->RegisterObjectBehaviour("Delegate",asBEHAVE_DESTRUCT,"void f()",asFUNCTION((EngineHelper::Destroy<asDelegate>)),asCALL_CDECL_OBJLAST));
	
	DBAS(s_pScriptEngine->RegisterObjectMethod("Delegate","VoidDelegate& opAssign(const VoidDelegate &in)", asMETHODPR(asDelegate, operator =, (const asDelegate&), asDelegate&), asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("Delegate","void call()",asMETHOD(asDelegate,Call),asCALL_THISCALL));

	// POINT
	DBAS(s_pScriptEngine->RegisterObjectType("POINT",sizeof(POINT),asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS));
	DBAS(s_pScriptEngine->RegisterObjectProperty("POINT","int x",offsetof(POINT,x)));
	DBAS(s_pScriptEngine->RegisterObjectProperty("POINT","int y",offsetof(POINT,y)));

	// RECT
	DBAS(s_pScriptEngine->RegisterObjectType("RECT",sizeof(RECT),asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS));
	DBAS(s_pScriptEngine->RegisterObjectProperty("RECT","int left",offsetof(RECT,left)));
	DBAS(s_pScriptEngine->RegisterObjectProperty("RECT","int top",offsetof(RECT,top)));
	DBAS(s_pScriptEngine->RegisterObjectProperty("RECT","int right",offsetof(RECT,right)));
	DBAS(s_pScriptEngine->RegisterObjectProperty("RECT","int bottom",offsetof(RECT,bottom)));

	// ============= Funcdefs ============= 
	DBAS(s_pScriptEngine->RegisterFuncdef("void AppCallback()"));
	DBAS(s_pScriptEngine->RegisterFuncdef("void UintAppCallback(uint)"));
	//DBAS(m_pEngine->RegisterFuncdef("void AppCallback()"));

	// ============= array =============

	RegisterScriptArray(s_pScriptEngine,false);

	// ============= asVM =============
	DBAS(s_pScriptEngine->RegisterObjectType("asVM",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(s_pScriptEngine->RegisterObjectMethod("asVM","uint BuildScriptFromFile(const string& in)",asMETHOD(asVM,BuildScriptFromFile),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("asVM","void ExecuteScript(const string& in)",asMETHODPR(asVM,ExecuteScript,(const string&),void),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("asVM","void ExecuteScript(uint)",asMETHODPR(asVM,ExecuteScript,(unsigned int),void),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("asVM","void RemoveScript(uint)",asMETHOD(asVM,RemoveScript),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("asVM","VoidDelegate GetFunc(AppCallback @) const",asMETHOD(asVM,GetFunc),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("asVM","void ListVariables()",asMETHOD(asVM,ListVariables),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("asVM","void ListObjects()",asMETHOD(asVM,ListObjects),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("asVM","void ListFunctions()",asMETHOD(asVM,ListFunctions),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterGlobalProperty("asVM as",(void*)s_pThis));
}*/

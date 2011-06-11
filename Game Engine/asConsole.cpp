// Programmed By Bryce Mehring

#include "StdAfx.h"

#include "BEngine.h"
#include "asConsole.h"
#include "asVM.h"

using namespace std;

asConsole::asConsole() : m_hConsole(GetConsoleWindow())
{
	RegisterScript();
}
asConsole::~asConsole()
{
	
}

void asConsole::Open()
{

	Go();
	ShowWindow( m_hConsole, SW_SHOW );
}

void asConsole::Close()
{
	Stop();

	ShowWindow( m_hConsole, SW_HIDE );
}

void asConsole::CLS() const
{
	system("cls");
}

void asConsole::DoWork()
{
	CLS();
	ConsoleInfo();

	cin.clear();

	while(!m_stoprequested)
	{
		Run();
	}
}

void asConsole::Run()
{
	string input;
	input.resize(256);
		
	//  \ ===== Get Input ===== /

	cout << endl << "> ";
	cin.getline(&input[0], 256);

	//  / ===== Get Input ===== \

	// Trim unused characters
	input.resize(strlen(input.c_str()));

	// ExecuteScript
	g_pEngine->GetScriptVM().ExecuteScript(input.c_str());
}

void asConsole::ConsoleInfo() const
{
	// Print some useful information
	cout << "AngelScript console: " << asGetLibraryVersion() << endl;
}

void asConsole::RegisterScript()
{
	asIScriptEngine& scriptEngine = g_pEngine->GetScriptVM().GetScriptEngine();

	DBAS(scriptEngine.RegisterObjectType("asConsole",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(scriptEngine.RegisterObjectMethod("asConsole","void Open()",asMETHOD(asConsole,Open),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterObjectMethod("asConsole","void Close()",asMETHOD(asConsole,Close),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterObjectMethod("asConsole","void cls() const",asMETHOD(asConsole,CLS),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterGlobalProperty("asConsole scriptConsole",this));
	
	scriptEngine.Release();
}


/*
void asConsole::ListVariables() const
{
	asUINT n;

	asVM* pVM = asVM::Instance();
	asIScriptEngine* pEngine = pVM->GetScriptEngine();

	// List the application registered variables
	cout << "Application variables:" << endl;
	for( n = 0; n < (asUINT)pEngine->GetGlobalPropertyCount(); n++ )
	{
		const char *name;
		int typeId;
		bool isConst;
		pEngine->GetGlobalPropertyByIndex(n, &name, &typeId, &isConst);
		string decl = isConst ? " const " : " ";
		decl += pEngine->GetTypeDeclaration(typeId);
		decl += " ";
		decl += name;
		cout << decl << endl;
	}

	// List the user variables in the module
	asIScriptModule *mod = pEngine->GetModule("console");
	if( mod )
	{
		cout << endl;
		cout << "User variables:" << endl;
		for( n = 0; n < (asUINT)mod->GetGlobalVarCount(); n++ )
		{
			cout << " " << mod->GetGlobalVarDeclaration(n) << endl;
		}
	}

}

void asConsole::ListFunctions() const
{
	asUINT n;

	asVM* pVM = asVM::Instance();
	asIScriptEngine* pEngine = pVM->GetScriptEngine();
	
	// List the application registered functions
	cout << "Application functions:" << endl;
	for( n = 0; n < (asUINT)pEngine->GetGlobalFunctionCount(); n++ )
	{
		int id = pEngine->GetGlobalFunctionIdByIndex(n);
		asIScriptFunction *func = pEngine->GetFunctionDescriptorById(id);

		// Skip the functions that start with _ as these are not meant to be called explicitly by the user
		if( func->GetName()[0] != '_' )
			cout << " " << func->GetDeclaration() << endl;
	}

	// List the user functions in the module
	asIScriptModule *mod = pEngine->GetModule("console");
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

void asConsole::PrintHelp() const
{
	cout << " exec [script]  - executes script statement" << endl;
	cout << " help           - this command" << endl;
	cout << " listfuncs      - list functions" << endl;
	cout << " listvars       - list variables" << endl;
	cout << " cls            - clear screen" << endl;
	cout << " quit           - end application" << endl;
}

void asConsole::CLS() const
{
	system("cls");
	ConsoleInfo();
}

void asConsole::InterpretLine(string& str)
{
	asVM* pVM = asVM::Instance();
	//pVM->AddSelectionToScript(str.c_str(),asBuildType::memory);

}

/*bool asConsole::IsFunction(string& str)
{
	asVM* pVM = asVM::Instance();

	string strToken;
	unsigned int pos = 0;
	asETokenClass asToken = pVM->GetToken(strToken,str,pos);

	bool bSuccess = false;

	if(asToken == asETokenClass::asTC_KEYWORD)
	{
		asToken = pVM->GetToken(strToken,str,pos);

		unsigned int iFindPos = str.find("=");

		if(iFindPos == string::npos)
		{
			iFindPos = str.find("()");

			if(iFindPos != string::npos)
			{
				bSuccess = true;
			}
		}
	}

	return bSuccess;
}
void asConsole::ExecuteScript()
{
	asVM* pVM = asVM::Instance();
	pVM->ExecuteScript(m_iScript);
	//pVM->RemoveScript(id);
}

};*/
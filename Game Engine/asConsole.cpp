
/*// Programmed By Bryce Mehring
#include "asConsole.h"
#include "asVM.h"
#include <iostream>

using namespace std;

namespace AngelScript
{

DWORD WINAPI ConsoleThread(void* parameter)
{

	asConsole* pConsole = asConsole::Instance();
	return pConsole->Run();

	// buffer that we will interpret

	cin.clear();
	system("cls");

	// Print some useful information and start the input loop
	cout << "Sample console using AngelScript " << asGetLibraryVersion() << " to perform scripted tasks." << endl;
	cout << "Type 'help' for more information." << endl;

	asVM* pVM = asVM::Instance();

	for(;;)
	{
		char buffer[1024];

		cout<<"Scripting Console. "<<endl<<"Enter Script: "<<endl;
		std::cin.getline(buffer,256,'!');

		cout<<"Output: "<<endl;

		unsigned int id = pVM->BuildScriptFromMemory(buffer);
		pVM->ExecuteScript(id);

	}
}

asConsole::asConsole() : m_bOpen(false)
{
	m_hConsole = GetConsoleWindow();
	ShowWindow( m_hConsole, SW_HIDE );

	// Create Thread
	m_ConsoleThread = CreateThread(NULL,NULL,ConsoleThread,NULL,CREATE_SUSPENDED,NULL);

	asVM* pVM = asVM::Instance();
//	m_iScript = pVM->CreateNewScript();
}
asConsole::~asConsole()
{
	CloseHandle(m_ConsoleThread);
}

void asConsole::Open()
{
	m_bOpen = true;

	ResumeThread(m_ConsoleThread);
	ShowWindow( m_hConsole, SW_SHOW );
}

void asConsole::Close()
{
	m_bOpen = false;

	ShowWindow( m_hConsole, SW_HIDE );
	SuspendThread(m_ConsoleThread);
}

DWORD asConsole::Run()
{
	ConsoleInfo();

	// Loop until the user quits
	for(;;)
	{
		string input , cmd, arg;
		input.resize(256);

		// Loop while the user uses '\' at the end of each line
		// This means that they have more code to add.
		
		//  \ ===== Get Input ===== /

		cout << "> ";
		cin.getline(&input[0], 256);

		//  / ===== Get Input ===== \

		// Trim unused characters
		input.resize(strlen(input.c_str()));

		int pos;
		if( (pos = input.find(" ")) != string::npos )
		{
			cmd = input.substr(0, pos);
			arg = input.substr(pos+1);
		}
		else
		{
			cmd = input;
			arg = "";
		}

		//if( cmd == "exec" )
		{
			//IsFunction(input);
		}
		//else
		{
			InterpretLine(input);
			ExecuteScript();
		}
		/*else if( cmd == "listfuncs" ) { ListFunctions(); }
		else if( cmd == "listvars" ) { ListVariables();  }
		else if(cmd == "cls") { CLS(); }
		else if( cmd == "quit" ) { break; }
		else if( cmd == "exec" ) { ExecuteScript(); }
		else { cout<<"Unknown Command"<<endl; }

	}

	return 0;
}

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

void asConsole::ConsoleInfo() const
{
	// Print some useful information
	cout << "AngelScript console: " << asGetLibraryVersion() << endl;
	cout << "Type 'config' for more information." << endl << endl;
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
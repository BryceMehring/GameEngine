
// Programmed By Bryce Mehring
#include "asConsole.h"
#include "asVM.h"
#include <iostream>

using namespace std;

DWORD WINAPI ConsoleThread(void* parameter)
{

	asConsole* pConsole = asConsole::Instance();
	return pConsole->Run();

	/*// buffer that we will interpret

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

	}*/
}

asConsole::asConsole() : m_bOpen(false)
{
	m_hConsole = GetConsoleWindow();
	ShowWindow( m_hConsole, SW_HIDE );

	// Create Thread
	m_ConsoleThread = CreateThread(NULL,NULL,ConsoleThread,NULL,CREATE_SUSPENDED,NULL);
}
asConsole::~asConsole() {}

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

	for(;;)
	{
		string input;
		input.resize(256);
		string cmd, arg;

		cout << "> ";
		cin.getline(&input[0], 256);

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

		// needs to append the string
		if(arg[arg.size() - 1] != '\\')
		{
			
			//ExecString(engine, arg);
			if( cmd == "help" )
			{
				PrintHelp();
			}
			else if( cmd == "listfuncs" )
			{
				ListFunctions();
			}
			else if( cmd == "listvars" )
			{
				ListVariables(); 
			}
			else if( cmd == "quit" )
			{
				break;
			}
			else if(cmd == "cls")
			{
				system("cls");
				ConsoleInfo();
			}
			else
			{
				cout << "Unknown command." << endl;
			}
		}
		else if( cmd == "exec")
		{
			// Interpret the command
			system("pause");
			
		}
	}

	return 0;
}

void asConsole::ListVariables()
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

void asConsole::ListFunctions()
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

void asConsole::PrintHelp()
{
	cout << " exec [script]  - executes script statement" << endl;
	cout << " help           - this command" << endl;
	cout << " listfuncs      - list functions" << endl;
	cout << " listvars       - list variables" << endl;
	cout << " cls            - clear screen" << endl;
	cout << " quit           - end application" << endl;
}

void asConsole::ConsoleInfo()
{
	// Print some useful information
	cout << "AngelScript console: " << asGetLibraryVersion() << endl;
	cout << "Type 'help' for more information." << endl << endl;
}
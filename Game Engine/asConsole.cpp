// Programmed By Bryce Mehring

#include "StdAfx.h"

#include "BEngine.h"
#include "asConsole.h"
#include "asVM.h"

using namespace std;

asConsole::asConsole(IBaseEngine* pEngine) : m_pEngine(pEngine), m_hConsole(GetConsoleWindow())
{
	ostringstream buffer;

	buffer << "AngelScript console: " << asGetLibraryVersion() ;

	SetConsoleTitle( buffer.str().c_str() );
	RegisterScript();
}
asConsole::~asConsole()
{
	Stop();
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
	m_pEngine->GetScriptVM().ExecuteScript(input.c_str());
}

void asConsole::RegisterScript()
{
	asIScriptEngine& scriptEngine = m_pEngine->GetScriptVM().GetScriptEngine();

	DBAS(scriptEngine.RegisterObjectType("asConsole",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(scriptEngine.RegisterObjectMethod("asConsole","void open()",asMETHOD(asConsole,Open),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterObjectMethod("asConsole","void close()",asMETHOD(asConsole,Close),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterGlobalProperty("asConsole console",this));
	
	scriptEngine.Release();
}
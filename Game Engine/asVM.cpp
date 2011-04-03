// Programmed by Bryce Mehring
#include "asVM.h"
#include <scriptstdstring.h>
#include <assert.h>
#include <fstream>
#include <iostream>

using namespace std;

#pragma comment(lib,"AngelScript.lib")

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


void print(int data)
{
	cout<<data<<endl;
}


asVM::asVM()
{
	int r;

	m_pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	RegisterStdString(m_pEngine);

	r = m_pEngine->SetMessageCallback(asFUNCTION(MessageCallback),0,asCALL_CDECL); assert( r >= 0 );

	r = m_pEngine->RegisterGlobalFunction("void Print(int)",asFUNCTION(0), asCALL_GENERIC); assert( r >= 0 );
	//r = m_pEngine->RegisterGlobalFunction("void Print(int)",asFUNCTIONPR(print, (int), void), asCALL_CDECL); assert( r >= 0 );

}
asVM::~asVM()
{
	m_pEngine->Release();
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
	return this->m_pEngine;
}

void asVM::ExecuteScript(unsigned int i)
{
	int r;

	assert(i < m_scripts.size());

	asIScriptContext* ptx = m_scripts[i].pCtx;

	r = ptx->Prepare(m_scripts[i].id); assert( r >= 0 );
	r = ptx->Execute(); assert( r >= 0 );
	r = ptx->Unprepare(); assert( r >= 0 );
}

unsigned int asVM::BuildScriptFromFile(const char* file)
{
	int r;

	r = m_builder.StartNewModule(m_pEngine,"TestModule");
	if( r < 0 ) 
	{
	  // If the code fails here it is usually because there
	  // is no more memory to allocate the module
	  printf("Unrecoverable error while starting a new module.\n");
	  system("pause");
	  return -1;
	}
	r = m_builder.AddSectionFromFile(file);
	if( r < 0 )
	{
	  // The builder wasn't able to load the file. Maybe the file
	  // has been removed, or the wrong name was given, or some
	  // preprocessing commands are incorrectly written.
	  printf("Please correct the errors in the script and try again.\n");
	  system("pause");
	  return -1;
	}
	r = m_builder.BuildModule(); 
	if( r < 0 )
	{
	  // An error occurred. Instruct the script writer to fix the 
	  // compilation errors that were listed in the output stream.
	  printf("Please correct the errors in the script and try again.\n");
	  system("pause");
	  return -1;
	}

	return AddScript();


}
unsigned int asVM::BuildScriptFromMemory(const char* buffer)
{
	m_builder.StartNewModule(m_pEngine,"TestModule");
	m_builder.AddSectionFromMemory(buffer);
	m_builder.BuildModule();

	return AddScript();
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

unsigned int asVM::AddScript()
{
	// temp structure to fill out and add to the vector of scripts
	Script script;

	asIScriptModule* pMod = m_pEngine->GetModule("TestModule");

	// fill structure
	script.id = pMod->GetFunctionIdByDecl("void main()");
	script.pCtx = m_pEngine->CreateContext();

	m_scripts.push_back(script);

	return (m_scripts.size() - 1);
}

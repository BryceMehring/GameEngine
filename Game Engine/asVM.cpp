// Programmed by Bryce Mehring
#include "asVM.h"
#include <scriptstdstring.h>
#include <assert.h>
#include <fstream>
#include <iostream>

using namespace std;

#pragma comment(lib,"AngelScript.lib")

struct Script
{
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

	r = m_pEngine->RegisterGlobalFunction("void Print(int)",asFUNCTIONPR(print, (int), void), asCALL_CDECL); assert( r >= 0 );

}
asVM::~asVM()
{
	m_pEngine->Release();
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
	r = ptx->Prepare(m_scripts[i].id);
	assert( r >= 0 );
	r = ptx->Execute(); assert( r >= 0 );

	r = ptx->Unprepare(); assert( r >= 0 );
}

void asVM::RegisterScript(const char* file)
{
	fstream inStream;
	inStream.open(file,ios::in);

	inStream.peek();

	if(inStream)
	{
		int length = 0;
		asETokenClass token = m_pEngine->ParseToken(file,0,&length);

		inStream.close();
	}
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

	Script script;

	asIScriptModule* pMod = m_pEngine->GetModule("TestModule");

	script.id = pMod->GetFunctionIdByDecl("void main()");
	script.pCtx = m_pEngine->CreateContext();

	m_scripts.push_back(script);

	return (m_scripts.size() - 1);
}

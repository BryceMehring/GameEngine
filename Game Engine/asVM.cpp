// Programmed by Bryce Mehring
#include "asVM.h"
#include <scriptstdstring.h>

// Implement a simple message callback function
void MessageCallback(const asSMessageInfo *msg, void *param)
{
	char buffer[200];
	const char *type = "ERR ";

	if( msg->type == asMSGTYPE_WARNING ) 
	{
		type = "WARN";
	}
	else if( msg->type == asMSGTYPE_INFORMATION ) 
	{
		type = "INFO";
	}

	sprintf_s(buffer,"%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}


asVM::asVM()
{
	m_pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	RegisterStdString(m_pEngine);

	m_pEngine->SetMessageCallback(asFUNCTION(MessageCallback),0,asCALL_CDECL);

}
asVM::~asVM()
{
	m_pEngine->Release();
}

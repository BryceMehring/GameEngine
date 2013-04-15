
#ifndef _ASREGISTERER_
#define _ASREGISTERER_

// macro functions

#define RegisterFactoryObjectL(CLASS,FACTORY,ENGINE) \
	DBAS(ENGINE.RegisterObjectBehaviour(#CLASS,asBEHAVE_FACTORY,#CLASS ## "@ f()", \
	asFUNCTION(FACTORY),asCALL_CDECL))
		

#define RegisterRefCountingObject(CLASS,engine) \
	DBAS(engine.RegisterObjectBehaviour(#CLASS,asBEHAVE_ADDREF,"void f()", \
		asMETHOD(CLASS,AddRef),asCALL_THISCALL)); \
	DBAS(engine.RegisterObjectBehaviour(#CLASS,asBEHAVE_RELEASE,"void f()", \
		asMETHOD(CLASS,Release),asCALL_THISCALL))

#define RegisterDowncastObject(child,parent,engine)  DBAS(engine.RegisterObjectBehaviour(#child,asBEHAVE_IMPLICIT_REF_CAST, \
#child ## "@ f()",asFUNCTION((EngineHelper::refCast<child,parent>)),asCALL_CDECL_OBJLAST))

		//#child ## "@ f()"

/*template< class T >
void RegisterRefCountingObject(asIScriptEngine& engine, const char* obj)
{
	DBAS(engine.RegisterObjectBehaviour(obj,asBEHAVE_ADDREF,"void f()",
		asMETHOD(T,AddRef),asCALL_THISCALL));
	DBAS(engine.RegisterObjectBehaviour(obj,asBEHAVE_RELEASE,"void f()",
		asMETHOD(T,Release),asCALL_THISCALL));
}*/


#endif // _ASREGISTERER_
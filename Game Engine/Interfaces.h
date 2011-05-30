// Programmed by Bryce Mehring
// 1/16/2011  to 3/10/2011

/*
	These interfaces are the base of the game engine. All of the different sections of the
	game engine will be developed as a DLL which will inherit from the IPlugin class.
*/

// guards
#ifndef _PLUGIN_INTERFACES_
#define _PLUGIN_INTERFACES_
#pragma once

#include <Windows.h>

// Macros
#ifdef PLUGIN_EXPORTS
#define PLUGINDECL __declspec(dllexport)
#else
#define PLUGINDECL __declspec(dllimport)
#endif


class RefCounting
{
public:

	/*
	The base class of all game engine objects. Reference Counting is a form of auto memory 
	management. To create a new object using reference counting, you use the normal way using
	c++'s new operator. Once the object(inherited from IRefCounting) is created, the counter 
	will start at one. When the pointer to the object is copied, AddRef must be called to
	increate the counter. Once the pointer is no longer needed, Release must be called to
	decrease the counter. Once the counter reaches 0, the object will be deleted. 
	*/

	// constructor sets the counter of references to 1
	RefCounting() : m_iCounter(1) {}

	// increases counter
	virtual void AddRef() { (++m_iCounter); }

	// decreases counter, if it reaches 0, the object will then be deleted
	virtual void Release()
	{
		if(--m_iCounter <= 0)
		{
			delete this;
		}
	}

protected:

	// protected destructor, forcing classes to use Release() to delete the object
	virtual ~RefCounting() {}

private:

	int m_iCounter;

};

enum DLLType
{
	Rendering,
	Input,
	// more dll types here
};

class IPlugin
{
public:
	

	/*
		Base Plug-in class. 
	*/

	friend class PluginManager;

	// returns the name of the plug-in
	virtual DLLType GetType() = 0;
	//virtual const VersionInfo& GetVersion() = 0;

	// displays a info box about the plug-in
	virtual void About() = 0;

protected:

	//IPlugin() {}
	virtual ~IPlugin() {}
	
};


/*class IPluinExplorter : public IPlugin
{
public:

	virtual ~IPluinExplorter() {}
	//virtual bool Export(Data* pData) = 0;

};

class IPluinImporter : public IPlugin
{
public:

	virtual ~IPluinImporter() {}
	//virtual bool Import(Data* pData) = 0;

};

class IGamePlugin : public IPlugin
{
public:

	virtual ~IGamePlugin() {}

	//....

};*/

// ===== graphics plugins =====

/*
	should the graphics rendering be implemented through a plugin?

	I think it should...because then the program could switch between DX9,DX10 and OpenGL
	at runtime, and it makes it really dynamic...



	
*/
class IRenderingPlugin : public IPlugin
{
public:

	// mesh functions
	/*virtual int LoadMesh(char* pFile) = 0;
	virtual void DrawMesh(int id, int x, int y, int z) = 0;

	//shaders
	virtual int LoadShader(char* pFile) = 0;
	*/
	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Present() = 0;
	//virtual void DrawSprite();

	// Fonts
	virtual void GetStringRec(const char* str, RECT& out) = 0;
	virtual void DrawString(const char* str, RECT& R, DWORD color) = 0;
	
	///add more functions...
protected:

	virtual ~IRenderingPlugin() {}

};

class IInputPlugin : public IPlugin
{
public:

	virtual void Poll() = 0;

protected:

	virtual ~IInputPlugin() {}

};

// Keyboard mouse
class IKMInput : public IInputPlugin
{
public:

	virtual bool KeyDown(char Key) = 0;
	virtual bool MouseClick(int Button) = 0;

	virtual void MousePos(POINT&) = 0;
	virtual int MouseX() = 0;
	virtual int MouseY() = 0;
	virtual int MouseZ() = 0;

protected:

	virtual ~IKMInput() {}

};

class IJoystickInput : public IInputPlugin
{
protected:

	virtual ~IJoystickInput() {}

	// more pure virtual functions here

};

// Incomplete
class ICollisionPlugin : public IPlugin
{
public:

	virtual void Test() = 0;

protected:

	virtual ~ICollisionPlugin() {}


};

class PluginManager;

typedef IPlugin* (*CREATEPLUGIN)(PluginManager& mgr);

extern "C" PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr);

#endif
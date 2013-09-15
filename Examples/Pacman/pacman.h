#pragma once

#ifndef _PACMAN_
#define _PACMAN_

#define PLUGIN_EXPORTS
#include "PluginManager.h"
#include "IKMInput.h"
#include "GameStateScript.h"
#include "RTTI.h"
#include "QuadTree.h"
#include "Camera.h"


class Pacman : public GameStateScript
{
public:

	RTTI_DECL;

	// returns the name of the plug-in
	virtual DLLType GetPluginType() const { return DLLType::GamePlugin; }
	virtual const char* GetName() const { return "Pacman"; }
	virtual int GetVersion() const { return 0; }

	// displays a info box about the plug-in
	virtual void About() const {}

	virtual void Init(class Game& game);
	virtual void Destroy(class Game& game);

	virtual void Update(class Game&);
	virtual void Draw(class Game&);

private:

	Pacman(PluginManager&);
	virtual ~Pacman();

	std::vector<string> m_tiles;

	int m_iSelectedSprite;

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager&); 

};

#endif // _PACMAN_
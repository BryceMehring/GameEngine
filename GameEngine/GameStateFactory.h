#ifndef _GAMESTATEFACTORY_
#define _GAMESTATEFACTORY_

#include "GameState.h"
#include "IStateCreator.h"
#include "Singleton.h"
#include <map>
#include <vector>

class GameStateFactory : public Singleton<GameStateFactory>
{
public:

	friend class Singleton<GameStateFactory>;

	typedef std::vector<std::string> KeyStruct;

	bool Register(const std::string& name, IStateCreator* pCreator);
	IGameState* CreateState(const std::string& name);

	const KeyStruct& GetKeys() const;

private:

	GameStateFactory() {}

	typedef std::map<std::string,IStateCreator*> CreatorMap;
	
	CreatorMap m_Creators;
	KeyStruct m_Keys;

};

#endif _GAMESTATEFACTORY_
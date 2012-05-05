#include "GameStateFactory.h"
#include <algorithm>

bool GameStateFactory::Register(const std::string& name, IStateCreator* pCreator)
{
	auto iter = m_Creators.find(name);

	if(iter != m_Creators.end())
	{
		//delete pCreator;
		return false;
	}

	m_Creators[name] = pCreator;
	m_Keys.push_back(name);

	return true;
}

const GameStateFactory::KeyStruct& GameStateFactory::GetKeys() const { return m_Keys; }


IGameState* GameStateFactory::CreateState(const std::string& name)
{
	auto iter = m_Creators.find(name);

	if(iter == m_Creators.end()) { return nullptr; }

	return iter->second->Create();
}
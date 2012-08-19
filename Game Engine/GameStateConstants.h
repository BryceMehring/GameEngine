#ifndef _GAMESTATECONSTANTS_
#define _GAMESTATECONSTANTS_

#include <hash_map>


template< class T >
class GameStateConstants
{
public:

	bool GetConstant(const std::string& str, T& out) const
	{
		auto iter = m_constants.find(str);
		bool success = (iter != m_constants.end());

		if(success)
		{
			out = *(iter->second);
		}

		return success;
	}
	bool AddConstant(const std::string& str, T& c)
	{
		return m_constants.insert(make_pair(str,&c)).second;
	}

	void Clear()
	{
		m_constants.clear();
	}

private:

	stdext::hash_map<std::string,T*> m_constants;

};

#endif //_GAMESTATECONSTANTS_
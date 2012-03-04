//#include "StdAfx.h"
#include <Windows.h>
#include <fstream>
#include <algorithm>

#include "Game.h"
#include "GameOfLifeFactory.h"

#pragma comment(lib,"Game Engine.lib")

using namespace std;

const int MAX_NUM = 100;

/*class Unit : public ISpatialObject
{
public:

	virtual const KEY& GetPos() const
	{
		return m_pos;
	}
	virtual Type GetType() const
	{
		return Type::Unit;
	}
	void SetPos(const KEY& P)
	{
		m_pos = P;
	}

	void Move()
	{
		if(m_pNode)
		{
			const Node::LIST_DTYPE* pNearObjs = m_pNode->GetNearObjects();
			for_each(pNearObjs->begin(),pNearObjs->end(),[&](ISpatialObject* pObj)
			{
				if(pObj != this)
				{
					if(pObj->GetType() == ISpatialObject::Unit)
					{
						Unit* pUnit = static_cast<Unit*>(pObj);
						cout<<pNearObjs->size()<<endl;
					}
				}
			});
		}

		int scalar = 10;
		m_pos.x += scalar;
		//m_pos.y -= scalar;
	}

private:

	KEY m_pos;

};*/

bool isPrime(unsigned long long num)
{
	bool success = true;
	for(unsigned long long i = 2; (i*i) <= num; i++)
	{
		if((num % i) == 0)
		{
			success = false;
			break;
		}
	}
	return success;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Game myGame(hInstance);
	myGame.SetStateFactory(new GameOfLifeFactory);
	myGame.SetState(GAME);
	return myGame.PlayGame();
}

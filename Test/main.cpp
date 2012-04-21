//#include "StdAfx.h"
#include <Windows.h>
#include <fstream>
#include <algorithm>

#include "Game.h"
#include "GameOfLifeFactory.h"

#pragma comment(lib,"Game Engine.lib")

using namespace std;

const int MAX_NUM = 100;

class RTTI
{
public:

	RTTI(const string& name) : m_name(name)
	{
	}

	const string& GetName() const { return m_name; }

private:

	string m_name;
};

class IBase
{
public:

	virtual const RTTI* RTTI() const = 0;
	virtual void Update(void* p) = 0;

	// ...?

};

class MyClass : public IBase
{
public:

	static const ::RTTI s_rtti;

	virtual const ::RTTI* RTTI() const { return &s_rtti; }
	virtual void Update(void* p)
	{
		string str = *((string*)p);
		cout<<str<<endl;
	}

};

const RTTI MyClass::s_rtti("MyClass");

class AnotherClass : public IBase
{
public:

	static const ::RTTI s_rtti;

	virtual const ::RTTI* RTTI() const { return &s_rtti; }
	virtual void Update(void* p)
	{
		int n = *((int*)p);
		cout<<n<<endl;
	}

};

const RTTI AnotherClass::s_rtti("AnotherClass");



class TestObj
{
public:

	TestObj()
	{
		for(int i = 0; i < 10; ++i)
		{
			m_list.push_back(i);
		}
	}

	void Loop()
	{
		for(std::list<int>::iterator iter = m_list.begin(); iter != m_list.end(); ++iter)
		{
			if(*iter > 5)
			{
				delete this;
			}
		}
	}

private:

	std::list<int> m_list;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// messing around
	/*int i = 0;
	int j = 0;
	while(i <= 50)
	{
		do
		{
			++j;
		} while(j <= 50);

		++i;
	}*/


	//TestObj* pObj = new TestObj;
	//pObj->Loop();

	// The message would come in, based off of the type of the message
	// we can polymorphically create an object that responds to that message
	// The classes that would be generated would be derived from IBase

	/*string name = "Bryce";
	IBase* pClass = new MyClass;
	IBase* pClass3 = new MyClass;
	IBase* pClass2 = new AnotherClass;

	int n = 1337;

	bool bSameType2 = pClass->RTTI() == &AnotherClass::s_rtti;
	bool bSameType = pClass->RTTI() == pClass3->RTTI();

	pClass->Update(&name);
	pClass2->Update(&n);

	delete pClass;
	delete pClass3;
	delete pClass2;

	srand((unsigned int)time(0));
	Dice* pDice = new SDice(0,50);

	const int size1 = sizeof(Dice);
	const int size2 = sizeof(SDice);

	/*int i = 0;
	while(i < 10)
	{
		pDice->Roll();
		++i;
	}

	delete pDice;

	void const*  p = nullptr;

	const DiceStats& stats = SDice::GetDiceStats();
	int firstRoll = stats.GetFirstRoll();*/
	//stats.UpdateRoll(1337); // this fails to compile because we have a reference to a const obj
	
	Game myGame(hInstance);
	myGame.SetStateFactory(new GameOfLifeFactory);
	myGame.SetState(GAME);
	myGame.SetState(MENU);
	return myGame.PlayGame();
}

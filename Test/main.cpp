//#include "StdAfx.h"
#include <Windows.h>
#include <fstream>
#include <algorithm>

#include "WindowManager.h"
#include "QuadTree.h"
#include "Timer.h"
#include "RefCounting.h"
#include "Delegates.h"
#include "Singleton.h"
#include "Menu.h"
#include "FileManager.h"
#include "GameOfLife.h"
#include "IRenderer.h"

#define SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#pragma comment(lib,"Game Engine.lib")

using namespace std;

const int MAX_NUM = 100;

void LoadAndSort(const char* pFile);

class Unit : public ISpatialObject
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

};

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
	GameOfLife myGame(hInstance);

	PluginManager& pm = PluginManager::Instance();
	IRenderer* pRenderer = static_cast<IRenderer*>(pm.GetPlugin(Rendering));

	pRenderer->About();

	pRenderer->Begin();
	POINT P = {50,50};
	pRenderer->DrawString("Hello World",P,0xffff0000);
	pRenderer->End();
	pRenderer->Present();

	myGame.PlayGame();
	//pRenderer->DrawString("Hello World",

	FileManager& fm = ::FileManager::Instance();
	fm.WriteToLog("Hello World log Test\n");
	fm.WriteToLog("An Error has occured");

	Menu* pMenu = new Menu("Main Menu");
	Menu* pOptions = new Menu("Options");
	Menu* pGame = new Menu("Game");

	pMenu->AddMenu(pOptions);
	pMenu->AddMenu(pGame);

	UI ui;
	ui.SetMenu(pMenu);

	//LoadAndSort(fm.OpenFileName().c_str());

	/*srand((unsigned int)time(NULL));
	std::vector<Unit*> Units;
	RECT R = {0,0,1000,1000};
	QuadTree myTree(R);

	FileManager& fm = FileManager::Instance();
	string file = fm.OpenFileName();

	for(int i = 0; i < 200; ++i)
	{
		Unit* pUnit = new Unit();
		KEY P = {rand()%1000,rand()%1000};

		pUnit->SetPos(P);
		Units.push_back(pUnit);
		myTree.Insert(pUnit);
	}

	//while(true)
	{
		Units[0]->Move();
		myTree.Update();
	}

	/*

	MyObj* pObj = (MyObj*)::operator new(sizeof(MyObj));
	new (pObj) MyObj;

	//Timer theTimer;

	unsigned int counter = 0;
	double total = 0;
	
	//Timer theClock;
	
	

	RECT R = {0,0,MAX_NUM,MAX_NUM};

	//theClock.Start();
	

	//while(true)
		//theTimer.Start();

		QuadTree tree(R);

		for(int i = 0; i < 20; ++i)
		{
			POINT a = {0,0};

			a.x = rand() % MAX_NUM;
			a.y = rand() % MAX_NUM;

			tree.AddPoint(a);
		}

	//	theTimer.Stop();


		/*auto theTime = theTimer.GetTimeInMilliseconds();
		theTimer.Reset();
		total += theTime;
		double avg = total / ++counter;

		if(theClock.GetTimeInSeconds() >= 1)
		{
			cout<<"Time: "<< avg << endl;
			theClock.Start();
		}


	string file("QuadTreeTest");
	tree.SaveToFile(file);
	
	while(true)
	{
		POINT newPoint = {rand() % (MAX_NUM*2),rand() % (MAX_NUM*2)};
	
		std::list<KEY> data;
		
		cout<<"Point: "<<newPoint.x << " " << newPoint.y << endl;
		
		if(tree.FindNearPoint(newPoint,data))
		{
			cout<<"Near Points: " << endl;

			for(std::list<KEY>::iterator iter = data.begin(); iter != data.end(); ++iter)
			{
				cout << iter->x << " " << iter->y << endl;

				if(iter->x > 0)
				{
					iter->x += 5000;
				}
			}
		}

		tree.Update();

		cin.get();
		//system("cls");
	}
	// {35FA7712-8FEB-4925-880E-D4A44C659BAE}

	LoadAndSort("..//Names//Names.txt");
	cin.get();*/
	return 0;
}

/*void LoadAndSort(const char* pFile)
{
	std::fstream in(pFile,ios::in);

	if(!in.is_open())
		return;

	std::list<string> names;

	while(!in.eof())
	{
		string line;
		getline(in,line);
		names.push_back(line);
	}

	names.sort([](const string& a,const string& b) -> bool
	{
		return std::lexicographical_compare(a.begin(),a.end(),b.begin(),b.end());
	});

	in.close();

	for(std::list<string>::iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		cout<<*iter<<endl;
	}

	system(pFile);

}*/
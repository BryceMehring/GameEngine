#include "Game.h"

#include <iostream>
#include <ctime>
#include "RefCounting.h"

using namespace std;

struct MemChecker
{
    ~MemChecker()
    {
        cout<<"Alloc: " <<RefCounting::GetAddRefCount()<<endl;
        cout<<"Dealloc: " <<RefCounting::GetAddReleaseCount()<<endl;
        cout<<"Memory Leak: "<<(RefCounting::GetAddRefCount() != RefCounting::GetAddReleaseCount())<<endl;
    }
};

void Function(int a)
{
	cout<<a<<endl;
}



int main(int size, char** cmd)
{
    /*if(size < 2)
	{
		cout<<"Invalid Number of command line arguments"<<endl;
		cin.get();
		return 1;
    }*/

    MemChecker checker;
    const char* pState = "FlockingAlgorithm";

	//unsigned int seed = FileManager::Instance().GetSeedFromLog();

	// seed the random number generator 
	srand (time(0));

    Game myGame;
    myGame.SetNextState(pState);
    return myGame.Run();
}

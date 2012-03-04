#ifndef _MENU_
#define _MENU_

#include "Delegates.h"
#include "DxPolygon.h"
#include "IRender.h"
#include "IKMInput.h"
#include "QuadTree.h"

class IUIElement
{
public:

	virtual ~IUIElement() {}

	virtual void foo() = 0;
	virtual bool Update() = 0;

private:
	
};

class TextBox : public IUIElement
{
public:
};

class Button : public IUIElement
{
public:

	virtual bool Update()
	{
		// ... do interesting stuff here
		// this is triggered
		if(true)
		{
			m_callback.Call();
		}
		return true;
	}

private:

	Delegate<void,void> m_callback;
};

class UIMenuButton : public IUIElement
{
public:

private:



};

class Mouse : public ISpatialObject
{
public:

private:

	POINT m_pos;
};


// Todo: change the name of this class?
class Menu : public IRender // todo: could inherit from the graph node class
{
public:

	typedef Delegate<void,void> callback;

	Menu();
	Menu(const std::string& str);
	~Menu();

	void SetMenuTitle(const std::string& str);

	void AddMenu(Menu* pMenu);

	void GetTriggered(class UI* pUI);
	bool IsTriggered();
	void Update();

	virtual void Render();

private:

	// data members
	std::string m_menuTitle;
	std::vector<Menu*> m_SubMenu;
	std::vector<IUIElement*> m_elements;
	Menu * m_pPrev;
	DxPolygon* m_pPolygon;
};

class UI
{
public:

	explicit UI(Menu* pMenu = nullptr);

	void SetMenu(Menu* pMenu);

	void Update();

	// the UI would control the changing between Menu's
	// What if the mouse was an object that was being tracked in the quadtree
	// And then whenever the mouse was clicked, its observers would be updated

private:

	//Mouse* m_pMouse; // the mouse is in the QuadTree
//	QuadTree* m_pQuadTree;
	Menu* m_pMenu; // this could be useful in game as well as in a real menu 
};

#endif
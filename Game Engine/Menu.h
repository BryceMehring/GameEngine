#ifndef _MENU_
#define _MENU_

#include "Delegates.h"
#include "DxPolygon.h"
#include "IRender.h"
#include "QuadTree.h"

class IUIElement : public IRender
{
public:

	virtual ~IUIElement() {}

	virtual void Update(class IKMInput* pInput) = 0;
	
};

struct Text
{
	std::string name;
	POINT P;
};

class TextBox : public IUIElement
{
public:
};


// Todo: change the name of this class?
class Menu : public IRender // todo: could inherit from the graph node class
{
public:

	typedef Delegate<void,void> callback;

	Menu();
	Menu(const std::string& str);
	~Menu();

	void SetMenuTitle(const std::string& str,const POINT& P);
	void SetPolygon(DxPolygon* pPolygon);

	void AddMenu(Menu* pMenu);
	void AddElement(IUIElement* pElement);

	void Update(class UI* pUI, class IKMInput* pInput);

	virtual void Render(class IRenderer* pRenderer);

private:

	// data members
	POINT m_point;
	std::string m_menuTitle;
	std::vector<Menu*> m_SubMenu;
	std::vector<IUIElement*> m_elements;
	Menu * m_pPrev;
	DxPolygon* m_pPolygon;
};

// todo: this class should inherit from an abstract base interface class called: IRender
class UI
{
public:

	explicit UI(Menu* pMenu = nullptr);
	~UI();

	void SetMenu(Menu* pMenu);

	// The ability to treat pointers or references of objects of the same inheritance 
	// hierarchy tree as the parent class. 
	void Update(class IKMInput* pInput);
	void Render(class IRenderer* pRenderer);

	// the UI would control the changing between Menu's
	// What if the mouse was an object that was being tracked in the quadtree
	// And then whenever the mouse was clicked, its observers would be updated

private:

	//Mouse* m_pMouse; // the mouse is in the QuadTree
//	QuadTree* m_pQuadTree;
	Menu* m_pMenu; // this could be useful in game as well as in a real menu 
};

class ButtonBase : public IUIElement
{
public:

	ButtonBase(const Text& name, DxPolygon* pPolygon);

	virtual void Render(class IRenderer* pRenderer);

protected:

	Text m_name;
	DxPolygon* m_pPolygon;

	bool IsClicked(class IKMInput* pInput) const;

};

template< class T >
class GenericButton : public ButtonBase
{
public:

	typedef Delegate<void,T> DELEGATE;

	GenericButton(const Text& name,DELEGATE callback, const T& type, DxPolygon* pPolygon) : ButtonBase(name,pPolygon), m_callback(callback), m_type(type)
	{
	}

	virtual void Update(class IKMInput* pInput)
	{
		if(IsClicked(pInput))
		{
			m_callback.Call(m_type);
		}
	}

private:

	DELEGATE m_callback;
	T m_type;
};

template<>
class GenericButton<void> : public ButtonBase
{
public:

	typedef Delegate<void,void> DELEGATE;

	GenericButton(const Text& name, DELEGATE callback, DxPolygon* pPolygon) : ButtonBase(name,pPolygon), m_callback(callback)
	{
	}

	virtual void Update(class IKMInput* pInput)
	{
		if(IsClicked(pInput))
		{
			m_callback.Call();
		}
	}

private:

	DELEGATE m_callback;
};

class Mouse : public ISpatialObject
{
public:

private:

	POINT m_pos;
};

#endif
#ifndef _MENU_
#define _MENU_

#include "Delegates.h"
#include "DxPolygon.h"
#include "IRender.h"
#include "QuadTree.h"

// todo: should this be a global function or should I put this into an interface?
extern bool IsClicked(class IKMInput* pInput, DxPolygon* pPolygon);

class IUIElement : public IRender
{
public:

	virtual ~IUIElement() {}

	virtual void Update(class IKMInput* pInput) = 0;

protected:

	class Menu* m_pMenu;
	
};

struct Text
{
	std::string name;
	POINT P;
};


// Todo: change the name of this class?
class Menu : public IRender // todo: could inherit from the graph node class
{
public:

	Menu();
	//Menu(const std::string& str);
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

	ButtonBase() {}
	ButtonBase(const Text& name, DxPolygon* pPolygon);

	virtual void Render(class IRenderer* pRenderer);

protected:

	Text m_name;
	DxPolygon* m_pPolygon;

};

template< class T >
class GenericButton : public ButtonBase
{
public:

	typedef Delegate<void,T> DELEGATE;

	GenericButton() {}
	GenericButton(const Text& name,DELEGATE callback, const T& type, DxPolygon* pPolygon) : ButtonBase(name,pPolygon), m_callback(callback), m_type(type)
	{
	}

	void SetName(const Text& name) { m_name = name; }
	void SetCallback(const DELEGATE& callback, const T& arg)
	{ 
		m_callback = callback;
		m_type = arg;
	}
	void SetPolygon(DxPolygon* pPolygon) { m_pPolygon = pPolygon; }
	
	
	virtual void Update(class IKMInput* pInput)
	{
		if(IsClicked(pInput,m_pPolygon))
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
		if(IsClicked(pInput,m_pPolygon))
		{
			m_callback.Call();
		}
	}

private:

	DELEGATE m_callback;
};

template< class T >
class SquareButton : public GenericButton<T>
{
public:

	SquareButton(const RECT& R, const std::string& name)
	{
		Text text = {name,{R.left,R.top}};
		this->SetPolygon(new DxSquare(R));
		this->SetName(text);
	}

private:

};

/*class TextBox : public IUIElement
{
public:

	TextBox(const std::string& name, const RECT& R);

	void Write(const std::string& line, DWORD color = 0xffffffff);

	virtual void Update(IKMInput* pInput);
	virtual void Render(IRenderer* pRenderer);

	void CLS();

	void SaveToFile(const std::string& file) const;

protected:

	struct LineData
	{
		std::string line;
		D3DCOLOR color;
		RECT R;
		int height;
	};

//private:

	// ===== Data members =====

	// todo: need to organize

	typedef std::vector<LineData> TextDataType;
	TextDataType m_text;

	DxSquare m_square;

	unsigned int m_spacePos;

	// todo: I could move the carrot into a class
	// carrot
	POINT m_carrotPos;
	bool m_drawCarrot;

	float m_fCarrotTime;
	float m_fScrollTime;

	int m_scrollSpeed;

	// ===== helper functions =====
protected:
	virtual void Backspace();
	virtual void Enter();
	virtual void AddKey(char Key); 
private:

	// todo: passing around these interfaces seems redundant
	void UpdateScrolling(class IKMInput* pInput);
	void UpdateCarrot(class IKMInput* pInput);
	void UpdateInput(class IKMInput* pInput);
	void UpdateTextInput(class IKMInput* pInput);

	void ScrollDown(int speed);
	void ScrollUp(int speed);
	void ScrollStop();

};*/

class Mouse : public ISpatialObject
{
public:

private:

	POINT m_pos;
};

#endif
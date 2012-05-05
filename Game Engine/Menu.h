#ifndef _MENU_
#define _MENU_



#include "Delegates.h"
#include "DxPolygon.h"
#include "IRender.h"
#include "GameState.h"
//#include "CreatorId.h"


#include "RTTI.h"


// todo: should this be a global function or should I put this into an interface?
bool IsClicked(class IKMInput* pInput, DxPolygon* pPolygon);

class IUIElement : public IRender
{
public:

	virtual ~IUIElement() {}

	virtual void Select() = 0;
	virtual void Enter() = 0;
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
class Menu : public IRender
{
public:

	RTTI_DECL;

	// todo: make this thread safe

	Menu();
	//Menu(const std::string& str);
	~Menu();

	virtual void Update(class GUI* pGUI, class IKMInput* pInput);
	virtual void Render(class IRenderer* pRenderer);

	void SetMenuTitle(const std::string& str,const POINT& P);
	void SetPolygon(DxPolygon* pPolygon);

	void AddMenu(Menu* pMenu);
	void AddElement(IUIElement* pElement);

private:

	// data members
	POINT m_point;
	std::string m_menuTitle;
	std::vector<IUIElement*> m_elements;
	std::vector<Menu*> m_menus;
	Menu * m_pPrev;
	DxPolygon* m_pPolygon;
};


// todo: this class should traverse the menu graph
class GUI : public IRender
{
public:

	friend class Menu;

	typedef Delegate<void,Menu*> ChangeMenuCallback;

	explicit GUI(Menu* pMenu = nullptr);
	~GUI();

	ChangeMenuCallback CreateCallback();

	void SetMenu(Menu* pMenu);

	void Update(class IKMInput* pInput);
	void Render(class IRenderer* pRenderer);

private:

	Menu* m_pMenu;
	unsigned int m_uiCurrentIndex;

};

class ButtonBase : public IUIElement
{
public:

	ButtonBase() : m_color(-1) {}
	ButtonBase(const Text& name, DxPolygon* pPolygon);
	virtual ~ButtonBase();

	virtual void Render(class IRenderer* pRenderer);
	virtual void Update(class IKMInput* pInput);

	virtual void Select()
	{
		m_pPolygon->SetColor(m_pPolygon->GetColor()*24);
	}

	void SetPolygon(DxPolygon* pPolygon) { m_pPolygon = pPolygon; }
	void SetPolygonColor(DWORD color) { m_pPolygon->SetColor(color); }
	void SetTextColor(DWORD color) { m_color = color; }
	void SetName(const Text& name) { m_name = name; }

protected:

	Text m_name;
	DWORD m_color;
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

	void SetCallback(const DELEGATE& callback)
	{ 
		m_callback = callback;
	}
	void SetArg(const T& arg)
	{
		m_type = arg;
	}
	
	virtual void Enter()
	{
		m_callback.Call(m_type);
	}
	virtual void Update(class IKMInput* pInput)
	{
		ButtonBase::Update(pInput);

		if(IsClicked(pInput,m_pPolygon))
		{
			m_callback.Call(m_type);
		}
	}

private:

	DELEGATE m_callback;
	T m_type;
};

template< class T >
class GenericButton<T&> : public ButtonBase
{
public:

	typedef Delegate<void,T&> DELEGATE;

	GenericButton() {}
	GenericButton(const Text& name,DELEGATE callback, const T& type, DxPolygon* pPolygon) : ButtonBase(name,pPolygon), m_callback(callback), m_type(type)
	{
	}

	void SetCallback(const DELEGATE& callback)
	{ 
		m_callback = callback;
	}
	void SetArg(const T& arg)
	{
		m_type = arg;
	}
	
	virtual void Enter()
	{
		m_callback.Call(m_type);
	}
	virtual void Update(class IKMInput* pInput)
	{
		ButtonBase::Update(pInput);

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

	GenericButton()
	{
	}
	GenericButton(const Text& name, DELEGATE callback, DxPolygon* pPolygon) : ButtonBase(name,pPolygon), m_callback(callback)
	{
	}

	void SetCallback(const DELEGATE& callback)
	{ 
		m_callback = callback;
	}

	virtual void Enter()
	{
		m_callback.Call();
	}
	virtual void Update(class IKMInput* pInput)
	{
		ButtonBase::Update(pInput);

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

#endif
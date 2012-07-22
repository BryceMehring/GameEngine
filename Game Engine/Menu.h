#ifndef _MENU_
#define _MENU_

// todo: split this header file down

#include "Delegates.h"
#include "DxPolygon.h"
#include "IRender.h"
#include "GameState.h"

#include "RTTI.h"


// todo: should this be a global function or should I put this into an interface?
bool IsClicked(class IKMInput* pInput, DxPolygon* pPolygon);

class IUIElement : public IRender
{
public:

	virtual ~IUIElement() {}

	virtual void Select() = 0;
	virtual void Deselect() = 0;
	virtual void Enter() = 0;
	virtual void Update(class IKMInput* pInput, double dt) = 0;

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

	virtual void Update(class GUI* pGUI, class IKMInput* pInput, double dt);
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

	void Update(class IKMInput* pInput, double dt);
	void Render(class IRenderer* pRenderer);

private:

	Menu* m_pMenu;
	unsigned int m_uiCurrentIndex;
	bool m_bSetMenu;

};

// Buttons
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
		m_pPolygon->SetColor(0xffff0000);
	}
	virtual void Deselect()
	{
		m_pPolygon->SetColor(0xffffffff);
	}

	void SetPolygon(DxPolygon* pPolygon) { m_pPolygon = pPolygon; }
	void SetPolygonColor(DWORD color) { m_pPolygon->SetColor(color); }
	void SetTextColor(DWORD color) { m_color = color; }
	void SetName(const Text& name) { m_name = name; }

protected:

	Text m_name;
	DWORD m_color;
	DxPolygon* m_pPolygon;


	bool m_bMouseHover;

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
	virtual void Update(class IKMInput* pInput, double)
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
	virtual void Update(class IKMInput* pInput, double)
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
	virtual void Update(class IKMInput* pInput, double)
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

// A basic ui textbox
class TextBox : public IUIElement
{
public:

	TextBox(const std::string& name, const RECT& R);

	// Enters a new line
	void Write(const std::string& line, DWORD color = 0xffffffff, bool bContinue = false);

	virtual void Update(IKMInput* pInput, double dt);
	virtual void Render(IRenderer* pRenderer);
	
	// todo: clears the buffer
	void CLS();

	// todo: Save text buffer to file
	void SaveToFile(const std::string& file) const;

	// todo: need to implement
	virtual void Select() {}
	virtual void Deselect() {}

protected:

	struct LineData
	{
		LineData() {}
		LineData(const std::string& str, D3DCOLOR color, const POINT& P,bool c)
		: line(str), color(color), P(P), bContinue(c)
		{

		}

		std::string line;
		D3DCOLOR color;
		POINT P;
		bool bContinue;
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
	double m_fScrollTime;

	float m_scrollAccel;
	float m_scrollSpeed;

	// ===== helper functions =====
protected:
	virtual void Backspace();
	virtual void Enter();
	virtual void AddKey(char Key); 
private:

	// todo: passing around these interfaces seems redundant
	void UpdateScrolling(class IKMInput* pInput, double dt);
	void UpdateCarrot(class IKMInput* pInput);
	void UpdateInput(class IKMInput* pInput);
	void UpdateTextInput(class IKMInput* pInput);

	unsigned int GetNumberOfRowsInLine(const std::string&) const;

};

class ScriptingConsole : public TextBox
{
public:

	ScriptingConsole(asVM* pVM, const std::string& name, const RECT& R);

	virtual void Enter();
	virtual void Backspace();

	void MessageCallback(const asSMessageInfo *msg);

	void RegisterScript();

protected:

	asVM* m_pVM;

	unsigned int m_uiStartIndex;

	// helper functions

	bool IsBlocked() const; 

	template< class T >
	void _Grab(const T& p)
	{
		stringstream stream;
		stream << p;

		Write(stream.str(),0xff00ffff);
	}

	// These are registered with AngelScript
	void Grab();
	void Grab(const std::string&);
	void Grab(int);
	void Grab(float);
	void Grab(double);
	void Grab(unsigned int);
	void Grab(bool);

};



#endif
#ifndef _MENU_
#define _MENU_

// todo: split this header file down

#include "Delegates.h"
#include "IKMInput.h"
#include "IRender.h"
#include "GameState.h"
#include "Sprite.h"

#include "RTTI.h"

class IUIElement : public IRender
{
public:

	virtual ~IUIElement() {}

	virtual void Select() = 0;
	virtual void Deselect() = 0;
	virtual void Trigger() = 0;

	virtual void Update(class IKMInput&, double dt) = 0;

protected:

	class Menu* m_pMenu;
	
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

	virtual void Update(class GUI* pGUI, class IKMInput& input, double dt);
	virtual void Render(class IRenderer& renderer);

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


class GUI : public IRender
{
public:

	friend class Menu;

	typedef Delegate<void,Menu*> ChangeMenuCallback;

	explicit GUI(Menu* pMenu = nullptr);
	~GUI();

	ChangeMenuCallback CreateCallback();

	void SetMenu(Menu* pMenu);
	Menu* GetMenu();

	void Update(class IKMInput& input, double dt);
	void Render(class IRenderer& renderer);

private:

	Menu* m_pMenu;
	unsigned int m_uiCurrentIndex;
	bool m_bSetMenu;

};

// Buttons
class ButtonBase : public IUIElement
{
public:

	ButtonBase(const Math::Sprite& s, const std::string& str);
	virtual ~ButtonBase();

	virtual void Render(class IRenderer&);
	virtual void Update(class IKMInput&);

	virtual void Select()
	{
		//m_pPolygon->SetColor(0xffff0000);
	}
	virtual void Deselect()
	{
		//m_pPolygon->SetColor(0xffffffff);
	}

protected:

	Math::Sprite m_sprite;
	std::string m_text;
	bool m_bMouseHover; 

};

template< class T >
class GenericButton : public ButtonBase
{
public:

	typedef Delegate<void,T> DELEGATE;

	GenericButton(const Math::Sprite& s, const std::string& str) : ButtonBase(s,str) {}
	GenericButton(const Math::Sprite& s, const std::string& str, const DELEGATE& callback, const T& type) : ButtonBase(s,str), m_callback(callback), m_type(type)
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
	
	virtual void Trigger()
	{
		m_callback.Call(m_type);
	}
	virtual void Update(IKMInput& input, double)
	{
		ButtonBase::Update(input);

		if(input.MouseClick(0))
		{
			const ::D3DXVECTOR2& pos = input.GetTransformedMousePos();

			if(m_sprite.IsPointWithin(pos))
			{
				Trigger();
			}
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

	GenericButton(const Math::Sprite& s, const std::string& str) : ButtonBase(s,str) {}
	GenericButton(const Math::Sprite& s, const std::string& str, const DELEGATE& callback, const T& type) : ButtonBase(s,str), m_callback(callback), m_type(type)
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
	
	virtual void Trigger()
	{
		m_callback.Call(m_type);
	}
	virtual void Update(IKMInput& input, double)
	{
		ButtonBase::Update(input);

		if(input.MouseClick(0))
		{
			const ::D3DXVECTOR2& pos = input.GetTransformedMousePos();

			if(m_sprite.IsPointWithin(pos))
			{
				Trigger();
			}
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

	GenericButton(const Math::Sprite& s, const std::string& str) : ButtonBase(s,str) {}
	GenericButton(const Math::Sprite& s, const std::string& str, const DELEGATE& callback) : ButtonBase(s,str), m_callback(callback)
	{
	}

	void SetCallback(const DELEGATE& callback)
	{ 
		m_callback = callback;
	}

	virtual void Trigger()
	{
		m_callback.Call();
	}
	virtual void Update(IKMInput& input, double)
	{
		ButtonBase::Update(input);

		if(input.MouseClick(0))
		{
			const ::D3DXVECTOR2& pos = input.GetTransformedMousePos();

			if(m_sprite.IsPointWithin(pos))
			{
				Trigger();
			}
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

	TextBox(const std::string& name, const D3DXVECTOR3& pos, float w, float h);
	virtual ~TextBox();

	// Enters a new line
	void Write(const std::string& line, const D3DXVECTOR4& color, bool bContinue = false);
	
	virtual void Update(IKMInput&, double dt);
	virtual void Render(IRenderer&);
	
	virtual void CLS();

	void SaveToFile(const std::string& file) const;

	// todo: need to implement
	virtual void Select() {}
	virtual void Deselect() {}
	virtual void Trigger() {}

protected:

	struct LineData
	{
		LineData() {}
		LineData(const std::string& str, const D3DXVECTOR4& color, const Math::FRECT& R,bool c)
		: line(str), color(color), R(R), bContinue(c)
		{

		}

		std::string line;
		Math::FRECT R;
		D3DXVECTOR4 color;
		bool bContinue; 
	};

//private:

	// ===== Data members =====

	// todo: need to organize

	typedef std::vector<LineData> TextDataType;
	TextDataType m_text;

	Math::Sprite m_sprite;

	unsigned int m_spacePos;
	
	// todo: I could move the carrot into a class
	// carrot
	POINT m_carrotPos;
	bool m_drawCarrot;

	float m_fCarrotTime;
	double m_fScrollTime;

	D3DXVECTOR2 m_pos;

	// ===== helper functions =====
protected:
	virtual void Backspace();
	virtual void Enter();
	virtual void AddKey(char Key); 
private:

	// todo: passing around these interfaces seems redundant
	void UpdateScrolling(class IKMInput&, double dt);
	void UpdateCarrot(class IKMInput&);
	void UpdateInput(class IKMInput&);
	void UpdateTextInput(class IKMInput&);

	// splits the last line if needed so that it fits within the bounders of
	// the textbox
	void BreakLastLine(IRenderer&);

};

class ScriptingConsole : public TextBox
{
public:

	ScriptingConsole(asVM* pVM, const std::string& name, const D3DXVECTOR3& pos, float w, float h);
	virtual ~ScriptingConsole();

	virtual void Update(IKMInput&, double dt);
	virtual void Enter();
	virtual void Backspace();

	virtual void CLS();

	void MessageCallback(const asSMessageInfo *msg);

	void RegisterScript();

protected:

	asVM* m_pVM;

	unsigned int m_uiStartIndex;
	unsigned int m_uiBackIndex;

	// helper functions

	bool IsBlocked() const; 

	template< class T >
	void _Grab(const T& p)
	{
		stringstream stream;
		stream << p;

		Write(stream.str(),::D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f));
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

class Slider : public IUIElement
{
public:

	virtual void Select();
	virtual void Deselect();
	virtual void Trigger();
	virtual void Render(IRenderer&);

	virtual void Update(class IKMInput&, double dt);

private:

	std::string m_SpriteTexture;

};



#endif
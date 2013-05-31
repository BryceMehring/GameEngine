#ifndef _MENU_
#define _MENU_

// todo: split this header file down

#include "Delegates.h"
#include "IKMInput.h"
#include "IRender.h"
#include "IGameState.h"
#include "RefCounting.h"

#include <sstream>

namespace UI
{

class IUIElement : public IRender, public RefCounting
{
public:

	virtual void Select() = 0;
	virtual void Deselect() = 0;
	virtual void Trigger() = 0;

	virtual void Update(class IKMInput&, double dt) = 0;

protected:

	virtual ~IUIElement() {}

};

// Buttons
class ButtonBase : public IUIElement
{
public:

	ButtonBase() {}
	ButtonBase(const Math::FRECT& s, const std::string& str);

	virtual void Update(class IKMInput&);
	virtual void Render(class IRenderer&);

	virtual void Select()
	{
		m_bSelected = true;
		//m_pPolygon->SetColor(0xffff0000);
	}
	virtual void Deselect()
	{
		m_bSelected = false;
		//m_pPolygon->SetColor(0xffffffff);
	}

protected:

	virtual ~ButtonBase() {}

	Math::FRECT m_sprite;
	std::string m_text;
	bool m_bMouseHover;
	bool m_bSelected;



};

template< class T >
class GenericButton : public ButtonBase
{
public:

	typedef Delegate<void,T> DELEGATE;

	GenericButton(const Math::FRECT& s, const std::string& str) : ButtonBase(s,str) {}
	GenericButton(const Math::FRECT& s, const std::string& str, const DELEGATE& callback, const T& type) : ButtonBase(s,str), m_callback(callback), m_type(type)
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
			const glm::vec2& pos = input.GetTransformedMousePos();

			if(m_sprite.IsPointWithin(pos))
			{
				Trigger();
			}
		}
	}

private:

	virtual ~GenericButton() {}

	DELEGATE m_callback;
	T m_type;
};

template< class T >
class GenericButton<T&> : public ButtonBase
{
public:

	typedef Delegate<void,T&> DELEGATE;

	GenericButton(const Math::FRECT& s, const std::string& str) : ButtonBase(s,str) {}
	GenericButton(const Math::FRECT& s, const std::string& str, const DELEGATE& callback, const T& type) : ButtonBase(s,str), m_callback(callback), m_type(type)
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

		if(input.MouseRelease(0))
		{
			const glm::vec2& pos = input.GetTransformedMousePos();

			if(m_sprite.IsPointWithin(pos))
			{
				Trigger();
			}
		}
	}

private:

	virtual ~GenericButton() {}

	DELEGATE m_callback;
	T m_type;
};

template<>
class GenericButton<void> : public ButtonBase
{
public:

	typedef Delegate<void,void> DELEGATE;

	GenericButton(const Math::FRECT& s, const std::string& str) : ButtonBase(s,str) {}
	GenericButton(const Math::FRECT& s, const std::string& str, const DELEGATE& callback) : ButtonBase(s,str), m_callback(callback)
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
			const glm::vec2& pos = input.GetTransformedMousePos();

			if(m_sprite.IsPointWithin(pos))
			{
				Trigger();
			}
		}
	}

private:

	virtual ~GenericButton() {}

	DELEGATE m_callback;
};


// Todo: change the name of this class?
// Menu manages updating and drawing IUIElements on the current menu
class Menu : public IRender, public RefCounting
{
public:

	Menu();

	virtual void Update(class GUI* pGUI, class IKMInput& input, double dt);
	virtual void Render(class IRenderer& renderer);

	void SetMenuTitle(const std::string& str,const glm::vec2& pos);

	// Adds a sub menu connected to this menu
	void AddMenu(Menu* pMenu, GenericButton<Menu*>* pElement, class GUI* pGUI);

	// Adds a gui element to this menu
	void AddElement(IUIElement* pElement);

private:

	~Menu();

	std::vector<IUIElement*> m_elements; // A list of all gui elements in the menu
	std::vector<Menu*> m_menus; // a list of all menus connected to this menu
	Menu * m_pPrev; // The previous menu in the menu tree

	glm::vec2 m_pos;
	std::string m_menuTitle; // todo: is this needed?
};

// GUI class that the game will interface with.
// The gui is a small wrapper around the Menu
class GUI : public IRender
{
public:

	typedef Delegate<void,Menu*> ChangeMenuCallback;

	explicit GUI(Menu* pMenu = nullptr);
	~GUI();

	// Create a callback to change the menu
	ChangeMenuCallback CreateCallback();

	// Get/set the current menu in use
	void SetMenu(Menu* pMenu);
	Menu* GetMenu();

	// Indexes index the current UI menu's elements via the tab key.
	void SetIndex(unsigned int i);
	unsigned int GetIndex() const;

	void Update(class IKMInput& input, double dt);
	void Render(class IRenderer& renderer);

private:

	Menu* m_pMenu;
	unsigned int m_uiCurrentIndex;

};


// A basic ui textbox
class TextBox : public IUIElement
{
public:

	TextBox(const std::string& name, const glm::vec2& pos, float w, float h);
	virtual ~TextBox();

	// Enters a new line
	void Write(const std::string& line, const glm::vec3& color, bool bContinue = false);
	
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
		LineData(const std::string& str, const glm::vec3& color, const Math::FRECT& R,bool c)
			: line(str), R(R), color(color), bContinue(c)
		{

		}

		std::string line;
		Math::FRECT R;
		glm::vec3 color;
		bool bContinue;
	};

	//private:

	// ===== Data members =====

	// todo: need to organize

	typedef std::vector<LineData> TextDataType;
	TextDataType m_text;

	Math::FRECT m_sprite;

	unsigned int m_spacePos;
	
	// todo: I could move the carrot into a class
	// carrot
	//POINT m_carrotPos;
	bool m_drawCarrot;

	//float m_fCarrotTime;
	double m_fScrollTime;

	glm::vec2 m_pos;

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

/*class ScriptingConsole : public TextBox
{
public:

	ScriptingConsole(asVM* pVM, const std::string& name, const glm::vec2& pos, float w, float h);
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
		std::stringstream stream;
		stream << p;

		Write(stream.str(),::glm::vec4(1.0f,1.0f,1.0f,1.0f));
	}

	// These are registered with AngelScript
	void Grab();
	void Grab(const std::string&);
	void Grab(int);
	void Grab(float);
	void Grab(double);
	void Grab(unsigned int);
	void Grab(bool);

};*/

class Slider : public IUIElement
{
public:

	typedef Delegate<void,float> DELEGATE;

	Slider(const glm::vec2& start,
		   const glm::vec2& end,
		   float min, float max, const std::string& tex, const DELEGATE& callback);

	virtual void Select();
	virtual void Deselect();
	virtual void Trigger();

	virtual void Update(class IKMInput&, double dt);
	virtual void Render(IRenderer&);

	float GetValue() const;

private:

	virtual ~Slider() {}

	// todo: reduce the number of member variables
	glm::vec2 m_pos;
	glm::vec2 m_start;
	glm::vec2 m_end;
	float m_fPercentage;
	float m_fMin;
	float m_fMax;
	bool m_bEnable;
	bool m_bUpdateEnable;
	DELEGATE m_callback;
	std::string m_SpriteTexture;

};

} // UI namespace


#endif // _MENU_

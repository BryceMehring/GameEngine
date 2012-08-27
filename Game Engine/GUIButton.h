#ifndef _GUIBUTTON_
#define _GUIBUTTON_

#include "menu.h"

// Buttons
/*class ButtonBase : public IUIElement
{
public:

	ButtonBase() : m_color(-1) {}
	ButtonBase(const Text& name, DxPolygon* pPolygon);
	virtual ~ButtonBase();

	virtual void Render(class IRenderer&);
	virtual void Update(class IKMInput&);

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
	
	virtual void Trigger()
	{
		m_callback.Call(m_type);
	}
	virtual void Update(class IKMInput& input, double)
	{
		ButtonBase::Update(input);

		if(IsClicked(input,m_pPolygon))
		{
			Trigger();
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
	
	virtual void Trigger()
	{
		m_callback.Call(m_type);
	}
	virtual void Update(class IKMInput& input, double)
	{
		ButtonBase::Update(input);

		if(IsClicked(input,m_pPolygon))
		{
			Trigger();
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

	virtual void Trigger()
	{
		m_callback.Call();
	}
	virtual void Update(class IKMInput& input, double)
	{
		ButtonBase::Update(input);

		if(IsClicked(input,m_pPolygon))
		{
			Trigger();
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

};*/

#endif
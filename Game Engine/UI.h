
#ifndef _UI_
#define _UI_
#pragma once

#include "BEngine.h"
#include "UICreator.h"
#include "DistributedFactory.h"
#include "Graph.h"
#include "fsm.h"

/*

This UI class manages all of the check boxes, user input from the window
How should I go about doing this? Should I keep a pointer to the Rendering and Input DLL?

*/


// forward class declarations
struct UIData;
class IUIElement;


// http://www.objectmentor.com/resources/articles/umlfsm.pdf
class UINode : public GraphVertex
{
public:

	UINode()
	{
	}
	virtual ~UINode()
	{
	}

	/* =================== AddElement =============================
	
	This Method is called by AngelScript.
	This Method Adds an element to the UI

		Parameters:
			"const string& c" - the string 'c' is the key for the factory
			 "UIData* pData"    - the data that gets passed off to the objects constructor
	 
	 Return Value:

			Pass: index, if pData != NULL
			Fail: -1, if pData == NULL

	 ======================= AddElement =========================
	*/
	unsigned int AddElement(const std::string& c, UIData* pData);
	unsigned int AddElement(IUIElement* pElement);
	void RemoveElement(unsigned int i);

	// fsm
	virtual void On(UINT n);
	virtual void Off(UINT n);

private:

	typedef std::vector<IUIElement*> CollectionType;
	CollectionType m_elements;

};

class UIFSM;
class OnState;
class OffState;

class UIState
{
public:

	virtual ~UIState() {}
	virtual void Click(UIFSM* pFSM) = 0;
	virtual void Back(UIFSM* pFSM) = 0;

protected:

	static OffState s_offState;
	static OnState s_onState;

};

class UIFSM : public UINode
{
public:

	UIFSM() : m_pState(NULL) {}
	virtual ~UIFSM() {}

	void SetState(UIState* pState) { m_pState = pState; }
	void Click() { m_pState->Click(this); }
	void Back() { m_pState->Back(this); }

private:

	UIState* m_pState;

};

class OnState : public UIState
{
public:

	virtual void Click(UIFSM* pFSM)
	{
		pFSM->SetState(&s_offState);
		pFSM->Click();
	}
	virtual void Back(UIFSM* pFSM)
	{
	}

};

class OffState : public UIState
{
public:

	virtual void Click(UIFSM* pFSM)
	{
	}
	virtual void Back(UIFSM* pFSM)
	{
		pFSM->SetState(&s_onState);
		pFSM->Back();
	}

};


// todo: I need the UI to manage writing text to the screen. The UI would
// Interact with the plugins. This is because I would like to implement text collision detection,
// or maybe this could be implemented into the Renderer. I'll look into this.
class UI : public IScripted
{
public:

	// It manages all different types of items on the UI. It should
	// support more items in the future.

	UI(IBaseEngine* pEngine);
	virtual ~UI();

	IBaseEngine* GetEngine();

	void KeyDownCallback(char Key);

	// Focus
	IUIElement* GetFocus() const;
	void SetFocus(IUIElement* pElement);

	void Update(float dt);
	void Render() const;

	// virtual functions from IScripted
	virtual void RegisterScript();

private:

	// typedef Uigraph TheGraph

	IBaseEngine* m_pEngine;
	IUIElement* m_pInFocus;

	Factory<UICreator> m_factory;
};

struct UIData : public RefCounting
{
	UIData()
	{
		memset(&rect,0,sizeof(RECT));
	}

	RECT rect;

	// todo: need to change the name of this variable. 
	std::string str;

protected:

	virtual ~UIData() {}

};

// element

// todo: does IUIElement have to inherit from RefCounting?
// not all ui elements can be checked, or drawn. Need to subclass these methods
// into more interfaces
class IUIElement : public IGameEntity
{
public:

	// UI is the manager of IUIElement's
	friend class UI;

	// todo: is this the best option? this is not really an interface anymore
	// do all uielements need to access the ui?
	IUIElement(UI* pUI);
 
	virtual GameEntityType GetEntityType() const; 

	// todo: this interface is really weak... need to change it/add to it/subclass it

protected:

	// todo: I could switch this out for a IBaseEngine pointer.
	UI* m_pUI;
	virtual ~IUIElement() {}
	
};


class ClickableElement : public IUIElement
{
public:

	ClickableElement(RECT*,UI*);

	virtual void Update(float dt);
	virtual void ClickResponse() = 0;

protected:

	virtual ~ClickableElement();

private:

	RECT* m_pClickRect;
	float m_fClickTime;

};

// This is the data structure for a CheckBox
// todo: move this to the cpp file and only have a forward class declaration 
// in the header file


struct CheckBoxData : public UIData
{
	bool checked;

	// used for calling a script function
	AngelScript::ScriptFunction func;

protected:

	virtual ~CheckBoxData() {}
}; 

// Encapsulates a CheckBox
class CheckBox : public ClickableElement, public IRender
{
public:

	// a CheckBox manages drawing it along with checking if the mouse collides with the check box
	CheckBox(CheckBoxData*,UI* pUI);
	

	// IUIElement
	virtual bool IsChecked() const;
	virtual void ClickResponse();
	
	// IRender
	virtual IRenderType GetRenderType() { return Text; }
	virtual void Render();

private:

	virtual ~CheckBox();                                                                
	                
	CheckBoxData* m_pData;
};

struct TextBoxData : public UIData
{
	bool focus;
	std::string text;

private:

	virtual ~TextBoxData() {}
};

class TextBox : public ClickableElement, public IRender
{
public:

	TextBox(UIData* pData, UI* pUI) : ClickableElement(&pData->rect,pUI), m_pData(pData), m_bFocused(false) {}

	// ClickableElement
	virtual void ClickResponse()
	{
		m_pUI->SetFocus(this);
		m_bFocused = true;
	}

	void Update(char Key)
	{
		std::string& textRef = m_pData->str;
		IRenderingPlugin& renderer = m_pUI->GetEngine()->GetRenderer();

		RECT R = m_pData->rect;
		renderer.GetStringRec(textRef.c_str(),R);

		if(R.bottom <= m_pData->rect.bottom)
		{
			if(Key == 13)
			{
				textRef += "\n";
			}

			else if(Key == 8)
			{
				if(!textRef.empty())
				{
					textRef.resize(textRef.size() - 1);
				}
			}
			else
			{
				// todo: need to fix the logic here
				if(R.right >= m_pData->rect.right)
				{
					char end =  textRef.back();
					textRef.resize(textRef.size() - 1);
					textRef += "\n";
					textRef += end;
				}

				m_pData->str += Key;
				
			}
		}
	}
	virtual void Update(float dt)
	{
		if(m_bFocused == false)
		{
			ClickableElement::Update(dt);
		}
	}

	// IRender
	virtual IRenderType GetRenderType() { return Text; }
	virtual void Render()
	{
		// todo: need to draw the name of the TextBox: 	m_pData->str
		IRenderingPlugin& renderer = m_pUI->GetEngine()->GetRenderer();
		renderer.DrawString(m_pData->str.c_str(),m_pData->rect,0xffffffff,false);
	}

private:

	bool m_bFocused;
	UIData* m_pData;

	virtual ~TextBox();

};

#endif
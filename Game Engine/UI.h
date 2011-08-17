
#ifndef _UI_
#define _UI_
#pragma once


/*

This UI class manages all of the check boxes, user input from the window
How should I go about doing this? Should I keep a pointer to the Rendering and Input DLL?

*/

#include "Graph.h"
#include "DistributedFactory.h"
#include "UICreator.h"
#include "OnceFSM.h"
#include "DxPolygon.h"
#include "asVM.h"


// forward class declarations
class BEngine;
struct UIData;
class IUIElement;
class UIGraph;

class ITrigger : public RefCounting
{
public:

	virtual ~ITrigger() {}

	// todo: maybe I could return an int to show direction?
	// answer: no
	virtual bool Triggered() = 0;

};

class UIEdge : public GraphEdge
{
public:



	// todo: Where should this function be called?
	// 
	void AddTrigger(ITrigger* pTrigger)
	{
		if(pTrigger)
		{
			m_triggers.push_back(pTrigger);
		}
	}

	// Loops through all triggers and see if they are triggered. 
	bool IsTriggered() const
	{
		bool bTriggered = false;

		TriggerContainerType::const_iterator iter = m_triggers.begin();
		for(; iter != m_triggers.end(); ++iter)
		{
			if((*iter)->Triggered())
			{
				bTriggered = true;
				break;
			}
		}
		return bTriggered;
	}

	//virtual bool Trigger() = 0;

private:
	// todo: need to free space?
	// answer: yes

	virtual ~UIEdge()
	{
		// Release all of the triggers
		TriggerContainerType::iterator iter = m_triggers.begin();
		for(; iter != m_triggers.end(); ++iter)
		{
			(*iter)->Release();
		}
	}

	typedef std::vector<ITrigger*> TriggerContainerType;
	TriggerContainerType m_triggers;

};


// http://www.objectmentor.com/resources/articles/umlfsm.pdf
class UINode : public GraphVertex, public IRender
{
public:

	UINode(UI* pUI) : m_pUI(pUI)
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
	unsigned int AddElement(IUIElement* pElement);
	void RemoveElement(unsigned int i);

	void Update(float dt);

	virtual IRenderType GetRenderType() const { return IRender::Text; } // todo: not everything is text.
	virtual void Render(IRenderingPlugin& renderer);

private:

	typedef std::vector<IUIElement*> CollectionType;
	CollectionType m_elements;

	UI* m_pUI;

};

/*class UIFSM;
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

	//
	//pFSM->SetState(&s_offState);
	//	pFSM->On();
	//	pFSM->Click();
	virtual void Click(UIFSM* pFSM);
	virtual void Back(UIFSM* pFSM) {}

};

class OffState : public UIState
{
public:

	virtual void Click(UIFSM* pFSM)
	{
	}

	//pFSM->SetState(&s_onState);
	///pFSM->Off();
	//pFSM->Back();
	virtual void Back(UIFSM* pFSM);

};*/

// todo: need to implement
// http://sourcemaking.com/design_patterns/state/cpp/1

class UIGraph : public Graph<UINode,UIEdge>, public RefCounting
{
public:

	typedef Graph<UINode,UIEdge> GraphBase;

	using GraphBase::AddVertex;

	UIGraph(UI* pUI) : m_pUI(pUI) {}

	// These are the functions that are registered with AngelScript
	UINode* AddVertex()
	{
		UINode* pNode = new UINode(m_pUI);
		GraphBase::AddVertex(pNode);
		return pNode;
	}

	/*UIEdge* CreateEdge(UINode* pTail, UINode* pHead)
	{
		UIEdge* pEdge = new UIEdge();
		GraphBase::CreateEdge(
	}*/

protected:

	virtual ~UIGraph() {}

private:

	UI* m_pUI;

};


// todo: I need the UI to manage writing text to the screen. The UI would
// Interact with the plugins. This is because I would like to implement text collision detection,
// or maybe this could be implemented into the Renderer. I'll look into this.
class UI : public IScripted
{
public:

	typedef Factory<UICreator> UIFactoryType;

	// It manages all different types of items on the UI.

	UI(BEngine* pEngine);
	virtual ~UI();

	BEngine* GetEngine() const;
	UIGraph* GetGraph() const;

	void SetCurrentNode(UINode* pNode);
	UINode* GetCurrentNode() const;

	IUIElement* CreateElement(const std::string& c,UIData* pObj);

	// Focus
	IUIElement* GetFocus() const;
	void SetFocus(IUIElement* pElement);

	void Update(float dt);
	void Render(IRenderingPlugin& renderer) const;

	// virtual functions from IScripted
	virtual void RegisterScript();

private:

	// The engine that the UI is attached to
	BEngine* m_pEngine;
	
	IUIElement* m_pInFocus; // The current UI element that the user is using
	UINode* m_pCurrentNode; // The current node of the graph.

	// The graph data structure
	UIGraph* m_pGraph;

	UIFactoryType m_factory;
};

struct UIData : public RefCounting
{
	UIData() { SetRectEmpty(&rect); }

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
class IUIElement : public IEntity
{
public:

	// todo: is this the best option? this is not really an interface anymore
	// do all uielements need to access the ui?
	IUIElement(UI* pUI);
	virtual ~IUIElement() {}

	virtual EntityType GetEntityType() const { return EntityType::UIType; }

protected:

	UI* m_pUI;
	
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

	OnceFSM m_ClickFSM;
	RECT* m_pClickRect;
};

class IUIButton : public ClickableElement, public IRender
{
public:
	IUIButton(RECT* r,UI* ui) : ClickableElement(r,ui) {}
	virtual ~IUIButton() {}
	virtual bool IsChecked() const = 0;
};

class UITrigger :  public ITrigger
{
public:

	UITrigger(IKMInput* p, DWORD button) : m_pInterface(p), m_button(button)
	{}
	virtual bool Triggered()
	{
		bool bTriggered = false;

		if(m_pInterface)
		{

			bTriggered = m_pInterface->MouseClick(m_button);
		}

		return bTriggered;
	}
	
private:

	IKMInput* m_pInterface;
	DWORD m_button;

};

// This is the data structure for a CheckBox
// todo: move this to the cpp file and only have a forward class declaration 
// in the header file


struct CheckBoxData : public UIData
{
	bool checked;

	// used for calling a script function
	ScriptFunction func;

protected:

	virtual ~CheckBoxData() {}
}; 

// Encapsulates a CheckBox
class CheckBox : public IUIButton
{
public:

	// a CheckBox manages drawing it along with checking if the mouse collides with the check box
	CheckBox(CheckBoxData*,UI* pUI);
	
	// IUIElement
	virtual bool IsChecked() const;
	virtual void ClickResponse();
	
	// IRender
	virtual IRenderType GetRenderType() const { return Text; }
	virtual void Render(IRenderingPlugin& renderer);

private:

	virtual ~CheckBox();                                                                
	     
	DxSquare m_box;
	CheckBoxData* m_pData;
};

// todo: need to finish implementing this
struct IndexedButtonData : public UIData
{
	unsigned int index;

	// used for calling a script function
	// todo: I could factor this data member out
	ScriptFunction func;
};

class IndexedButton : public ClickableElement, public IRender
{
public:

	IndexedButton(IndexedButtonData*,UI* pUI);

	// IUIElement
	virtual bool IsChecked() const;
	virtual void ClickResponse();
	
	// IRender
	virtual IRenderType GetRenderType() const { return Text; }
	virtual void Render(IRenderingPlugin& renderer);

protected:

	virtual ~IndexedButton();     

private:

	DxSquare m_box;
	IndexedButtonData* m_pData;
};

struct TextBoxData : public UIData
{
	bool focus;
	
private:

	virtual ~TextBoxData() {}
};

struct LineData
{
	std::string line;
	D3DCOLOR color;
	RECT R;
};

class TextBox : public ClickableElement, public DxSquare
{
public:

	TextBox(UIData* pData, UI* pUI);

	void Write(const std::string& line, DWORD color = 0xffffffff);
	//void Write(

	// ClickableElement
	virtual void ClickResponse();

	virtual void Update(float dt);

	// IRender
	virtual IRenderType GetRenderType() const;
	virtual void Render(IRenderingPlugin& renderer);

	void CLS();

protected:

	virtual ~TextBox();

//private:

	// ===== Data members =====

	static const int SCROLL_SPEED = 5;

	OnceFSM m_Textfsm;

	bool m_bFocused;

	// todo: I could move the carrot into a class
	// carrot
	POINT m_carrotPos;
	bool m_drawCarrot;

	float m_fTime;

	typedef std::vector<LineData> TextDataType;
	TextDataType m_text;

	UIData* m_pData;

	int m_scrollSpeed;

	// ===== helper functions =====
protected:
	virtual void Backspace();
	virtual void Enter();
	virtual void AddKey(char Key); 
private:

	void UpdateScrolling(IKMInput* pInput);
	void UpdateCarrot(IKMInput* pInput);
	void UpdateTextInput(IKMInput* pInput);

	void ScrollDown();
	void ScrollUp();
	void ScrollStop();

};

#endif
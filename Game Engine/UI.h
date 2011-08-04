
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


// http://www.objectmentor.com/resources/articles/umlfsm.pdf
class UINode : public GraphVertex, public IRender
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
	unsigned int AddElement(IUIElement* pElement);
	void RemoveElement(unsigned int i);

	void Update(float dt);

	virtual IRenderType GetRenderType() { return IRender::Text; } // todo: not everything is text.
	virtual void Render(IRenderingPlugin& renderer);

private:

	typedef std::vector<IUIElement*> CollectionType;
	CollectionType m_elements;

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

class UIGraph : public Graph<UINode,GraphEdge>, public RefCounting
{
public:

	typedef Graph<UINode,GraphEdge> GraphBase;

	virtual ~UIGraph() {} // this should be private!

	using Graph<UINode,GraphEdge>::AddVertex;

	UIGraph(UI* pUI) : m_pUI(pUI) {}

	// These are the functions that are registered with AngelScript
	UINode* AddVertex()
	{
		UINode* pNode = new UINode();
		AddVertex(pNode);
		return pNode;
	}

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
class IUIElement : public IObject, public RefCounting
{
public:

	// todo: is this the best option? this is not really an interface anymore
	// do all uielements need to access the ui?
	IUIElement(UI* pUI);
	virtual ~IUIElement() {}

	virtual ObjectType GetObjectType() { return ObjectType::UIType; }
	virtual void Update(float dt) = 0;

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
	virtual void Render(IRenderingPlugin& renderer);

private:

	virtual ~CheckBox();                                                                
	     
	DxSquare m_box;
	CheckBoxData* m_pData;
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

	void ScrollDown();
	void ScrollUp();
	void ScrollStop();

	// ClickableElement
	virtual void ClickResponse();

	virtual void Update(float dt);

	// IRender
	virtual IRenderType GetRenderType();
	virtual void Render(IRenderingPlugin& renderer);

	void CLS();

protected:

	virtual ~TextBox();

//private:

	// ===== Data members =====

	static const int SCROLL_SPEED = 5;

	OnceFSM m_Textfsm;
	bool m_bFocused;

	typedef std::deque<LineData> TextDataType;
	TextDataType m_text;

	UIData* m_pData;

	int m_scrollSpeed;

	// ===== helper functions =====
	virtual void Backspace();
	virtual void Enter();
	void UpdateScrolling(IKMInput* pInput);

};

#endif
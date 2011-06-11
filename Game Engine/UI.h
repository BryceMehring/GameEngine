
#ifndef _UI_
#define _UI_
#pragma once

#include "BEngine.h"
#include "Interfaces.h"

/*

This UI class manages all of the check boxes, user input from the window
How should I go about doing this? Should I keep a pointer to the Rendering and Input DLL?


*/

// forward class declarations
class CheckBox;
class UI;

// element

// todo: does IUIElement have to inherit from RefCounting?
// not all ui elements can be checked, or drawn. Need to subclass these methods
// into more interfaces
class IUIElement
{
public:

	// UI is the manager of IUIElement's
	friend class UI;

	virtual bool IsChecked() const = 0;
	virtual void Update(float dt) = 0;

	// todo: this interface is really weak... need to change it/add to it/subclass it

protected:

	virtual ~IUIElement() {}
	
};


class ClickableElement : public IUIElement
{
public:

	ClickableElement(RECT*);
	

	virtual void Update(float dt);
	virtual void ClickResponse() = 0;

protected:

	virtual ~ClickableElement();

private:

	RECT* m_pClickRect;

};

/*class ClickableText : public ClickableElement, public IRender
{
public:



};*/

struct UIData
{
	RECT m_Rect;

	// used for calling a script function
	AngelScript::ScriptFunction funct;

	std::string m_str;
};

// This is the data structure for a CheckBox
// todo: move this to the cpp file and only have a forward class declaration 
// in the header file

struct CheckBoxData : public RefCounting
{
	CheckBoxData()
	{
		memset(&rect,0,sizeof(RECT));
	}

	bool checked;

	POINT pos;
	RECT rect;

	std::string str;

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

	CheckBox(void*);
	CheckBox(CheckBoxData*);
	

	// IUIElement
	virtual bool IsChecked() const;
	virtual void ClickResponse();
	
	// IRender
	virtual IRenderType GetRenderType() { return Text; }
	virtual void Render();

private:

	virtual ~CheckBox();

	CheckBoxData* m_pData;
	UI* m_pUI; // a pointer to the ui that the CheckBox is attached to
	// todo: is this needed?

};

class UI : public IScripted
{
public:

	// It manages all different types of items on the UI. It should
	// support more items in the future.

	UI();
	virtual ~UI();

	// function that manage the different levels of ui
	// I could add this to another class, that manages the different levels
	void AddLevel();
	void Forward();
	void Back();
	void SetLevel(unsigned int l);
	unsigned int GetCurrentLevel() const;

	/* =================== AddElement =============================
	
	This Method is called by AngelScript.
	This Method Adds an element to the UI

		Parameters:
			"const string& c" - the string 'c' is the key for the factory
			 "void* pData"    - the data that gets passed off to the objects constructor
			 "typeId  "       - typeId is auto added by AngelScript
	 
	 Return Codes:

			Pass: if the returned uint is not -1. It is the index for the newly
			added UI element.

			Fail:
			-1 - pData is not a handle
	 

	 ======================= AddElement =========================
	*/
	unsigned int AddElement(const std::string& c, void* pData, int typeId);
	unsigned int AddElement(IUIElement* pElement);
	
	void RemoveElement(IUIElement* pElement);
	void RemoveElement(unsigned int i);

//	bool IsChecked(unsigned int i) const;

	void Update(float dt);
	void Render() const;

	// virtual functions from IScripted
	virtual void RegisterScript();

private:

	// todo: need to change CheckBox into an abstract interface?
	typedef std::vector<std::vector<IUIElement*>> value_type;
	typedef std::vector<IUIElement*> sub_type;

	value_type::iterator m_currentLevel;
	value_type m_levels;

};

#endif
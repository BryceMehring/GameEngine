
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

// This is the data structure for a CheckBox
// todo: move this to the cpp file and only have a forward class declaration 
// in the header file
struct CheckBoxData : public RefCounting
{
public:

	CheckBoxData()
	{
		memset((void*)&m_pos,0,sizeof(POINT));
		memset((void*)&m_Rect,0,sizeof(RECT));
	}

protected:

	friend class CheckBox;
	friend class UI;

	bool m_checked;

	POINT m_pos;
	RECT m_Rect;

	std::string m_str;

	// used for calling a script function
	int m_funcId;
	unsigned int m_ScriptIndex;

	virtual ~CheckBoxData() {}
}; 

// element

// todo: does IUIElement have to inherit from RefCounting?
class IUIElement : public RefCounting
{
public:

	virtual bool IsChecked() const = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;

protected:
	virtual ~IUIElement() {}
};


// Encapsulates a CheckBox
class CheckBox : public IUIElement
{
public:

	// a CheckBox manages drawing it along with checking if the mouse collides with the check box

	friend class UI;

	CheckBox(void*);
	CheckBox(CheckBoxData*);
	~CheckBox();

	virtual bool IsChecked() const;
	virtual void Update(float dt);
	virtual void Draw();

private:

	CheckBoxData* m_pData;

	// Plugin Interfaces, these are set in the UIManager.
	// todo: these should not be static?
	static IKMInput* s_pInput;
	static IRenderingPlugin* s_pRenderer;

};

class UI : public AngelScript::IScripted
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

	// todo: the script function should not pass a pointer to the interface. The script should pass a string, and that string will get mapped to a IUIElement.

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
	unsigned int AddElement(const string& c, void* pData, int typeId);

	unsigned int AddElement(IUIElement* pElement);
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
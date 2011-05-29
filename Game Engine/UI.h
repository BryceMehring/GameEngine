
#ifndef _UI_
#define _UI_
#pragma once

#include "BEngine.h"
#include "Interfaces.h"

/*

This UI class manages all of the check boxes, user input from the window
How should I go about doing this? Should I keep a pointer to the Rendering and Input DLL?


*/

typedef void (IBaseEngine::*FUNCT)(bool);
typedef void (*UI_CALLBACK)(bool);

// This is the data structure for a CheckBox
struct CheckBoxData
{
	CheckBoxData() {}
	CheckBoxData(const CheckBoxData& other)
	{
		system("pause");
	}

	bool m_checked;
	RECT m_Rect;
	std::string m_str;

	// used for calling a script function
	int m_funcId;
	unsigned int m_ScriptIndex;
}; 

// element

// todo: does IUIElement have to inherit from RefCounting?
class IUIElement : public RefCounting
{
protected:

	virtual ~IUIElement() {}

public:

	virtual bool IsChecked() const = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;

};


// Encapsulates a CheckBox
class CheckBox : public IUIElement
{
public:

	// a CheckBox manages drawing it along with checking if the mouse collides with the check box

	friend class UI;

	CheckBox(void* param);
	CheckBox(const CheckBoxData&);

	virtual bool IsChecked() const;
	virtual void Update(float dt);
	virtual void Draw();

private:

	CheckBoxData m_data;

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

	// the string 'c' is the key for the factory
	unsigned int AddElement(const string& c, void* pObj);
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

	value_type::iterator m_currentLevel;
	value_type m_levels;

};

#endif
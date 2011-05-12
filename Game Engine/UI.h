
#ifndef _UI_
#define _UI_
#pragma once

#include "PluginManager.h"
#include "Singleton.h"
#include "asVM.h"

/*

This UI class manages all of the check boxes, user input from the window
How should I go about doing this? Should I keep a pointer to the Rendering and Input DLL?


*/

typedef void (IBaseEngine::*FUNCT)(bool);
typedef void (*UI_CALLBACK)(bool);

// This is the data structure for a CheckBox
struct CheckBoxData
{
	bool m_checked;
	RECT m_Rect;
	std::string m_str;

	int funcId; // Script function
	FUNCT m_Callback;
};


// Encapsulates a CheckBox
class CheckBox
{
public:

	// a CheckBox manages drawing it along with checking if the mouse collides with the check box

	friend class UIManager;

	CheckBox(const CheckBoxData&);

	bool IsChecked() const;
	void Update(float dt);
	void Draw() const;

private:

	CheckBoxData m_data;

	// Plugin Interfaces, these are set in the UIManager.
	static IKMInput* s_pInput;
	static IRenderingPlugin* s_pRenderer;

};

// Single UI manager
class UIManager : public Singleton<UIManager>, public IScripted
{
public:

	// It manages all different types of items on the UI. It should
	// support more items in the future. Lua scripting should be added 
	// to manipulate the UI

	friend class Singleton<UIManager>;

	// function that manage the different levels of ui
	// I could add this to another class, that manages the different levels
	void AddLevel();
	void Forward();
	void Back();
	void SetLevel(unsigned int l);
	unsigned int GetCurrentLevel() const;

	unsigned int AddCheckBox(const CheckBoxData& data);
	void RemoveCheckBox(unsigned int index);

	bool IsChecked(unsigned int index) const;

	void Update(float dt);
	void Render() const;

	// virtual functions from IScripted
	virtual void RegisterScript();

private:

	UIManager();
	~UIManager();

	typedef std::vector<std::vector<CheckBox>> value_type;

	value_type::iterator m_iter;
	value_type m_checkBoxes;

};

#endif

#ifndef _UI_
#define _UI_
#pragma once

#include "PluginManager.h"
#include "Singleton.h"

/*

This UI class manages all of the check boxes, user input from the window
How should I go about doing this? Should I keep a pointer to the Rendering and Input DLL?


*/

typedef void (IBaseEngine::*FUNCT)(bool);

// This is the data structure for a CheckBox
struct CheckBoxData
{
	bool m_checked;
	POINT m_pos[2];
	std::string m_str;
	FUNCT m_Callback;
};

// forward class declaration needed for friend access.
class UIManager;

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
class UIManager : public Singleton<UIManager>
{
public:

	// It manages all different types of items on the UI. It should
	// support more items in the future. Lua scripting should be added 
	// to manipulate the UI

	friend class Singleton<UIManager>;

	unsigned int AddCheckBox(const CheckBoxData& data);

	bool IsChecked(unsigned int index) const;

	void Update(float dt);
	void Render() const;

private:

	UIManager();
	~UIManager();

	std::vector<CheckBox> m_checkBoxes;

};

#endif
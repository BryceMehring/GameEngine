
#ifndef _UI_
#define _UI_
#pragma once

#include "PluginManager.h"

/*

This UI class manages all of the check boxes, user input from the window
How should I go about doing this? Should I keep a pointer to the Rendering and Input DLL?


*/

struct CheckBoxData
{
	bool m_checked;
	POINT m_pos[2];
	char m_text[32];
};


class UI;
class CheckBox
{
public:

	friend class UI;

	void IsChecked();
	void Draw();

private:

	CheckBoxData m_data;

	static IKMInput* s_pInput;
	static IRenderingPlugin* s_pRendering;

};


class UI
{
public:

	UI()
	{
		CheckBox::s_pRendering = 0;
	}

	unsigned int AddCheckBox(CheckBoxData& data);

	bool IsChecked(unsigned int index);
	void Render();

private:

	std::vector<CheckBox> m_checkBoxes;

};

#endif
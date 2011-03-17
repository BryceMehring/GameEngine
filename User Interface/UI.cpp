#include "UI.h"

// static variable definitions
IKMInput* CheckBox::s_pInput = 0;
IRenderingPlugin* CheckBox::s_pRenderer = 0;

// CheckBox

CheckBox::CheckBox(const CheckBoxData& data) : m_data(data)
{
}

void CheckBox::Update(float dt)
{
	static bool bNoClick = false;

	if(bNoClick)
	{
		if(s_pInput->MouseClick(0) == false)
		{
			bNoClick = false;
		}
	}

	if(s_pInput->MouseClick(0) && !bNoClick)
	{
		//m_data.m_time = 0;
		POINT mousePos;
		s_pInput->MousePos(mousePos);

		// check if the box was clicked
		if((mousePos.x >= m_data.m_pos[0].x) && (mousePos.x <= m_data.m_pos[1].x))
		{
			if((mousePos.y >= m_data.m_pos[0].y) && (mousePos.y <= m_data.m_pos[1].y))
			{
				m_data.m_checked = !m_data.m_checked;

				// do not come back in here until the user is not clicking
				bNoClick = true;
				
				// Call callback function
				//(g_pEngine->*m_data.m_Callback)(m_data.m_checked);
			}
		}
	}
}

bool CheckBox::IsChecked() const
{
	return m_data.m_checked;
}

void CheckBox::Draw() const
{
	DWORD color;

	switch(m_data.m_checked)
	{
	case true:
		color = D3DCOLOR_XRGB(0,255,0);
		break;
	case false:
		color = D3DCOLOR_XRGB(255,255,255);
		break;
	}

	s_pRenderer->DrawString(m_data.m_str.c_str(),m_data.m_pos[0],color);

	// need to implement DrawString to draw at a pos on the screen
	//s_pRenderer->DrawString(m_data.m_str.c_str());
}

// UIManager
UIManager::UIManager(LuaMachine& vm) : LuaScript(vm)
{
	PluginManager& plugManager = PluginManager::Instance();
	CheckBox::s_pInput = (IKMInput*)plugManager.GetPlugin(0);
	CheckBox::s_pRenderer = (IRenderingPlugin*)plugManager.GetPlugin(1);
}
UIManager::~UIManager() {}

unsigned int UIManager::AddCheckBox(LuaMachine& vm)
{
	//m_checkBoxes.push_back(data);
	//return m_checkBoxes.size() - 1;
	return 0;
}

bool UIManager::IsChecked(unsigned int index) const
{
	bool bChecked = false;

	if(index < m_checkBoxes.size())
	{
		bChecked = m_checkBoxes[index].IsChecked();
	}

	return bChecked;
}
void UIManager::Update(float dt)
{
	for(unsigned int i = 0; i < m_checkBoxes.size(); ++i)
	{
		m_checkBoxes[i].Update(dt);
	}
}
void UIManager::Render() const
{
	for(unsigned int i = 0; i < m_checkBoxes.size(); ++i)
	{
		m_checkBoxes[i].Draw();
	}
}
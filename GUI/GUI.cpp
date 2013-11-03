#include "GUI.h"

namespace UI
{

GUI::GUI(Menu* pMenu) : m_pMenu(pMenu), m_uiCurrentIndex(0)
{
}

GUI::~GUI()
{
	if(m_pMenu != nullptr)
	{
		m_pMenu->Release();
	}
}

GUI::ChangeMenuCallback GUI::CreateCallback()
{
	return ChangeMenuCallback(this,&GUI::SetMenu);
}

void GUI::Update(::IKMInput& input, double dt)
{
	if(m_pMenu != nullptr)
	{
		m_pMenu->Update(this,input,dt);
	}
}

void GUI::Render(::IRenderer& renderer)
{
	if(m_pMenu != nullptr)
	{
		m_pMenu->Render(renderer);
	}
}

void GUI::SetMenu(Menu* pMenu)
{
	if(pMenu != nullptr)
	{
		if(m_pMenu != nullptr)
		{
			m_pMenu->Release();
		}

		m_pMenu = pMenu;
		m_uiCurrentIndex = 0;
	}
}

void GUI::SetIndex(unsigned int i)
{
	m_uiCurrentIndex = i;
}

unsigned int GUI::GetIndex() const
{
	return m_uiCurrentIndex;
}

Menu* GUI::GetMenu()
{
	m_pMenu->AddRef();
	return m_pMenu;
}

}

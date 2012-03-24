
#include "Menu.h"
#include "PluginManager.h"
#include "IRenderer.h"
#include "Game.h"
#include <dinput.h>


Menu::Menu() : m_pPrev(nullptr), m_pPolygon(nullptr) {}
Menu::Menu(const std::string& str) : m_menuTitle(str), m_pPrev(nullptr), m_pPolygon(nullptr)  {}
Menu::~Menu()
{
	// This is recursive...
	// todo: fix this?... 
	for(unsigned int i = 0; i < m_SubMenu.size(); ++i)
	{
		delete m_SubMenu[i];
	}

	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		delete m_elements[i];
	}
}
void Menu::SetMenuTitle(const std::string& str,const POINT& P)
{
	m_point = P;
	m_menuTitle = str;
}
void Menu::SetPolygon(DxPolygon* pPolygon)
{
	if(pPolygon)
	{
		m_pPolygon = pPolygon;
	}
}
void Menu::AddMenu(Menu* pMenu)
{
	m_SubMenu.push_back(pMenu);
	pMenu->m_pPrev = this;
}
void Menu::AddElement(IUIElement* pElement)
{
	if(pElement)
	{
		m_elements.push_back(pElement);
	}
}
void Menu::Update(UI* pUI, IKMInput* pInput)
{
	if(pInput->KeyDown(DIK_W) && (m_pPrev != nullptr))
	{
		pUI->SetMenu(m_pPrev);
	}
	else
	{
		for(unsigned int i = 0; i < m_elements.size(); ++i)
		{
			m_elements[i]->Update(pInput);
		}
	}
}
void Menu::Render(IRenderer* pRenderer)
{
	// Render menu border and header
	pRenderer->DrawString(m_menuTitle.c_str(),m_point,0xffffffff);

	if(m_pPolygon)
	{
		m_pPolygon->Render(pRenderer);
	}

	// Render all of the elements
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		m_elements[i]->Render(pRenderer);
	}
}

UI::UI(Menu* pMenu) : m_pMenu(pMenu) {}
UI::~UI()
{
	delete m_pMenu;
}

void UI::SetMenu(Menu* pMenu) { m_pMenu = pMenu; }

void UI::Update(IKMInput* pInput)
{
	m_pMenu->Update(this,pInput);
}

void UI::Render(IRenderer* pRenderer)
{
	m_pMenu->Render(pRenderer);
}

ButtonBase::ButtonBase(const Text& name, DxPolygon* pPolygon) :
m_name(name), m_pPolygon(pPolygon)
{
}

void ButtonBase::Render(class IRenderer* pRenderer)
{
	pRenderer->DrawString(m_name.name.c_str(),m_name.P,0xffffffff);

	m_pPolygon->Render(pRenderer);
}

bool ButtonBase::IsClicked(IKMInput* pInput) const
{
	if(pInput->MouseClick(0))
	{
		return m_pPolygon->IsPointInPolygon(pInput->MousePos());
	}

	return false;
}

/*Button::Button(UI* pUI, Menu* pMenu, DxPolygon* pPolygon) : m_pUI(pUI), m_pMenu(pMenu), m_pPolygon(pPolygon)  {}
void Button::Update()
{
	PluginManager& pm = ::PluginManager::Instance();
	IKMInput* pInput = static_cast<IKMInput*>(pm.GetPlugin(Input));

	if(IsClicked())
	{
		m_pUI->SetMenu(m_pMenu);
	}
}*/

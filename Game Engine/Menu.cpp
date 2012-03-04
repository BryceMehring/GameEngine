
#include "Menu.h"
#include "PluginManager.h"
#include "IRenderer.h"


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
void Menu::SetMenuTitle(const std::string& str)
{
	m_menuTitle = str;
}
void Menu::AddMenu(Menu* pMenu)
{
	m_SubMenu.push_back(pMenu);
	pMenu->m_pPrev = this;
}
void Menu::GetTriggered(UI* pUI)
{
	Menu* pMenu = nullptr;

	PluginManager& pm = ::PluginManager::Instance();
	IKMInput* pInput = static_cast<IKMInput*>(pm.GetPlugin(Input));
		
	if(pInput->GetKeyDown() == ESCAPE)
	{
		pMenu = m_pPrev;
	}
	else
	{
		// todo: this is where the fsm would go... 
		if(pInput->MouseClick(0))
		{
			for(unsigned int i = 0; i < m_SubMenu.size(); ++i)
			{
				if(m_SubMenu[i]->IsTriggered())
				{
					pMenu = m_SubMenu[i];
					break;
				}
			}
		}
	}

	if(pMenu != nullptr)
	{
		pUI->SetMenu(pMenu);
	}
}
bool Menu::IsTriggered()
{
	PluginManager& pm = PluginManager::Instance();
	IKMInput* pInput = static_cast<IKMInput*>(pm.GetPlugin(Input));
	return m_pPolygon->IsPointInPolygon(pInput->MousePos());
}
void Menu::Update()
{
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		m_elements[i]->Update();
	}
}
void Menu::Render()
{
	PluginManager& pm = PluginManager::Instance();
	IRenderer* pRenderer = static_cast<IRenderer*>(pm.GetPlugin(Rendering));

	POINT P = {50,50};
	pRenderer->DrawString(m_menuTitle.c_str(),P,0xffffffff);
	m_pPolygon->Render();
}

UI::UI(Menu* pMenu) : m_pMenu(pMenu) {}

void UI::SetMenu(Menu* pMenu) { m_pMenu = pMenu; }

void UI::Update()
{
	m_pMenu->GetTriggered(this);
	m_pMenu->Update();
}
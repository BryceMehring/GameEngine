#include "Menu.h"
#include "GUI.h"

namespace UI
{

Menu::Menu() : m_pPrev(nullptr)
{
}
Menu::~Menu()
{
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		m_elements[i]->Release();
	}

	for(unsigned int i = 0; i < m_menus.size(); ++i)
	{
		m_menus[i]->Release();
	}
}
void Menu::SetMenuTitle(const std::string& str,const glm::vec2& pos)
{
	m_pos = pos;
	m_menuTitle = str;
}

void Menu::AddMenu(Menu* pMenu, GenericButton<Menu*>* pElement, GUI* pGUI)
{
	if(pMenu != nullptr && pElement != nullptr)
	{
		pElement->SetCallback(pGUI->CreateCallback());
		pElement->SetArg(pMenu);

		pMenu->m_pPrev = this;
		pMenu->AddElement(pElement);

		m_menus.push_back(pMenu);
	}
}

void Menu::AddElement(IElement* pElement)
{
	if(pElement != nullptr)
	{
		pElement->AddRef();
		m_elements.push_back(pElement);
	}
}

void Menu::Update(GUI* pGUI, ::IInput& input, double dt)
{
	if(input.KeyPress(32))
	{
		if(m_pPrev != nullptr)
		{
			pGUI->SetMenu(m_pPrev);
		}
	}

	if(input.KeyPress(258))
	{
		unsigned int index = pGUI->GetIndex();

		m_elements[index]->Deselect();
		index = (index + 1) % m_elements.size();
		m_elements[index]->Select();

		pGUI->SetIndex(index);

	}
	else if(input.KeyPress(257))
	{
		unsigned int index = pGUI->GetIndex();
		if(index < m_elements.size())
		{
			m_elements[index]->Trigger();
		}
	}
	
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		m_elements[i]->Update(input,dt);
	}
	
}
void Menu::Render(::IRenderer& renderer)
{
	// Render all of the elements
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		m_elements[i]->Render(renderer);
	}
}



} // UI namespace

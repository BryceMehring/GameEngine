
#include "Menu.h"
#include "PluginManager.h"
#include "IRenderer.h"
#include "Game.h"
#include "WindowManager.h"
#include <dinput.h>


using namespace std;

//const RTTI Menu::s_rtti("Menu");
RTTI_IMPL(Menu);

bool IsClicked(IKMInput* pInput, DxPolygon* pPolygon)
{
	if(pInput->MouseClick(0))
	{
		return pPolygon->IsPointInPolygon(pInput->MousePos());
	}

	return false;
}

Menu::Menu() : m_pPrev(nullptr), m_pPolygon(nullptr)
{
}
Menu::~Menu()
{
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		delete m_elements[i];
	}

	for(unsigned int i = 0; i < m_menus.size(); ++i)
	{
		delete m_menus[i];
	}

	delete m_pPolygon;
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
	if(pMenu != nullptr)
	{
		pMenu->m_pPrev = this;
		m_menus.push_back(pMenu);
	}
}

void Menu::AddElement(IUIElement* pElement)
{
	if(pElement != nullptr)
	{
		m_elements.push_back(pElement);
	}
}

void Menu::Update(GUI* pGUI, IKMInput* pInput)
{
	if(pInput->IsKeyDown())
	{
		m_menuTitle += pInput->GetKeyDown();
	}

	if(pInput->KeyDown(ESCAPE))
	{
		if(m_pPrev != nullptr)
		{
			pGUI->SetMenu(m_pPrev);
		}
	}

	if(pInput->KeyDown(TAB))
	{
		pGUI->m_uiCurrentIndex = ((pGUI->m_uiCurrentIndex + 1) % m_elements.size());
		m_elements[pGUI->m_uiCurrentIndex]->Select();
	}
	else if(pInput->KeyDown(ENTER))
	{
		m_elements[pGUI->m_uiCurrentIndex]->Enter();
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

GUI::GUI(Menu* pMenu) : m_pMenu(pMenu), m_uiCurrentIndex(0)
{
}

GUI::~GUI()
{
	delete m_pMenu;
}

GUI::ChangeMenuCallback GUI::CreateCallback()
{
	return ChangeMenuCallback(this,&GUI::SetMenu);
}

void GUI::Update(IKMInput* pInput)
{
	m_pMenu->Update(this,pInput);
}

void GUI::Render(IRenderer* pRenderer)
{
	m_pMenu->Render(pRenderer);
}

void GUI::SetMenu(Menu* pMenu)
{
	if(pMenu != nullptr)
	{
		m_pMenu = pMenu;
	}
}

ButtonBase::ButtonBase(const Text& name, DxPolygon* pPolygon) :
m_name(name), m_pPolygon(pPolygon), m_color(-1)
{
}

ButtonBase::~ButtonBase()
{
	delete m_pPolygon;
}

void ButtonBase::Update(IKMInput* pInput)
{
	if(m_pPolygon->IsPointInPolygon(pInput->MousePos()))
	{
		m_color = 0xffffff00;
	}
	else
	{
		m_color = 0xffffffff;
	}
}

void ButtonBase::Render(class IRenderer* pRenderer)
{
	pRenderer->DrawString(m_name.name.c_str(),m_name.P,m_color);

	m_pPolygon->Render(pRenderer);
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


/*TextBox::TextBox(const std::string& name, const RECT& R) : m_spacePos(-1), m_square(R)
{
	m_square.SetColor(0xffffffff);

	// Next we add an empty line
	LineData data;
	data.line = name;
	data.color = 0xffffffff;
	data.R = R;

	m_text.push_back(data);
	m_carrotPos.x = m_carrotPos.y = 0;
}

void TextBox::Write(const std::string& line, DWORD color)
{
	// todo: need to fix the logic here, this method is bugged
	// because when there is nothing in the list, it crashes.

	LineData data;
	data.line =  string("> ") + line;
	data.color = color;

	// todo: maybe I could cache the REct?
	// Get Rect size of 
	std::string& text = m_text.back().line;
	RECT R = {0};
	this->m_pUI->GetEngine()->GetStringRec(text.c_str(),R);

	// todo: this is bugged
	data.R = m_text.back().R;
	data.R.top += (R.bottom + 10);

	//data.R.top = R.bottom + 45;
	//data.R.bottom = R.bottom += 25;
	m_text.push_back(data);
}

void TextBox::ScrollDown(int speed)
{
	m_scrollSpeed = -speed;
}

void TextBox::ScrollUp(int speed)
{
	m_scrollSpeed = speed;
}

void TextBox::ScrollStop()
{
	m_scrollSpeed = 0;
}

void TextBox::Backspace()
{
	// reference to the line
	//std::string& text = m_pData->str;
	std::string& text = m_text.back().line;

	// size of the line
	size_t size = text.size(); 

	if(size > 0)
	{
		// remove last character
		text.resize(size - 1);
	}
}

void TextBox::AddKey(char Key)
{
	std::string& text = m_text.back().line;
	text += Key;

	// if there is a space
	if(Key == 32)
	{
		// todo: need to cache position in string to break line with: \n
		m_spacePos = text.size() - 1;
	}

	// todo: I could optimize this and just see if the size of the line is passed a set amount.
	// I would have to look into this because idk how it would work yet.

	// Get Rect size of 
	RECT R = {0};
	this->m_pUI->GetEngine()->GetStringRec(text.c_str(),R);

	// If the line is out of the rect
	// break it in-between words
	if(R.right >= m_rect.right - m_rect.left)
	{
		// If a space has not been entered
		if(m_spacePos == -1)
		{
			// set the break pos at the end of the line.
			m_spacePos = text.size() - 1;
		}

		// insert the break point.
		text.insert(m_spacePos,"\n");
		
		// clear the break point for the next line.
		m_spacePos = -1;
	}
}

void TextBox::Enter()
{
	Write("");
}

void TextBox::Update(IKMInput* pInput)
{
	if(pInput->IsKeyDown())
	{
		char key = pInput->GetKeyDown();

		if(key == 13)
		{
			m_text += "\n";
		}
		else
		{
			m_text += key;
		}
	}
}

void TextBox::Render(IRenderer* pRenderer)
{
	m_square.Render(pRenderer);

	if(m_drawCarrot)
	{
		pRenderer->DrawString("|",m_carrotPos,0xffffffff);
	}

	// Iterate across all lines
	for(TextDataType::iterator iter = m_text.begin(); iter != m_text.end(); ++iter)
	{
		LineData& data = *iter;

		// scroll lines
		data.R.top += m_scrollSpeed;

		// cull lines that are not on the screen
		const RECT& R = m_rect;

		// If the line is on the screen
		if(data.R.top >= R.top)
		{
			if(data.R.top <= R.bottom)
			{
				// draw it
				pRenderer->DrawString(data.line.c_str(),data.R,data.color,false);
			}
		}
	}
}

void TextBox::UpdateScrolling(IKMInput* pInput)
{
	// todo: need to implement auto scroll

	// Scroll Text Box using MouseY
	if(IsClicked(pInput,&m_square))
	{
		int dy = pInput->MouseY();

		if(dy > 1)
		{
			ScrollUp(dy);
		}
		else if(dy < -1)
		{
			ScrollDown(-dy);
		}
		else
		{
			ScrollStop();
		}
	}
	else if(m_fScrollTime > 0.1f)
	{
		m_fScrollTime = 0.0f;
		m_scrollSpeed /= 2;
	}

}
*/
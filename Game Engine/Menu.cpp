
#include "Menu.h"
#include "IRenderer.h"
#include "Game.h"

#include <sstream>


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

void Menu::Update(GUI* pGUI, IKMInput* pInput, double dt)
{
	if(pInput->KeyDown(ESCAPE))
	{
		if(m_pPrev != nullptr)
		{
			pGUI->SetMenu(m_pPrev);
		}
	}

	// todo: fix this
	if(pInput->KeyDown(TAB))
	{
		m_elements[pGUI->m_uiCurrentIndex]->Deselect();
		pGUI->m_uiCurrentIndex = ((pGUI->m_uiCurrentIndex + 1) % m_elements.size());
		m_elements[pGUI->m_uiCurrentIndex]->Select();
	}
	else if(pInput->KeyDown(ENTER))
	{
		if(pGUI->m_uiCurrentIndex < m_elements.size())
		{
			m_elements[pGUI->m_uiCurrentIndex]->Enter();
		}
	}
	else
	{
		for(unsigned int i = 0; i < m_elements.size(); ++i)
		{
			m_elements[i]->Update(pInput,dt);
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

void GUI::Update(IKMInput* pInput, double dt)
{
	m_pMenu->Update(this,pInput,dt);
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

	m_uiCurrentIndex = 0;
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


// textbox ctor
TextBox::TextBox(const std::string& name, const RECT& R)
: m_spacePos(-1), m_square(R), m_scrollSpeed(0), m_scrollAccel(0), m_fScrollTime(0.0)
{
	//m_text.reserve(10);
	Enter();

	m_carrotPos.x = m_carrotPos.y = 0;
}

void TextBox::Write(const std::string& line, DWORD color)
{
	

	// calculate the rect of the line
	RECT R;
	if(!m_text.empty())
	{
		R =  m_text.back().R;
		//R.top += 15;
		//R.bottom += 15;
		//R.right += 500;
		// calculate the # of rows in the textbox
		R.top += 15*GetNumberOfRowsInLine(m_text.back().line);
	}
	else
	{
		R = m_square.GetRect();
	}
	
	// Add line to vector
	m_text.push_back(LineData(string("> ") + line,color,R));
}

void TextBox::Backspace()
{
	// reference to the line
	std::string& text = m_text.back().line;
	size_t size = text.size(); 

	// if the current line has text to remove(excluding the '>')
	if(size > 2)
	{
		// remove last character
		text.resize(size - 1);
	}
	// If there is no text to remove, try to pop the line in the vector
	else if(m_text.size() > 1)
	{
		m_text.pop_back();
	}
}

void TextBox::AddKey(char Key)
{
	// Add key to the last line
	std::string& text = m_text.back().line;
	text += Key;

	//m_text.back().R.right += 6;
}

void TextBox::Enter()
{
	Write("");
}

void TextBox::Update(IKMInput* pInput, double dt)
{

	if(pInput->KeyDown(BACKSPACE))
	{
		Backspace();
	}

	if(pInput->KeyDown(ENTER))
	{
		Enter();
	}

	if(pInput->IsKeyDown())
	{
		char key = pInput->GetKeyDown();
		AddKey(key);
	}
	

	UpdateScrolling(pInput,dt);
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

		// cull lines that are not on the screen
		const RECT& R = m_square.GetRect();

		/*pRenderer->GetStringRec(data.line.c_str(),data.R);
		if(data.R.right < R.right - 10)
		{
			data.R.right += 20;
		}*/

		// scroll lines
		data.R.top += m_scrollSpeed;

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

void TextBox::UpdateScrolling(IKMInput* pInput, double dt)
{
	// Use the scroll on the mouse to scroll the textbox
	m_scrollSpeed = pInput->MouseZ()*8;

	// Scroll Text Box using MouseY
	/*if(IsClicked(pInput,&m_square))
	{
		//m_scrollAccel = -pInput->MouseY() * 10;
		m_scrollSpeed = -pInput->MouseY()*2;

		if(dy >= 1)
		{
			ScrollUp(-dy);
		}
		else if(dy <= -1)
		{
			ScrollDown(dy);
		}
		else
		{
			ScrollStop();
		}
	}
	else
	{
		m_fScrollTime += dt;

		if(m_fScrollTime > .3)
		{
			m_fScrollTime = 0;
			m_scrollSpeed = 0;
		}
		//m_scrollSpeed -= 1.0*dt;
	}*/

}

unsigned int TextBox::GetNumberOfRowsInLine(const std::string& line) const
{
	unsigned int uiStrLength = line.length();

	if(uiStrLength == 0)
	{
		uiStrLength = 1;
	}

	// Get the rectangle for the textbox
	const RECT& squareRect = m_square.GetRect();

	unsigned int n = count(line.begin(),line.end(),'\n') / 1.2f;

	// todo: the value '6' here should be modifiable in the ctor, because there could be different size font
	// which is not yet implemented yet.
	unsigned int r = (unsigned int)ceil((float)(6*uiStrLength) / ((squareRect.right - squareRect.left)));

	return (r + n);
}

void TextBox::CLS()
{
	m_text.clear();
	//TextBox::Enter();
}


ScriptingConsole::ScriptingConsole(asVM* pVM, const std::string& name, const RECT& R)
: TextBox(name,R), m_pVM(pVM), m_uiStartIndex(-1)
{

}

void ScriptingConsole::Enter()
{
	string line = this->m_text.back().line;
	line.erase(line.begin());
	line.erase(line.begin());

	if(line == "start")
	{
		m_uiStartIndex = m_text.size();
	}
	else if(line == "end")
	{
		string sum;

		for(unsigned int i = m_uiStartIndex; i < m_text.size() - 1; ++i)
		{
			string temp = m_text[i].line;
			temp.erase(temp.begin());

			sum += temp;
		}

		m_pVM->ExecuteScript(sum,0xffffffff);

		// reset the index
		m_uiStartIndex = -1;
	}
	else if(m_uiStartIndex == -1)
	{
		line = "console.grab(" + line + ")";

		m_pVM->ExecuteScript(line,0xffffffff);
	}
	TextBox::Enter();
}

void ScriptingConsole::Backspace()
{
	if(m_text.back().line.size() > 2)
	{
		TextBox::Backspace();
	}
}

void ScriptingConsole::MessageCallback(const asSMessageInfo *msg)
{
	// asMSGTYPE_ERROR, asMSGTYPE_WARNING, asMSGTYPE_INFORMATION
	const unsigned int COLOR[3] = {0xffff0000,0xffffff00,0xffffffff};

	Write(std::string(msg->message),COLOR[msg->type]);
}

void ScriptingConsole::Grab()
{
}

void ScriptingConsole::Grab(const std::string& str)
{
	_Grab(str);
}

void ScriptingConsole::Grab(int i)
{
	_Grab(i);
}

void ScriptingConsole::Grab(float f)
{
	_Grab(f);
}

void ScriptingConsole::Grab(double d)
{
	_Grab(d);
}

void ScriptingConsole::Grab(unsigned int ui)
{
	_Grab(ui);
}

void ScriptingConsole::Grab(bool b)
{
	_Grab(b);
}

void ScriptingConsole::RegisterScript()
{
	auto asEngine = m_pVM->GetScriptEngine();

	//asEngine->SetDefaultAccessMask(0xffffffff);
	DBAS(asEngine->RegisterObjectType("Console",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(asEngine->RegisterObjectMethod("Console","void write(const string& in, uint color = 0xffffffff)",asMETHOD(ScriptingConsole,Write),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void clear()",asMETHOD(ScriptingConsole,CLS),asCALL_THISCALL));

	DBAS(asEngine->RegisterObjectMethod("Console","void grab(void)",asMETHODPR(ScriptingConsole,Grab,(void),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void grab(const string& in)",asMETHODPR(ScriptingConsole,Grab,(const std::string&),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void grab(int)",asMETHODPR(ScriptingConsole,Grab,(int),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void grab(float)",asMETHODPR(ScriptingConsole,Grab,(float),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void grab(double)",asMETHODPR(ScriptingConsole,Grab,(double),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void grab(uint)",asMETHODPR(ScriptingConsole,Grab,(unsigned int),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void grab(bool)",asMETHODPR(ScriptingConsole,Grab,(bool),void),asCALL_THISCALL));
	
	DBAS(asEngine->RegisterGlobalProperty("Console console",this));

	asEngine->Release();
}
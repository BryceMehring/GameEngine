
#include "Menu.h"
#include "IRenderer.h"
#include "Game.h"
#include "FileManager.h"
#include "VecMath.h"
#include "GameConstants.h"

#include <sstream>



using namespace std;

//const RTTI Menu::s_rtti("Menu");
RTTI_IMPL(Menu);

bool IsClicked(IKMInput& input, DxPolygon* pPolygon)
{
	if(input.MouseClick(0))
	{
		return pPolygon->IsPointInPolygon(input.MousePos());
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
	if(pPolygon != nullptr)
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

void Menu::Update(GUI* pGUI, IKMInput& input, double dt)
{
	if(input.KeyDown(ESCAPE))
	{
		if(m_pPrev != nullptr)
		{
			pGUI->SetMenu(m_pPrev);
		}
	}

	// todo: fix this
	if(input.KeyDown(TAB))
	{
		m_elements[pGUI->m_uiCurrentIndex]->Deselect();
		pGUI->m_uiCurrentIndex = ((pGUI->m_uiCurrentIndex + 1) % m_elements.size());
		m_elements[pGUI->m_uiCurrentIndex]->Select();
	}
	else if(input.KeyDown(ENTER))
	{
		if(pGUI->m_uiCurrentIndex < m_elements.size())
		{
			m_elements[pGUI->m_uiCurrentIndex]->Trigger();
		}
	}
	
	// fixed bug in textbox
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		m_elements[i]->Update(input,dt);
	}
	
}
void Menu::Render(IRenderer& renderer)
{
	// Render menu border and header
	renderer.Get2DRenderer().DrawString(m_menuTitle.c_str(),m_point,0xffffffff);

	if(m_pPolygon)
	{
		m_pPolygon->Render(renderer);
	}

	// Render all of the elements
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		m_elements[i]->Render(renderer);
	}
}

GUI::GUI(Menu* pMenu) : m_pMenu(pMenu), m_uiCurrentIndex(0), m_bSetMenu(false)
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

void GUI::Update(IKMInput& input, double dt)
{
	if(m_bSetMenu)
	{
		input.SetMouseState(Default);

		m_bSetMenu = false;
	}

	m_pMenu->Update(this,input,dt);
}

void GUI::Render(IRenderer& renderer)
{
	if(m_pMenu)
	{
		m_pMenu->Render(renderer);
	}
}

void GUI::SetMenu(Menu* pMenu)
{
	if(pMenu != nullptr)
	{
		m_pMenu = pMenu;
		m_bSetMenu = true;
		m_uiCurrentIndex = 0;
	}
}

Menu* GUI::GetMenu()
{
	return m_pMenu;
}

ButtonBase::ButtonBase(const Text& name, DxPolygon* pPolygon) :
m_name(name), m_pPolygon(pPolygon), m_color(-1), m_bMouseHover(false)
{
}

ButtonBase::~ButtonBase()
{
	delete m_pPolygon;
}

void ButtonBase::Update(IKMInput& input)
{
	// todo: I could optimize this by checking first if the mouse is moving
	// then move on to check if the mouse is colliding with the button
	if(m_pPolygon->IsPointInPolygon(input.MousePos()))
	{
		m_color = 0xffffff00;
		input.SetMouseState(::MouseCursorState::Hand);

		m_bMouseHover = true;
	}
	else if(m_bMouseHover)
	{
		m_color = 0xffffffff;

		input.SetMouseState(::MouseCursorState::Default);

		m_bMouseHover = false;
	}
}

void ButtonBase::Render(IRenderer& renderer)
{
	renderer.Get2DRenderer().DrawString(m_name.name.c_str(),m_name.P,m_color);

	m_pPolygon->Render(renderer);
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
TextBox::~TextBox()
{
}

void TextBox::SaveToFile(const std::string& file) const
{
	::fstream out(file.c_str(),ios::out);

	if(out.is_open())
	{
		for(unsigned int i = 0; i < m_text.size(); ++i)
		{
			out << m_text[i].line.c_str() << endl;
		}

		out.close();
	}
}

void TextBox::Write(const std::string& line, DWORD color, bool c)
{
	// todo: everything must go on its own line
	// The renderer does not cull the way I want it to.
	/*unsigned int pos = line.find('\n');
	unsigned int start = 0;
	while(pos != string::npos)
	{
		Write(line.substr(start,pos - start),color,c);

		start = pos + 1;
		pos = line.find('\n',pos + 1);
	
	}*/



	// calculate the rect of the line
	POINT P;
	if(!m_text.empty())
	{
		P =  m_text.back().P;
		P.y += 15;
	}
	else
	{
		RECT R = m_square.GetRect();
		P.x = R.left;
		P.y = R.top;
	}
	
	// Add line to vector
	m_text.push_back(LineData(line,color,P,c));
}

void TextBox::Backspace()
{
	// reference to the line
	std::string& text = m_text.back().line;
	size_t size = text.size(); 

	// if the current line has text to remove(excluding the '>')
	if(size > 0)
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

void TextBox::Update(IKMInput& input, double dt)
{
	if(/*input.MouseClick(0,false) &&*/ PtInRect(&m_square.GetRect(),input.MousePos()) > 0)
	{
		input.SetMouseState(Beam);
	}
	else
	{
		input.SetMouseState(Default);
	}


	if(input.KeyDown(BACKSPACE))
	{
		Backspace();
	}

	if(input.KeyDown(ENTER))
	{
		Enter();
	}

	/*if(input.KeyDown(TAB))
	{
		// todo: fix this implementation
		for(int i = 0; i < 3; ++i)
		{
			AddKey(' ');
		}
	}*/

	if(input.IsKeyDown())
	{
		char key = input.GetKeyDown();
		AddKey(key);
	}

	//m_carrotPos = this->m_text.back().P;
	//m_carrotPos.x += m_text.back().line.size() * 8;

	UpdateScrolling(input,dt);
}

void TextBox::BreakLastLine(IRenderer& renderer)
{
	TextBox::LineData& theBack = m_text.back();
	RECT myR = {theBack.P.x,theBack.P.y,0,0};
	renderer.Get2DRenderer().GetStringRec(theBack.line.c_str(),myR);

	if(myR.right >= (this->m_square.GetRect().right - 15))
	{
		TextBox::Write("",0xffffffff,true);
	}
}

void TextBox::Render(IRenderer& renderer)
{
	m_square.Render(renderer);

	/*if(m_drawCarrot)
	{
		pRenderer->DrawString("|",m_carrotPos,0xffffffff);
	}*/

	BreakLastLine(renderer);

	// Iterate across all lines
	for(TextDataType::iterator iter = m_text.begin(); iter != m_text.end(); ++iter)
	{
		LineData& data = *iter;

		// cull lines that are not on the screen
		const RECT& R = m_square.GetRect();

		// scroll lines
		data.P.y += static_cast<int>(m_scrollSpeed);

		// If the line is on the screen
		if(data.P.y >= R.top)
		{
			if(data.P.y <= R.bottom - 15) 
			{
				std::string line = data.line;

				// Render the '>'
				if(!data.bContinue)
				{
					line = "> " + data.line;
				}

				// draw it
				renderer.Get2DRenderer().DrawString(line.c_str(),data.P,data.color);
				//pRenderer->DrawString(line.c_str(),temp,data.color,false);
			}
		}
	}
}

void TextBox::UpdateScrolling(IKMInput& input, double dt)
{
	static int lastPos = 0;
	m_scrollSpeed = 0;
	
	if(input.MouseClick(0,false))
	{
		m_scrollSpeed = -input.MouseY();
		//lastPos = newPos;
		//m_scrollSpeed = pInput->MousePos().y;// - this->m_text.back().P.y;
	}
	else
	{
		// Use the scroll on the mouse to scroll the textbox
		m_scrollSpeed = input.MouseZ()*8;
	}

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

void TextBox::CLS()
{
	m_text.clear();

	//TextBox::Enter();
}


ScriptingConsole::ScriptingConsole(asVM* pVM, const std::string& name, const RECT& R)
: TextBox(name,R), m_pVM(pVM), m_uiStartIndex(-1), m_uiBackIndex(0)
{

}

ScriptingConsole::~ScriptingConsole()
{
	SaveToFile(Constants::SCRIPTFILE);
}

bool ScriptingConsole::IsBlocked() const
{
	return m_uiStartIndex != -1;
}

void ScriptingConsole::Update(IKMInput& input, double dt)
{
	if(m_text.size() > 1)
	{
		// Check if the up or down arrow keys were pressed
		bool bUp = input.KeyDown(UP);
		bool bDown = input.KeyDown(DOWN);

		// if either of them were pressed
		if(bUp || bDown)
		{
			// go back a line
			m_text.back().line = m_text[m_uiBackIndex].line;

			// clamp index into range
			m_uiBackIndex = Math::Clamp(bUp ? (m_uiBackIndex - 1) : (m_uiBackIndex + 1),(unsigned int)0,m_text.size() - 2);
			
		}
	}

	TextBox::Update(input,dt);
}

void ScriptingConsole::Enter()
{
	// the last line
	const string& backLine = m_text.back().line;

	// if the user wants to start entering block code
	if(backLine == "start" /*|| backLine == "function"*/)
	{
		// mark the index
		m_uiStartIndex = m_text.size();
	}
	else if((backLine == "end") && IsBlocked())
	{
		// the user is done entering code
		// sum up all of the lines and execute the code

		string sum;

		const unsigned int uiMax = m_text.size() - 1;
		for(unsigned int i = m_uiStartIndex; i < uiMax; ++i)
		{
			sum += m_text[i].line;
		}

		m_pVM->ExecuteScript(sum,0xffffffff);

		// reset the index
		m_uiStartIndex = -1;
	}
	else if(!IsBlocked())
	{
		// single command line 

		// wrap code into console.grab() so that we can display the output
		string line = "console.write(" + backLine + ")";

		m_uiBackIndex = m_text.size();

		m_pVM->ExecuteScript(line,0xffffffff);
	}

	TextBox::Write("",0xffffffff,IsBlocked());
}

void ScriptingConsole::Backspace()
{
	LineData& line = m_text.back();

	if((IsBlocked() && (m_text.size() - 1 > m_uiStartIndex)) || line.line.size() > 0)
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

void ScriptingConsole::CLS()
{
	TextBox::CLS();

	m_uiBackIndex = 0;
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
	//DBAS(asEngine->RegisterObjectMethod("Console","void write(const string& in, uint color = 0xffffffff, bool c = false)",asMETHOD(ScriptingConsole,Write),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void clear()",asMETHOD(ScriptingConsole,CLS),asCALL_THISCALL));

	DBAS(asEngine->RegisterObjectMethod("Console","void write(void)",asMETHODPR(ScriptingConsole,Grab,(void),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void write(const string& in)",asMETHODPR(ScriptingConsole,Grab,(const std::string&),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void write(int)",asMETHODPR(ScriptingConsole,Grab,(int),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void write(float)",asMETHODPR(ScriptingConsole,Grab,(float),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void write(double)",asMETHODPR(ScriptingConsole,Grab,(double),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void write(uint)",asMETHODPR(ScriptingConsole,Grab,(unsigned int),void),asCALL_THISCALL));
	DBAS(asEngine->RegisterObjectMethod("Console","void write(bool)",asMETHODPR(ScriptingConsole,Grab,(bool),void),asCALL_THISCALL));

	DBAS(asEngine->RegisterGlobalProperty("Console console",this));

	asEngine->Release();
}
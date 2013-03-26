
#include "Menu.h"
#include "IRenderer.h"
#include "Game.h"
#include "FileManager.h"
#include "VecMath.h"
#include "GameConstants.h"

#include <sstream>
#include <glm\gtx\transform.hpp>

using namespace std;

RTTI_IMPL(Menu);


Menu::Menu() : m_pPrev(nullptr)
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
	
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		m_elements[i]->Update(input,dt);
	}
	
}
void Menu::Render(IRenderer& renderer)
{
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

ButtonBase::ButtonBase(const Math::Sprite& s, const std::string& str) : m_sprite(s), m_text(str), m_bSelected(false), m_bMouseHover(false)
{
}

ButtonBase::~ButtonBase()
{
	//delete m_pPolygon;
}

void ButtonBase::Update(IKMInput& input)
{
	// todo: I could optimize this by checking first if the mouse is moving
	// then move on to check if the mouse is colliding with the button
	if(m_sprite.IsPointWithin(input.GetTransformedMousePos()))
	{
		m_bMouseHover = true;
	}
	else
	{
		m_bMouseHover = false;
	}
}

void ButtonBase::Render(IRenderer& renderer)
{
	//TextureInfo buttonInfo;
	//renderer.GetResourceManager().GetTextureInfo("button",buttonInfo);

	float scale = (m_bMouseHover || m_bSelected) ? 1.5f : 1.0f;

	glm::vec2 pos = m_sprite.Middle();
	glm::mat4 S = glm::scale(m_sprite.Width() * scale,m_sprite.Height() * scale,1.0f);
	glm::mat4 T = glm::translate(pos.x,pos.y,0.0f);

	renderer.DrawSprite(S*T,"button");
	renderer.DrawString(m_text.c_str(),glm::vec2(m_sprite.topLeft.x + 4.0f,pos.y),0,::glm::vec4(1.0f,1.0f,1.0f,1.0f),glm::vec2(scale,scale));


	//renderer.Get2DRenderer().DrawString(m_name.name.c_str(),m_name.P,m_color);

	//renderer.Get2DRenderer().DrawSprite(

	//m_pPolygon->Render(renderer);
}

// textbox ctor
TextBox::TextBox(const std::string& name, const glm::vec2& pos, float w, float h)
: m_spacePos(-1), m_fScrollTime(0.0), m_sprite(w,h,pos), m_pos(-40.0f,40.0f)
{
	m_text.push_back(LineData());

	Enter();

	//m_carrotPos.x = m_carrotPos.y = 0;
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

void TextBox::Write(const std::string& line, const glm::vec4& color, bool c)
{
	m_text.push_back(LineData(line,color,m_sprite,c));
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
}

void TextBox::Enter()
{
	Write(string(""),glm::vec4(1.0f,1.0f,1.0f,1.0f));
}

void TextBox::Update(IKMInput& input, double dt)
{
	/*if(m_sprite.IsPointWithin(input.GetTransformedMousePos()))
	{
		input.SetMouseState(Beam);
	}
	else
	{
		input.SetMouseState(Default);
	}*/


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
	/*TextBox::LineData& theBack = m_text.back();
	RECT myR = {theBack.P.x,theBack.P.y,0,0};
	renderer.Get2DRenderer().GetStringRec(theBack.line.c_str(),myR);

	if(myR.right >= (this->m_square.GetRect().right - 15))
	{
		TextBox::Write("",0xffffffff,true);
	}*/
}

void TextBox::Render(IRenderer& renderer)
{
	/*if(m_drawCarrot)
	{
		pRenderer->DrawString("|",m_carrotPos,0xffffffff);

	}*/

	glm::vec2 middle = m_sprite.Middle();
	glm::mat4 S = glm::scale(m_sprite.Width(),m_sprite.Height(),1.0f);
	glm::mat4 T = glm::translate(middle.x,middle.y,3000.0f);

	glm::vec2 pos = m_pos;

	// Iterate across all lines
	for(auto iter = m_text.begin(); iter != m_text.end(); ++iter)
	{
		LineData& data = *iter;

		renderer.DrawString(">",pos);
		renderer.DrawString((" " + data.line).c_str(),pos,0,data.color);

		Math::FRECT R;
		renderer.GetStringRec((/*"> " + */data.line).c_str(),::glm::vec2(0.0f,0.0f),glm::vec2(1.0f,1.0f),R);

		pos.y -= R.Height();
	}
}

void TextBox::UpdateScrolling(IKMInput& input, double dt)
{
	if(input.MouseClick(0,false))
	{
		//m_scrollSpeed = input.MouseY() / 8.0f;
		//lastPos = newPos;
		//m_scrollSpeed = pInput->MousePos().y;// - this->m_text.back().P.y;
	}
	else
	{
		m_pos.y += 2.0f*input.MouseZ();
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


ScriptingConsole::ScriptingConsole(asVM* pVM, const std::string& name, const glm::vec2& pos, float w, float h)
: TextBox(name,pos,w,h), m_pVM(pVM), m_uiStartIndex(-1), m_uiBackIndex(0)
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
			m_uiBackIndex = glm::clamp(bUp ? (m_uiBackIndex - 1) : (m_uiBackIndex + 1),(unsigned int)0,m_text.size() - 2);
			
		}
	}

	TextBox::Update(input,dt);
}

void ScriptingConsole::Enter()
{
	// the last line
	const string& backLine = m_text.back().line;

	// if the user wants to start entering block code
	if(backLine == "start" || backLine == "function")
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

	TextBox::Enter();

	//TextBox::Write("",0xffffffff,IsBlocked());
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
	const glm::vec4 COLOR[3] = { glm::vec4(1.0f,0.0f,0.0f,1.0f),glm::vec4(1.0f,1.0f,0.0f,1.0f),glm::vec4(1.0f,1.0f,1.0f,1.0f) };

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
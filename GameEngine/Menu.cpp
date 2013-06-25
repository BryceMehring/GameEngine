
#include "Menu.h"
#include "IRenderer.h"
#include "Game.h"
#include "FileManager.h"
#include "VecMath.h"
#include "GameConstants.h"

#include <sstream>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>

using namespace std;

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
		m_menus.push_back(pMenu);
	}
}

void Menu::AddElement(IUIElement* pElement)
{
	if(pElement != nullptr)
	{
		pElement->AddRef();
		m_elements.push_back(pElement);
	}
}

void Menu::Update(GUI* pGUI, IKMInput& input, double dt)
{
	if(input.KeyDown(GLFW_KEY_SPACE))
	{
		if(m_pPrev != nullptr)
		{
			pGUI->SetMenu(m_pPrev);
		}
	}

	if(input.KeyDown(GLFW_KEY_TAB))
	{
		unsigned int index = pGUI->GetIndex();

		m_elements[index]->Deselect();
		index = (index + 1) % m_elements.size();
		m_elements[index]->Select();

		pGUI->SetIndex(index);

	}
	else if(input.KeyDown(GLFW_KEY_ENTER))
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
void Menu::Render(IRenderer& renderer)
{
	// Render all of the elements
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		m_elements[i]->Render(renderer);
	}
}

GUI::GUI(Menu* pMenu) : m_pMenu(pMenu), m_uiCurrentIndex(0)
{
}

GUI::~GUI()
{
	m_pMenu->Release();
}

GUI::ChangeMenuCallback GUI::CreateCallback()
{
	return ChangeMenuCallback(this,&GUI::SetMenu);
}

void GUI::Update(IKMInput& input, double dt)
{
	if(m_pMenu != nullptr)
	{
		m_pMenu->Update(this,input,dt);
	}
}

void GUI::Render(IRenderer& renderer)
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
		m_pMenu->AddRef();
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

ButtonBase::ButtonBase(const Math::FRECT& s, const glm::vec3& defaultColor,
					   const glm::vec3& hoverColor, const glm::vec2& scale, const std::string& str) :
					   m_sprite(s), m_defaultColor(defaultColor), m_hoverColor(hoverColor), m_scale(scale),
					   m_text(str),  m_bMouseHover(false), m_bSelected(false) {}

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

void ButtonBase::Select()
{
	m_bSelected = true;
}

void ButtonBase::Deselect()
{
	m_bSelected = false;
}

void ButtonBase::Render(IRenderer& renderer)
{
	glm::vec3 color = (m_bMouseHover || m_bSelected) ? m_hoverColor : m_defaultColor;

	glm::vec2 pos = m_sprite.Middle();
	glm::mat4 T = glm::translate(pos.x,pos.y,0.0f);
	T = glm::scale(T,m_sprite.Width(),m_sprite.Height(),1.0f);

	renderer.DrawSprite("button",T);
	renderer.DrawString(m_text.c_str(),glm::vec2(pos.x - (m_sprite.Width() / 2.0f),pos.y),m_scale,color);
}

// textbox ctor
TextBox::TextBox(const std::string& name, const glm::vec2& pos, float w, float h)
	: m_sprite(w,h,pos), m_spacePos(-1), m_fScrollTime(0.0), m_pos(-40.0f,40.0f)
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

void TextBox::Write(const std::string& line, const glm::vec3& color, bool c)
{
	m_text.push_back(LineData(line,color,m_sprite,c));
}

void TextBox::Backspace()
{
	// reference to the line
	std::string& text = m_text.back().line;
	size_t size = text.size();

	// if the current line has text to remove
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
	Write(string(""),glm::vec3(1.0f));
}

void TextBox::Update(IKMInput& input, double dt)
{
	if(input.KeyDown(GLFW_KEY_BACKSPACE))
	{
		Backspace();
	}
	else if(input.KeyDown(GLFW_KEY_ENTER))
	{
		Enter();
	}
	else
	{
		char inputChar;
		if(input.CharKeyDown(inputChar))
		{
			AddKey(inputChar);
		}
	}

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

	//glm::vec2 middle = m_sprite.Middle();
	//glm::mat4 S = glm::scale(m_sprite.Width(),m_sprite.Height(),1.0f);
	//glm::mat4 T = glm::translate(middle.x,middle.y,3000.0f);

	glm::vec2 pos = m_pos;

	// Iterate across all lines
	for(auto iter = m_text.begin(); iter != m_text.end(); ++iter)
	{
		const LineData& data = *iter;

		renderer.DrawString(("> " + data.line).c_str(),pos,glm::vec2(2.0f),data.color);

		pos.y -= 10.0f;
	}
}

void TextBox::UpdateScrolling(IKMInput& input, double dt)
{
	if(input.MouseClick(0,false))
	{
		m_pos.y = input.GetTransformedMousePos().y;

		//lastPos = newPos;
		//m_scrollSpeed = pInput->MousePos().y;// - this->m_text.back().P.y;
	}
	else
	{
		//m_pos.y += 2.0f*input.MouseZ();
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


/*ScriptingConsole::ScriptingConsole(asVM* pVM, const std::string& name, const glm::vec2& pos, float w, float h)
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
}*/

Slider::Slider(const glm::vec2& start,
			   const glm::vec2& end,
			   float min, float max, const std::string& tex, const DELEGATE& callback) :
	m_start(start), m_end(end), m_fPercentage(min), m_callback(callback),
	m_bEnable(false), m_bUpdateEnable(false), m_fMin(min), m_fMax(max), m_SpriteTexture(tex)
{
	m_pos.y = start.y;
	m_pos.x = (end.x + start.x) / 2.0f;
	Trigger();
}

void Slider::Select()
{
	m_bEnable = true;
}

void Slider::Deselect()
{
	m_bEnable = false;
}

void Slider::Trigger()
{
	// todo: this could be moved somewhere else
	float length = glm::length(m_end - m_start);

	float distance = glm::length(m_start - m_pos);
	m_fPercentage = (m_fMax - m_fMin) * distance / length + m_fMin;
	m_callback.Call(m_fPercentage);
}

void Slider::Update(IKMInput& input, double dt)
{
	if(input.MouseClick(GLFW_MOUSE_BUTTON_1))
	{
		Math::FRECT R(m_end.x - m_start.x,8.0f,(m_end + m_start) / 2.0f);
		const glm::vec2& mousePos = input.GetTransformedMousePos();
		if(R.IsPointWithin(input.GetTransformedMousePos()))
		{
			m_bUpdateEnable = true;
		}
	}
	else if(input.MouseRelease(GLFW_MOUSE_BUTTON_1))
	{
		m_bUpdateEnable = false;
	}

	if(m_bUpdateEnable)
	{
		const glm::vec2& mousePos = input.GetTransformedMousePos();
		
		// Move the slider to the mouse pos
		m_pos.x = mousePos.x;
		m_pos.x = glm::clamp(m_pos.x,m_start.x,m_end.x);

		Trigger();
	}
}

void Slider::Render(IRenderer& renderer)
{
	glm::vec3 line[2] =
	{
		glm::vec3(m_start.x,m_start.y,0.0f),
		glm::vec3(m_end.x,m_end.y,0.0f)
	};

	glm::mat4 T = glm::translate(m_pos.x,m_pos.y,0.0f);
	T = glm::scale(T,4.0f,4.0f,1.0f);

	std::stringstream stream;
	stream.precision(2);
	stream << std::fixed << m_fPercentage << endl;

	renderer.DrawSprite(m_SpriteTexture,T);
	renderer.DrawLine(line,2);
	renderer.DrawString(stream.str().c_str(),glm::vec2((m_end.x + m_start.x) / 2.0f - 10.0f,m_end.y - 10.0f),glm::vec2(2.0f));

}

float Slider::GetValue() const
{
	return m_fPercentage;
}

ProgressBar::ProgressBar(const glm::vec2& start, const glm::vec2& end, const DELEGATE& callback) : m_pos(start),
	m_start(start), m_end(end), m_callback(callback)
{
}

void ProgressBar::Select()
{
}
void ProgressBar::Deselect()
{
}
void ProgressBar::Trigger()
{
}

void ProgressBar::Update(class IKMInput&, double dt)
{
	if((m_end.x - m_pos.x) < 0.01f)
	{
		m_callback.Call();
	}
}
void ProgressBar::Render(IRenderer& renderer)
{
	glm::vec3 progStart[2] =
	{
		glm::vec3(m_start,0.0f),
		glm::vec3(m_pos,0.0f)
	};

	glm::vec3 progEnd[2] =
	{
		glm::vec3(m_pos,0.0f),
		glm::vec3(m_end,0.0f)
	};

	renderer.DrawLine(progStart,2,10.0f,glm::vec4(1.0f,0.0f,0.0f,1.0f));
	renderer.DrawLine(progEnd,2,10.0f);
}

void ProgressBar::SetProgress(float v)
{
	float dist = m_end.x - m_start.x;
	m_pos.x = m_start.x + dist * glm::clamp(v,0.0f,1.0f);
}


} // UI namespace

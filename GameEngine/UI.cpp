// Programmed by Bryce Mehring

#include "StdAfx.h"
#include "BEngine.h"
#include "UI.h"
#include "PluginManager.h"
#include "UICreator.h"
#include "EngineHelper.h"
#include "asRegisterer.h"
#include "asConsole.h"

using namespace std;

#pragma comment(lib,"d3dx9.lib")

// global functions
CheckBoxData* CheckBoxDataFactory()
{
	return new CheckBoxData();
}

UIData* UIDataFactory()
{
	return new UIData();
}
/*UITrigger* UITriggerFactory(IUIButton* p)
{
	return new UITrigger(p);
}*/

unsigned int UINode::AddElement(IUIElement* pElement)
{
	m_elements.push_back(pElement);
	return (m_elements.size() - 1);
}
void UINode::RemoveElement(unsigned int i)
{
	// todo: this is not the best structure for this task. switch data structures?
	if(i < m_elements.size())
	{
		delete m_elements.at(i);
		m_elements.erase(m_elements.begin() + i);
	}
}

void UINode::Update(float dt)
{
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		m_elements.at(i)->Update(dt);
	}

	for(unsigned int i = 0; i < m_Neighbors.size(); ++i)
	{
		// todo: need to implement
		// If this edge is being triggered
		UIEdge* pEdge = static_cast<UIEdge*>(m_Neighbors[i]);

		// Note: there is only one node that is updating at a time, because this is true
		// once a edge is triggered, another edge cannot be triggered that would override
		// the previous one.
		if(pEdge->IsTriggered())
		{
			UINode* pVert = static_cast<UINode*>(pEdge->GetHead());

			// the edges are created in the wrong order
			assert(pVert != this);

			// Switch to the next node.
			m_pUI->SetCurrentNode(pVert);

			break;
		}
	}
}
void UINode::Render(IRenderingPlugin& renderer)
{
	for(unsigned int i = 0; i < m_elements.size(); ++i)
	{
		// Check if the UI element can be rendered
		//IRender* pUIElement = reinterpret_cast<IRender*>(m_elements[i]);
		// this might be where it is slowing down
		IRender* pUIElement = dynamic_cast<IRender*>(m_elements[i]);

		if(pUIElement)
		{
			pUIElement->Render(renderer);
		}
	}
}

// IUIElement
IUIElement::IUIElement(UI* pUI) : m_pUI(pUI)
{

}

ClickableElement::ClickableElement(RECT* R, UI* pUI) : IUIElement(pUI), m_pClickRect(R) {}
ClickableElement::~ClickableElement()
{
	m_pClickRect = nullptr;
}

void ClickableElement::Update(float dt)
{
	IKMInput* pInput = m_pUI->GetEngine()->GetInput();

	m_ClickFSM.Run([&]() -> bool { return pInput->MouseClick(0); }, [&]() -> void 
	{
		POINT mousePos;
		pInput->MousePos(mousePos);

		// check if the box was clicked
		if(PtInRect(m_pClickRect,mousePos))
		{
			ClickResponse();
		}
	});
}


TextBox::~TextBox()
{
	m_pData->Release();
}

// CheckBox
 
CheckBox::CheckBox(CheckBoxData* pData, UI* pUI) : IUIButton(&pData->rect,pUI), m_pData(pData)
{
	m_pUI->GetEngine()->GetStringRec(m_pData->str.c_str(),m_pData->rect);
	m_box.ConstructFromRect(m_pData->rect);
}
CheckBox::~CheckBox()
{
	m_pData->Release();
}

bool CheckBox::IsChecked() const
{
	return m_pData->checked;
}
void CheckBox::ClickResponse()
{
	m_pData->checked = !m_pData->checked;

	asVM& vm = m_pUI->GetEngine()->GetScriptVM();
	vm.ExecuteScriptFunction(m_pData->func.iScriptIndex,m_pData->func.ifuncId);
	//vm.ExecuteScriptFunction(m_pData->func.iScriptIndex,m_pData->func.ifuncId,m_pData->checked);
}

void CheckBox::Render(IRenderingPlugin& renderer)
{
	DWORD color;

	if(m_pData->checked)
	{
		m_box.SetColor(D3DCOLOR_XRGB(0,255,0));
	}
	else
	{
		m_box.SetColor(D3DCOLOR_XRGB(255,255,255));
	}

	m_box.Render(renderer);

	renderer.DrawString(m_pData->str.c_str(),m_pData->rect,0xffffffff,false);
}

// constructor
TextBox::TextBox(UIData* pData, UI* pUI) : ClickableElement(&pData->rect,pUI), m_pData(pData),
m_bFocused(false), m_scrollSpeed(0), m_drawCarrot(true), m_fTime(0.0f), m_spacePos(-1)
{
	// todo: need to organize
	ConstructFromRect(pData->rect);
	SetColor(0xffffffff);

	LineData data;
	data.line = pData->str;
	data.color = 0xffffffff;
	data.R = pData->rect;

	m_carrotPos.x = m_carrotPos.y = 0;

	m_text.push_back(data);
}

void TextBox::ClickResponse()
{
	m_bFocused = true;
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

void TextBox::ScrollDown()
{
	m_scrollSpeed = -SCROLL_SPEED;
}

void TextBox::ScrollUp()
{
	m_scrollSpeed = SCROLL_SPEED;
}
void TextBox::ScrollStop()
{
	m_scrollSpeed = 0;
}


void TextBox::Update(float dt)
{
	// if the TextBox is checked
	if(m_bFocused)
	{
		m_fTime += dt;

		BEngine* pEngine = m_pUI->GetEngine();
		IKMInput* pInput = pEngine->GetInput();

		UpdateTextInput(pInput);
		UpdateCarrot(pInput);
		UpdateScrolling(pInput);
	}
	else
	{
		ClickableElement::Update(dt);
	}
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
void TextBox::Enter()
{
	Write("");
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
	if(R.right >= (this->m_pData->rect.right - this->m_pData->rect.left))
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

IRender::IRenderType TextBox::GetRenderType() const { return Text; }
void TextBox::Render(IRenderingPlugin& renderer)
{
	DxSquare::Render(renderer);

	if(m_drawCarrot)
	{
		renderer.DrawString("|",m_carrotPos,0xffffffff);
	}

	// Iterate across all lines
	for(TextDataType::iterator iter = m_text.begin(); iter != m_text.end(); ++iter)
	{
		LineData& data = *iter;

		// scroll lines
		data.R.top += m_scrollSpeed;

		// cull lines that are not on the screen
		const RECT& R = m_pData->rect;

		// If the line is on the screen
		if(data.R.top >= R.top)
		{
			if(data.R.top <= R.bottom)
			{
				// draw it
				renderer.DrawString(data.line.c_str(),data.R,data.color,false);
			}
		}
	}

	// todo: need to draw the name of the TextBox: 	m_pData->str
	//renderer.DrawString(m_pData->str.c_str(),m_pData->rect,0xffffffff,false);
}
void TextBox::CLS()
{
	m_text.clear();
	Write("");
}
void TextBox::UpdateScrolling(IKMInput* pInput)
{
	if(pInput->KeyDown(DIK_DOWN))
	{
		ScrollDown();
	}
	else if(pInput->KeyDown(DIK_UP))
	{
		ScrollUp();
	}
	else
	{
		ScrollStop();
	}
}
void TextBox::UpdateCarrot(IKMInput* pInput)
{
	// todo: need to calc

	//m_carrotPos.x = m_text.back().R.left;
	m_carrotPos.y = m_text.back().R.top;

	if(m_fTime > 0.04f)
	{
		m_drawCarrot = !m_drawCarrot;
		m_fTime = 0.0f;

		if(pInput->KeyDown(DIK_RIGHT))
		{
			m_carrotPos.x += 6;
		}
		else if(pInput->KeyDown(DIK_LEFT))
		{
			m_carrotPos.x -= 6;
		}
	}
}

void TextBox::UpdateTextInput(IKMInput* pInput)
{
	// Get User input only once for text
	m_Textfsm.Run([=]() -> bool { return pInput->IsKeyDown(); }, [&]() -> void
	{
		char Key = pInput->GetKeyDown();

		switch(Key)
		{
		case 8:
			Backspace();
			break;
		case 13:
			Enter();
			break;
		default:
			AddKey(Key);
		}

		/*// backspace
		if(Key == 8)
		{
				
		}
		// enter
		else if(Key == 13)
		{
				// adds a new empty line
				
		}
		else
		{
			// Add the key to the end of the current line of the textbox
			AddKey(Key);				
		}*/
	});
}



// UI
UI::UI(BEngine* pEngine) : m_pEngine(pEngine), m_pInFocus(nullptr),
m_pCurrentNode(nullptr)
{
	m_factory.RegisterType("CheckBox",new CheckBoxCreator());
	m_factory.RegisterType("TextBox",new TextBoxCreator());

	m_pGraph = new UIGraph(this);

	RegisterScript();
}
UI::~UI()
{
	m_pGraph->Release();
}


BEngine* UI::GetEngine() const
{
	return m_pEngine;
}
UIGraph* UI::GetGraph() const
{
	m_pGraph->AddRef();
	return m_pGraph;
}

IUIElement* UI::CreateElement(const string& c,UIData* pObj)
{
	UICreator::TupleType t(pObj,this);

	// create object based on the c string and pass pData to the constructor
	return static_cast<IUIElement*>(m_factory.CreateEntity(c,t));
}

IUIElement* UI::GetFocus() const
{
	return m_pInFocus;
}
void UI::SetFocus(IUIElement* pElement)
{
	m_pInFocus = pElement;
}
void UI::SetCurrentNode(UINode* pNode)
{
	assert(pNode != NULL);

	m_pCurrentNode = pNode;
}
UINode* UI::GetCurrentNode() const
{
	assert(m_pCurrentNode != NULL);

	m_pCurrentNode->AddRef();
	return m_pCurrentNode;
}
void UI::Update(float dt)
{
	if(m_pInFocus == nullptr)
	{
		if(m_pCurrentNode != nullptr)
		{
			m_pCurrentNode->Update(dt);
		}
	}
	else
	{
		m_pInFocus->Update(dt);
	}
}
void UI::Render(IRenderingPlugin& renderer) const
{
	if(m_pCurrentNode)
	{
		m_pCurrentNode->Render(renderer);
	}
}

void UI::RegisterScript()
{
	// todo: this function should only be called once?

	// Register Script with UI 
	asVM& vm = m_pEngine->GetScriptVM();
	asIScriptEngine& engine = vm.GetScriptEngine();

	// ===== Register IUIButton =====
	DBAS(engine.RegisterObjectType("IUIButton",0,asOBJ_REF));
	RegisterRefCountingObject(IUIButton,engine);

	// ===== Register ITrigger =====
	DBAS(engine.RegisterObjectType("ITrigger",0,asOBJ_REF));
	RegisterRefCountingObject(ITrigger,engine);

	// ===== Register UITrigger =====
	DBAS(engine.RegisterObjectType("UITrigger",0,asOBJ_REF));

//	DBAS(engine.RegisterObjectBehaviour("UITrigger",asBEHAVE_FACTORY,"UITrigger@ f(IUIButton@)",
	//	asFUNCTION(UITriggerFactory),asCALL_CDECL));

	RegisterRefCountingObject(UITrigger,engine);
	RegisterDowncastObject(UITrigger,ITrigger,engine);


	// ===== Register IUIElement =====
	DBAS(engine.RegisterObjectType("IUIElement",0,asOBJ_REF));
	RegisterRefCountingObject(IUIElement,engine);
	//RegisterRefCountingObject<IUIElement>(engine,"IUIElement");

	// ===== Register UIData =====
	DBAS(engine.RegisterObjectType("UIData",0,asOBJ_REF));

	// factory
	RegisterFactoryObjectL(UIData,UIDataFactory,engine);

	RegisterRefCountingObject(UIData,engine);
	
	DBAS(engine.RegisterObjectProperty("UIData","RECT rect",offsetof(CheckBoxData,rect)));
	DBAS(engine.RegisterObjectProperty("UIData","string name",offsetof(CheckBoxData,str)));

	// ===== Register UINode =====
	DBAS(engine.RegisterObjectType("UINode",0,asOBJ_REF));
	RegisterRefCountingObject(UINode,engine);
	DBAS(engine.RegisterObjectMethod("UINode","uint AddElement(IUIElement@)",asMETHOD(UINode,AddElement),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UINode","void RemoveElement(uint)",asMETHOD(UINode,RemoveElement),asCALL_THISCALL));

	// ===== Register UIEdge =====
	DBAS(engine.RegisterObjectType("UIEdge",0,asOBJ_REF));
	RegisterRefCountingObject(UIEdge,engine);

	DBAS(engine.RegisterObjectMethod("UIEdge","void AddTrigger(ITrigger@)",asMETHOD(UIEdge,AddTrigger),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UIEdge","bool IsTriggered() const",asMETHOD(UIEdge,IsTriggered),asCALL_THISCALL));

	// ===== Register Graph =====

	DBAS(engine.RegisterObjectType("UIGraph",0,asOBJ_REF));

	RegisterRefCountingObject(UIGraph,engine);

	DBAS(engine.RegisterObjectMethod("UIGraph","uint GetVertexSize() const",asMETHOD(UIGraph::GraphBase,GetVertexSize),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UIGraph","uint GetEdgeSize() const",asMETHOD(UIGraph::GraphBase,GetEdgeSize),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UIGraph","UIEdge@ GetEdge(uint)",asMETHOD(UIGraph::GraphBase,GetEdge),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UIGraph","uint CreateEdge(UINode@,UINode@)",asMETHOD(UIGraph::GraphBase,GetEdge),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UIGraph","UINode@ AddVertex()",asMETHODPR(UIGraph,AddVertex,(void),UINode*),asCALL_THISCALL));

	
	// ===== Register CheckBoxData =====
	DBAS(engine.RegisterObjectType("CheckBoxData",0,asOBJ_REF));

	RegisterFactoryObjectL(CheckBoxData,CheckBoxDataFactory,engine);
	RegisterRefCountingObject(CheckBoxData,engine);

	RegisterDowncastObject(CheckBoxData,UIData,engine);
	DBAS(engine.RegisterObjectBehaviour("CheckBoxData",asBEHAVE_IMPLICIT_REF_CAST,"UIData@ f()",
		asFUNCTION((EngineHelper::refCast<CheckBoxData,UIData>)),asCALL_CDECL_OBJLAST));

	DBAS(engine.RegisterObjectProperty("CheckBoxData","bool checked",offsetof(CheckBoxData,checked)));
	DBAS(engine.RegisterObjectProperty("CheckBoxData","RECT rect",offsetof(CheckBoxData,rect)));
	DBAS(engine.RegisterObjectProperty("CheckBoxData","string name",offsetof(CheckBoxData,str)));
	DBAS(engine.RegisterObjectProperty("CheckBoxData","ScriptFunction func",offsetof(CheckBoxData,func)));

	// Register UI
	DBAS(engine.RegisterObjectType("UI",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(engine.RegisterObjectMethod("UI","IUIElement@ CreateElement(const string& in,UIData@)",asMETHOD(UI,CreateElement),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UI","UIGraph@ GetGraph() const",asMETHOD(UI,GetGraph),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UI","void SetCurrentNode(UINode@)",asMETHOD(UI,SetCurrentNode),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UI","UINode@ GetCurrentNode() const",asMETHOD(UI,GetCurrentNode),asCALL_THISCALL));
	DBAS(engine.RegisterGlobalProperty("UI ui",this));

	engine.Release();
}
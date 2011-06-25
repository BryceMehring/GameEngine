// Programmed by Bryce Mehring

#include "StdAfx.h"
#include "UI.h"
#include "PluginManager.h"
#include "UICreator.h"
#include "EngineHelper.h"

using namespace AngelScript;
using namespace std;

// global functions
CheckBoxData* CheckBoxDataFactory()
{
	return new CheckBoxData();
}

UIData* UIDataFactory()
{
	return new UIData();
}

IUIElement::IUIElement(UI* pUI) : m_pUI(pUI)
{

}

IUIElement::GameEntityType IUIElement::GetEntityType() const
{
	return GameEntityType::UIType;
}

ClickableElement::ClickableElement(RECT* R, UI* pUI) : IUIElement(pUI), m_pClickRect(R), m_fClickTime(0) {}
ClickableElement::~ClickableElement()
{
	m_pClickRect = nullptr;
}


// todo: maybe I could implement the fsm here. 
void ClickableElement::Update(float dt)
{
	IKMInput& input = m_pUI->GetEngine()->GetInput();

	if(input.MouseClick(0))
	{
		POINT mousePos;
		input.MousePos(mousePos);

		// check if the box was clicked
		if((mousePos.x >= m_pClickRect->left) && (mousePos.x <= m_pClickRect->right))
		{
			if((mousePos.y >= m_pClickRect->top) && (mousePos.y <= m_pClickRect->bottom))
			{
				if(m_fClickTime == 0.0f)
				{
					m_fClickTime += dt;

					// Call script function
					ClickResponse();
				}
			}
		}
	}
	else
	{
		m_fClickTime = 0.0f;
	}
}


TextBox::~TextBox()
{
	m_pData->Release();
}

// CheckBox
 
CheckBox::CheckBox(CheckBoxData* pData, UI* pUI) : ClickableElement(&pData->rect,pUI), m_pData(pData)
{
	IRenderingPlugin& renderer = m_pUI->GetEngine()->GetRenderer();
	renderer.GetStringRec(m_pData->str.c_str(),m_pData->rect);
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
	vm.ExecuteScriptFunction(m_pData->func.iScriptIndex,m_pData->func.ifuncId,m_pData->checked);
}

void CheckBox::Render()
{
	DWORD color;

	if(m_pData->checked)
	{
		color = D3DCOLOR_XRGB(0,255,0);
	}
	else
	{
		color = D3DCOLOR_XRGB(255,255,255);
	}

	IRenderingPlugin& renderer = m_pUI->GetEngine()->GetRenderer();
	renderer.DrawString(m_pData->str.c_str(),m_pData->rect,color,false);
}

// UI
UI::UI(IBaseEngine* pEngine) : m_pEngine(pEngine), m_pInFocus(nullptr)
{
	m_levels.resize(1);
	m_currentLevel = m_levels.begin();

	m_factory.RegisterType("CheckBox",new CheckBoxCreator());
	m_factory.RegisterType("TextBox",new TextBoxCreator());
}
UI::~UI()
{
	for(value_type::iterator iter = m_levels.begin(); iter != m_levels.end(); ++iter)
	{
		for(sub_type::iterator subIter = iter->begin(); subIter != iter->end(); ++subIter)
		{
			delete (*subIter);
		}
	}
}


IBaseEngine* UI::GetEngine() 
{
	return m_pEngine;
}

void UI::AddLevel()
{
	m_levels.push_back(std::vector<IUIElement*>());
}
// todo: need to add bound guards
void UI::Forward()
{
	m_currentLevel++;
}
void UI::Back()
{
	m_currentLevel--;
}
void UI::SetLevel(unsigned int l)
{
	m_currentLevel = m_levels.begin() + l;
}
unsigned int UI::GetCurrentLevel() const
{
	// todo: need to implement
	return 0;
}

unsigned int UI::AddElement(const string& c, UIData* pObj)
{
	unsigned int index = -1;

	if(pObj)
	{
		UICreator::TupleType t(pObj,this);

		// create object based on the c string and pass pData to the constructor
		IUIElement* pElement = static_cast<IUIElement*>(m_factory.CreateEntity(c,t));

		index = AddElement(pElement); 
	}

	return index;
}
unsigned int UI::AddElement(IUIElement* pElement)
{
	m_currentLevel->push_back(pElement);
	return m_currentLevel->size() - 1;
}
void UI::RemoveElement(unsigned int i)
{
	// todo: this is not the best structure for this task. switch data structures?
	if(i < m_currentLevel->size())
	{
		delete m_currentLevel->at(i);
		m_currentLevel->erase(m_currentLevel->begin() + i);
	}
}

/*bool UI::IsChecked(unsigned int index) const
{
	bool bChecked = false;

	if(index < m_checkBoxes.size())
	{
		bChecked = (*m_iter)[index].IsChecked();
	}

	return bChecked;
}*/

IUIElement* UI::GetFocus() const
{
	return m_pInFocus;
}
void UI::SetFocus(IUIElement* pElement)
{
	m_pInFocus = pElement;
}
void UI::Update(float dt)
{
	if(m_pInFocus == nullptr)
	{
		for(unsigned int i = 0; i < m_currentLevel->size(); ++i)
		{
			m_currentLevel->at(i)->Update(dt);
		}
	}
	else
	{
		m_pInFocus->Update(dt);
	}
}
void UI::Render() const
{
	for(unsigned int i = 0; i < m_currentLevel->size(); ++i)
	{
		// Check if the UI element can be rendered
		IRender* pUIElement = dynamic_cast<IRender*>(m_currentLevel->at(i));

		if(pUIElement)
		{
			pUIElement->Render();
			//g_pEngine->AddObjectToRenderList(pUIElement);
		}
	}
}

void UI::KeyDownCallback(char Key)
{
	if(m_pInFocus)
	{
		TextBox* pTextBox = dynamic_cast<TextBox*>(m_pInFocus);

		if(pTextBox)
		{
			pTextBox->Update(Key);
		}
	}
}

void UI::RegisterScript()
{
	// todo: this function should only be called once?

	// Register Script with UI 
	asVM& vm = m_pEngine->GetScriptVM();
	asIScriptEngine& engine = vm.GetScriptEngine();

	// Register References
	DBAS(engine.RegisterObjectType("UIData",0,asOBJ_REF));
	DBAS(engine.RegisterObjectType("CheckBoxData",0,asOBJ_REF));
	DBAS(engine.RegisterObjectType("UI",0,asOBJ_REF | asOBJ_NOHANDLE));

	// UIData Behavior
	DBAS(engine.RegisterObjectBehaviour("UIData",asBEHAVE_FACTORY,"UIData@ f()",
		asFUNCTION(UIDataFactory),asCALL_CDECL));
	DBAS(engine.RegisterObjectBehaviour("UIData",asBEHAVE_ADDREF,"void f()",
		asMETHOD(UIData,AddRef),asCALL_THISCALL));
	DBAS(engine.RegisterObjectBehaviour("UIData",asBEHAVE_RELEASE,"void f()",
		asMETHOD(UIData,Release),asCALL_THISCALL));

	DBAS(engine.RegisterObjectProperty("UIData","RECT rect",offsetof(CheckBoxData,rect)));
	DBAS(engine.RegisterObjectProperty("UIData","string name",offsetof(CheckBoxData,str)));


	// Register CheckBoxData

	// Register CheckBoxData Behavior
	DBAS(engine.RegisterObjectBehaviour("CheckBoxData",asBEHAVE_FACTORY,"CheckBoxData@ f()",
		asFUNCTION(CheckBoxDataFactory),asCALL_CDECL));
	DBAS(engine.RegisterObjectBehaviour("CheckBoxData",asBEHAVE_ADDREF,"void f()",
		asMETHOD(CheckBoxData,AddRef),asCALL_THISCALL));
	DBAS(engine.RegisterObjectBehaviour("CheckBoxData",asBEHAVE_RELEASE,"void f()",
		asMETHOD(CheckBoxData,Release),asCALL_THISCALL));

	DBAS(engine.RegisterObjectBehaviour("CheckBoxData",asBEHAVE_IMPLICIT_REF_CAST,"UIData@ f()",
		asFUNCTION((EngineHelper::refCast<CheckBoxData,UIData>)),asCALL_CDECL_OBJLAST));

	DBAS(engine.RegisterObjectProperty("CheckBoxData","bool checked",offsetof(CheckBoxData,checked)));
	DBAS(engine.RegisterObjectProperty("CheckBoxData","RECT rect",offsetof(CheckBoxData,rect)));
	DBAS(engine.RegisterObjectProperty("CheckBoxData","string name",offsetof(CheckBoxData,str)));
	DBAS(engine.RegisterObjectProperty("CheckBoxData","ScriptFunction func",offsetof(CheckBoxData,func)));
	
	
	// Register UI
	DBAS(engine.RegisterObjectMethod("UI","void AddLevel()",asMETHOD(UI, AddLevel),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UI","uint AddElement(const string& in, UIData@)",asMETHODPR(UI,AddElement,(const string&, UIData*),unsigned int), asCALL_THISCALL));	
	DBAS(engine.RegisterGlobalProperty("UI ui",this));

	engine.Release();
}
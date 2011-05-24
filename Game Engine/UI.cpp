// Programmed by Bryce Mehring

#include "StdAfx.h"
#include "UI.h"

using namespace AngelScript;


// static variable definitions
IKMInput* CheckBox::s_pInput = 0;
IRenderingPlugin* CheckBox::s_pRenderer = 0;


// function prototypes
IUIElement* CheckBoxFactory(const CheckBoxData& data);

// CheckBox

CheckBox::CheckBox(const CheckBoxData& data) : m_data(data)
{
}

void CheckBox::Update(float dt)
{
	static bool bNoClick = false;

	if(bNoClick)
	{
		if(s_pInput->MouseClick(0) == false)
		{
			bNoClick = false;
		}
	}

	if(s_pInput->MouseClick(0) && !bNoClick)
	{
		//m_data.m_time = 0;
		POINT mousePos;
		s_pInput->MousePos(mousePos);

		// check if the box was clicked
		// todo: need to fix the bounds
		if((mousePos.x >= m_data.m_Rect.left) && (mousePos.x <= m_data.m_Rect.right))
		{
			if((mousePos.y >= m_data.m_Rect.top) && (mousePos.y <= m_data.m_Rect.bottom))
			{
				m_data.m_checked = !m_data.m_checked;

				// do not come back in here until the user is not clicking
				bNoClick = true;

				// Call callback function
				asVM* pVM = asVM::Instance();
				pVM->ExecuteScriptFunction(m_data.m_ScriptIndex,m_data.m_funcId,m_data.m_checked);
			//	(pEngine->*m_data.m_Callback)(m_data.m_checked);
			}
		}
	}
}

bool CheckBox::IsChecked() const
{
	return m_data.m_checked;
}

void CheckBox::Draw()
{
	DWORD color;

	switch(m_data.m_checked)
	{
	case true:
		color = D3DCOLOR_XRGB(0,255,0);
		break;
	case false:
		color = D3DCOLOR_XRGB(255,255,255);
		break;
	}

	POINT P = {m_data.m_Rect.left,m_data.m_Rect.top};

	s_pRenderer->DrawString(m_data.m_str.c_str(),P,color);

	// need to implement DrawString to draw at a pos on the screen
	//s_pRenderer->DrawString(m_data.m_str.c_str());
}

// UI
UI::UI()
{
	PluginManager* pPlugManager = PluginManager::Instance();
	CheckBox::s_pInput = (IKMInput*)pPlugManager->GetPlugin(Input);
	CheckBox::s_pRenderer = (IRenderingPlugin*)pPlugManager->GetPlugin(Rendering);

	m_levels.resize(1);
	m_currentLevel = m_levels.begin();
}
UI::~UI() {}

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
		m_currentLevel->at(i)->Release();
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
void UI::Update(float dt)
{
	for(unsigned int i = 0; i < m_currentLevel->size(); ++i)
	{
		m_currentLevel->at(i)->Update(dt);
	}
}
void UI::Render() const
{
	for(unsigned int i = 0; i < m_currentLevel->size(); ++i)
	{
		m_currentLevel->at(i)->Draw();
	}
}

void UI::RegisterScript()
{
	// Register Script with UI 
	asVM* pVM = asVM::Instance();
	asIScriptEngine* pEngine = pVM->GetScriptEngine();

	// Register CheckBoxData
	DBAS(pEngine->RegisterObjectType("CheckBoxData",sizeof(CheckBoxData),asOBJ_VALUE));
	DBAS(pEngine->RegisterObjectBehaviour("CheckBoxData",asBEHAVE_CONSTRUCT,"void CheckBoxData()", asFUNCTION(Construct<CheckBoxData>),asCALL_CDECL_OBJLAST));
	DBAS(pEngine->RegisterObjectBehaviour("CheckBoxData",asBEHAVE_DESTRUCT,"void CheckBoxData()", asFUNCTION(Destroy<CheckBoxData>),asCALL_CDECL_OBJLAST));

	DBAS(pEngine->RegisterObjectProperty("CheckBoxData","bool checked",offsetof(CheckBoxData,m_checked)));
	DBAS(pEngine->RegisterObjectProperty("CheckBoxData","RECT rect",offsetof(CheckBoxData,m_Rect)));
	DBAS(pEngine->RegisterObjectProperty("CheckBoxData","string name",offsetof(CheckBoxData,m_str)));
	DBAS(pEngine->RegisterObjectProperty("CheckBoxData","int func",offsetof(CheckBoxData,m_funcId)));
	DBAS(pEngine->RegisterObjectProperty("CheckBoxData","uint scriptIndex",offsetof(CheckBoxData,m_ScriptIndex)));

	// major todo: I really do not need to register IUIElement and CheckBox with as. I can just register C++ functions that add elements to the ui.
	
	// Register  UI
	// todo: if this class is not a singleton, I cannot register a global property, this.
	DBAS(pEngine->RegisterObjectType("IUIElement",0,asOBJ_REF));
	DBAS(pEngine->RegisterObjectType("CheckBox",0,asOBJ_REF));

	// Behaviors

	DBAS(pEngine->RegisterObjectBehaviour("IUIElement", asBEHAVE_REF_CAST, "CheckBox@ f()", asFUNCTION((refCast<IUIElement,CheckBox>)), asCALL_CDECL_OBJLAST));
	DBAS(pEngine->RegisterObjectBehaviour("CheckBox", asBEHAVE_IMPLICIT_REF_CAST, "IUIElement@ f()", asFUNCTION((refCast<CheckBox,IUIElement>)), asCALL_CDECL_OBJLAST));

	DBAS(pEngine->RegisterObjectBehaviour("CheckBox",asBEHAVE_FACTORY,"CheckBox@ f(const CheckBox& in)",asFUNCTION(CheckBoxFactory),asCALL_CDECL));
	DBAS(pEngine->RegisterObjectBehaviour("CheckBox",asBEHAVE_ADDREF,"void f()",asMETHOD(CheckBox,AddRef),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectBehaviour("CheckBox",asBEHAVE_RELEASE,"void f()",asMETHOD(CheckBox,Release),asCALL_THISCALL));

	DBAS(pEngine->RegisterObjectBehaviour("IUIElement",asBEHAVE_ADDREF,"void f()",asMETHOD(IUIElement,AddRef),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectBehaviour("IUIElement",asBEHAVE_RELEASE,"void f()",asMETHOD(IUIElement,Release),asCALL_THISCALL));

	DBAS(pEngine->RegisterObjectMethod("IUIElement", "bool IsChecked()", asMETHOD(IUIElement, IsChecked), asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("IUIElement", "void Update(float)",asMETHOD(IUIElement,Update),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("IUIElement", "void Draw()",asMETHOD(IUIElement,Draw),asCALL_THISCALL));
	
	//DBAS(pEngine->RegisterObjectType("UI",0,asOBJ_REF | asOBJ_NOHANDLE));
	//DBAS(pEngine->RegisterObjectMethod("UI","void AddLevel()",asMETHOD(UI, AddLevel),asCALL_THISCALL));
	//DBAS(pEngine->RegisterObjectMethod("UI","uint AddElement(const CheckBoxData& in)",asMETHOD(UI,AddElement),asCALL_THISCALL));	
	
	pEngine->Release();
}

// function definitions
IUIElement* CheckBoxFactory(const CheckBoxData& data)
{
	return new CheckBox(data);
}
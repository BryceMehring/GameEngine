// Programmed by Bryce Mehring

#include "StdAfx.h"
#include "UI.h"

using namespace AngelScript;


// static variable definitions
IKMInput* CheckBox::s_pInput = 0;
IRenderingPlugin* CheckBox::s_pRenderer = 0;

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
				pVM->ExecuteScriptFunction(m_data.m_ScriptIndex,m_data.m_funcId);
			//	(pEngine->*m_data.m_Callback)(m_data.m_checked);
			}
		}
	}
}

bool CheckBox::IsChecked() const
{
	return m_data.m_checked;
}

void CheckBox::Draw() const
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

// UIManager
UIManager::UIManager()
{
	PluginManager* pPlugManager = PluginManager::Instance();
	CheckBox::s_pInput = (IKMInput*)pPlugManager->GetPlugin(DLLType::Input);
	CheckBox::s_pRenderer = (IRenderingPlugin*)pPlugManager->GetPlugin(DLLType::Rendering);

	m_checkBoxes.resize(1);
	m_iter = m_checkBoxes.begin();
}
UIManager::~UIManager() {}

void UIManager::AddLevel()
{
	m_checkBoxes.push_back(std::vector<CheckBox>());
}
void UIManager::Forward()
{
	m_iter++;
}
void UIManager::Back()
{
	m_iter--;
}
void UIManager::SetLevel(unsigned int l)
{
	m_iter = m_checkBoxes.begin() + l;
}
unsigned int UIManager::GetCurrentLevel() const
{
	return 0;
}


unsigned int UIManager::AddCheckBox(const CheckBoxData& data)
{
	m_iter->push_back(data);
	return m_iter->size() - 1;
}
void UIManager::RemoveCheckBox(unsigned int index)
{
	if(index < m_iter->size())
	{
		m_iter->erase(m_iter->begin() + index);
	}
}

bool UIManager::IsChecked(unsigned int index) const
{
	bool bChecked = false;

	if(index < m_checkBoxes.size())
	{
		bChecked = (*m_iter)[index].IsChecked();
	}

	return bChecked;
}
void UIManager::Update(float dt)
{
	for(unsigned int i = 0; i < m_checkBoxes.size(); ++i)
	{
		(*m_iter)[i].Update(dt);
	}
}
void UIManager::Render() const
{
	for(unsigned int i = 0; i < m_checkBoxes.size(); ++i)
	{
		(*m_iter)[i].Draw();
	}
}

void UIManager::RegisterScript()
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

	
	// Register  UIManager
	DBAS(pEngine->RegisterObjectType("UIManager",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterObjectMethod("UIManager","void AddLevel()",asMETHOD(UIManager, AddLevel),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("UIManager","uint AddCheckBox(const CheckBoxData& in)",asMETHOD(UIManager,AddCheckBox),asCALL_THISCALL));
	DBAS(pEngine->RegisterGlobalProperty("UIManager ui",this));
	
	pEngine->Release();
}
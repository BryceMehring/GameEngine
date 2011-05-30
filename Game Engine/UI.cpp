// Programmed by Bryce Mehring

#include <d3dx9.h>
#include "UI.h"
#include "PluginManager.h"
#include "Factory.h"

using namespace AngelScript;

// global functions
CheckBoxData* CheckBoxDataFactory()
{
	return new CheckBoxData();
}

// static variable definitions
IKMInput* CheckBox::s_pInput = 0;
IRenderingPlugin* CheckBox::s_pRenderer = 0;

// CheckBox 
CheckBox::CheckBox(void* pParam) : m_pData((CheckBoxData*)pParam)
{
	s_pRenderer->GetStringRec(m_pData->m_str.c_str(),m_pData->m_Rect);

	m_pData->m_Rect.left += m_pData->m_pos.x;
	m_pData->m_Rect.right += m_pData->m_pos.x;
	m_pData->m_Rect.bottom += m_pData->m_pos.y;
	m_pData->m_Rect.top += m_pData->m_pos.y;
}
CheckBox::CheckBox(CheckBoxData* pData) : m_pData(pData)
{
	m_pData->AddRef();
}
CheckBox::~CheckBox()
{
	m_pData->Release();
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
		//m_pData->m_time = 0;
		POINT mousePos;
		s_pInput->MousePos(mousePos);

		// check if the box was clicked
		// todo: need to fix the bounds
		if((mousePos.x >= m_pData->m_Rect.left) && (mousePos.x <= m_pData->m_Rect.right))
		{
			if((mousePos.y >= m_pData->m_Rect.top) && (mousePos.y <= m_pData->m_Rect.bottom))
			{
				m_pData->m_checked = !m_pData->m_checked;

				// do not come back in here until the user is not clicking
				bNoClick = true;

				// Call script function
				asVM* pVM = asVM::Instance();
				pVM->ExecuteScriptFunction(m_pData->m_ScriptIndex,m_pData->m_funcId,m_pData->m_checked);
			}
		}
	}
}

bool CheckBox::IsChecked() const
{
	return m_pData->m_checked;
}

void CheckBox::Draw()
{
	DWORD color;

	if(m_pData->m_checked)
	{
		color = D3DCOLOR_XRGB(0,255,0);
	}
	else
	{
		color = D3DCOLOR_XRGB(255,255,255);
	}

	s_pRenderer->DrawString(m_pData->m_str.c_str(),m_pData->m_Rect,color);
	// need to implement DrawString to draw at a pos on the screen
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
UI::~UI()
{
	for(value_type::iterator iter = m_levels.begin(); iter != m_levels.end(); ++iter)
	{
		for(sub_type::iterator subIter = iter->begin(); subIter != iter->end(); ++subIter)
		{
			(*subIter)->Release();
		}
	}
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

unsigned int UI::AddElement(const string& c, void* pObj, int typeId)
{
	unsigned int iReturn = -1;

	if(typeId & asTYPEID_OBJHANDLE)
	{
		// todo: need to figure out what this means
		// because *(void**)pObj != pObj
		void* pData =  *(void**)pObj;

		// AddRef to pData
		asIScriptEngine* pEngine = asVM::Instance()->GetScriptEngine();
		pEngine->AddRefScriptObject(pData, typeId);

		// create object based on the c string and pass pData to the constructor
		Factory<IUIElement>* pUIInstance = Factory<IUIElement>::Instance();
		IUIElement* pElement = pUIInstance->Create(c,pData);

		iReturn = AddElement(pElement);
	}

	return iReturn;
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
	// todo: this function should only be called once?

	// Register Script with UI 
	asVM* pVM = asVM::Instance();
	asIScriptEngine* pEngine = pVM->GetScriptEngine();

	// Register CheckBoxData
	DBAS(pEngine->RegisterObjectType("CheckBoxData",0,asOBJ_REF));

	// Register CheckBoxData Behavior
	DBAS(pEngine->RegisterObjectBehaviour("CheckBoxData",asBEHAVE_FACTORY,"CheckBoxData@ f()",
		asFUNCTION(CheckBoxDataFactory),asCALL_CDECL));
	DBAS(pEngine->RegisterObjectBehaviour("CheckBoxData",asBEHAVE_ADDREF,"void f()",
		asMETHOD(CheckBoxData,AddRef),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectBehaviour("CheckBoxData",asBEHAVE_RELEASE,"void f()",
		asMETHOD(CheckBoxData,Release),asCALL_THISCALL));
	
	DBAS(pEngine->RegisterObjectProperty("CheckBoxData","bool checked",offsetof(CheckBoxData,m_checked)));
	DBAS(pEngine->RegisterObjectProperty("CheckBoxData","POINT pos",offsetof(CheckBoxData,m_pos)));
	DBAS(pEngine->RegisterObjectProperty("CheckBoxData","string name",offsetof(CheckBoxData,m_str)));
	DBAS(pEngine->RegisterObjectProperty("CheckBoxData","int func",offsetof(CheckBoxData,m_funcId)));
	DBAS(pEngine->RegisterObjectProperty("CheckBoxData","uint scriptIndex",offsetof(CheckBoxData,m_ScriptIndex)));
	
	DBAS(pEngine->RegisterObjectType("UI",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterObjectMethod("UI","void AddLevel()",asMETHOD(UI, AddLevel),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("UI","uint AddElement(const string& in, ?&in)",asMETHODPR(UI,AddElement,(const string&, void*, int),unsigned int), asCALL_THISCALL));	
	DBAS(pEngine->RegisterGlobalProperty("UI ui",this));

	pEngine->Release();
}
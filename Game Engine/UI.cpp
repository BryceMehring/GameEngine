// Programmed by Bryce Mehring

#include "StdAfx.h"
#include "UI.h"
#include "PluginManager.h"
#include "Factory.h"

using namespace AngelScript;
using namespace std;

ClickableElement::ClickableElement(RECT* R) : m_pClickRect(R) {}
ClickableElement::~ClickableElement()
{
	m_pClickRect = NULL;
}

void ClickableElement::Update(float dt)
{
	// todo: need to clean up the logic

	static bool bNoClick = false;
	IKMInput& input = g_pEngine->GetInput();

	if(bNoClick)
	{
		if(input.MouseClick(0) == false)
		{
			bNoClick = false;
		}
	}

	if(input.MouseClick(0) && !bNoClick)
	{
		//m_pData->m_time = 0;
		POINT mousePos;
		input.MousePos(mousePos);

		// check if the box was clicked
		if((mousePos.x >= m_pClickRect->left) && (mousePos.x <= m_pClickRect->right))
		{
			if((mousePos.y >= m_pClickRect->top) && (mousePos.y <= m_pClickRect->bottom))
			{
				// do not come back in here until the user is not clicking
				bNoClick = true;

				// Call script function
				ClickResponse();
			}
		}
	}
}


// CheckBox

// global functions
CheckBoxData* CheckBoxDataFactory()
{
	return new CheckBoxData();
}
 
CheckBox::CheckBox(void* pParam) : ClickableElement(&((CheckBoxData*)pParam)->rect), m_pData((CheckBoxData*)pParam)
{
	//todo: I could add this object to another class, so that is class gets rendered 
	// But what about input?

	// to make this work I would need a render abstract interface

	// engine.AddObjectToRenderQueue(this);

	IRenderingPlugin& renderer = g_pEngine->GetRenderer();
	renderer.GetStringRec(m_pData->str.c_str(),m_pData->rect);

	// todo: this is bad coding need to fix this below
	m_pData->rect.left = m_pData->pos.x;
	m_pData->rect.top = m_pData->pos.y;

	m_pData->rect.right += m_pData->pos.x;
	m_pData->rect.bottom += m_pData->pos.y;
	
}
CheckBox::CheckBox(CheckBoxData* pData) : ClickableElement(&pData->rect), m_pData(pData)
{
	
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

	asVM& vm = g_pEngine->GetScriptVM();
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

	IRenderingPlugin& renderer = g_pEngine->GetRenderer();
	renderer.DrawString(m_pData->str.c_str(),m_pData->rect,color,false);
}

// UI
UI::UI()
{
	m_levels.resize(1);
	m_currentLevel = m_levels.begin();
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
		asIScriptEngine& scriptEngine = g_pEngine->GetScriptVM().GetScriptEngine();
		scriptEngine.AddRefScriptObject(pData, typeId);

		scriptEngine.Release();

		// create object based on the c string and pass pData to the constructor
		Factory<IUIElement>& uiInstance = Factory<IUIElement>::Instance();
		IUIElement* pElement = uiInstance.Create(c,pData);

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
		IRender* pUIElement = dynamic_cast<IRender*>(m_currentLevel->at(i));

		if(pUIElement)
		{
			pUIElement->Render();
			//g_pEngine->AddObjectToRenderList(pUIElement);
		}
	}
}

void UI::RegisterScript()
{
	// todo: this function should only be called once?

	// Register Script with UI 
	asVM& vm = g_pEngine->GetScriptVM();
	asIScriptEngine& engine = vm.GetScriptEngine();

	// Register CheckBoxData
	DBAS(engine.RegisterObjectType("CheckBoxData",sizeof(CheckBoxData),asOBJ_REF));

	// Register CheckBoxData Behavior
	DBAS(engine.RegisterObjectBehaviour("CheckBoxData",asBEHAVE_FACTORY,"CheckBoxData@ f()",
		asFUNCTION(CheckBoxDataFactory),asCALL_CDECL));
	DBAS(engine.RegisterObjectBehaviour("CheckBoxData",asBEHAVE_ADDREF,"void f()",
		asMETHOD(CheckBoxData,AddRef),asCALL_THISCALL));
	DBAS(engine.RegisterObjectBehaviour("CheckBoxData",asBEHAVE_RELEASE,"void f()",
		asMETHOD(CheckBoxData,Release),asCALL_THISCALL));
	
	DBAS(engine.RegisterObjectProperty("CheckBoxData","bool checked",offsetof(CheckBoxData,checked)));
	DBAS(engine.RegisterObjectProperty("CheckBoxData","POINT pos",offsetof(CheckBoxData,pos)));
	DBAS(engine.RegisterObjectProperty("CheckBoxData","string name",offsetof(CheckBoxData,str)));
	DBAS(engine.RegisterObjectProperty("CheckBoxData","ScriptFunction func",offsetof(CheckBoxData,func)));

	DBAS(engine.RegisterObjectType("UI",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(engine.RegisterObjectMethod("UI","void AddLevel()",asMETHOD(UI, AddLevel),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("UI","uint AddElement(const string& in, ?&in)",asMETHODPR(UI,AddElement,(const string&, void*, int),unsigned int), asCALL_THISCALL));	
	DBAS(engine.RegisterGlobalProperty("UI ui",this));

	engine.Release();
}
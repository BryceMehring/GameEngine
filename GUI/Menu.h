#ifndef _MENU_
#define _MENU_

#include "RefCounting.h"
#include "ButtonBase.h"
#include "IRenderer.h"
#include "IInput.h"

namespace UI
{

// Menu manages updating and drawing IElements on the current menu
class Menu : public RefCounting
{
public:

	Menu();

	virtual void Update(class GUI* pGUI, IInput& input, double dt);
	virtual void Render(IRenderer& renderer);

	// Adds a sub menu connected to this menu
	void AddMenu(Menu* pMenu, Button* pElement, class GUI* pGUI);

	// Adds a gui element to this menu
	void AddElement(IElement* pElement);

private:

	~Menu();

	std::vector<IElement*> m_elements; // A list of all gui elements in the menu
	std::vector<Menu*> m_menus; // a list of all menus connected to this menu
	Menu * m_pPrev; // The previous menu in the menu tree

};

} // UI namespace


#endif // _MENU_

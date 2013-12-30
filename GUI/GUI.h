#ifndef _GUI_
#define _GUI_

#include "Delegates.h"
#include "Menu.h"
#include "IInput.h"
#include "IRenderer.h"

namespace UI
{

// GUI class that the game will interface with.
// The gui is a small wrapper around the Menu
class GUI
{
public:

	typedef Delegate<void,Menu*> ChangeMenuCallback;

	explicit GUI(Menu* pMenu = nullptr);
	~GUI();

	// Create a callback to change the menu
	ChangeMenuCallback CreateCallback();

	// Get/set the current menu in use
	void SetMenu(Menu* pMenu);
	Menu* GetMenu();

	// Indexes index the current UI menu's elements via the tab key.
	void SetIndex(unsigned int i);
	unsigned int GetIndex() const;

	void Update(::IInput& input, double dt);
	void Render(::IRenderer& renderer);

private:

	Menu* m_pMenu;
	unsigned int m_uiCurrentIndex;

};

}

#endif // _GUI_
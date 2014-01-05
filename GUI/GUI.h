#ifndef _GUI_
#define _GUI_

#include "Menu.h"
#include "IInput.h"
#include "IRenderer.h"

#include <functional>

namespace UI
{

// GUI class that the game will interface with.
// The gui is a small wrapper around the Menu
class GUI
{
public:

	explicit GUI(Menu* pMenu = nullptr);
	~GUI();

	// Get/set the current menu in use
	void SetMenu(Menu* pMenu);
	Menu* GetMenu();

	// Indexes index the current UI menu's elements via the tab key.
	void SetIndex(unsigned int i);
	unsigned int GetIndex() const;

	void Update(IInput& input, double dt);
	void Render(IRenderer& renderer);

private:

	Menu* m_pMenu;
	unsigned int m_uiCurrentIndex;

};

}

#endif // _GUI_
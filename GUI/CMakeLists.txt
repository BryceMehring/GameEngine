

set(GUI_HEADERS 
	ButtonBase.h
	GenericButton.h
	GUI.h
	IElement.h
	Menu.h
	ProgressBar.h
	Slider.h
	TextBox.h
	)

set(GUI_SOURCE
	ButtonBase.cpp
	GUI.cpp
	Menu.cpp
	ProgressBar.cpp
	Slider.cpp
	TextBox.cpp
	)

add_library(GUI STATIC ${GUI_HEADERS} ${GUI_SOURCE})
  
  
  
  
  
  

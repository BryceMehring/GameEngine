#ifndef _IINPUT_
#define _IINPUT_

#include "IPlugin.h"
#include <glm/vec2.hpp>
#include <string>

enum class JoystickAxes
{
	LS,
	RS,
	LT,
	RT,
};

/* Printable keys */
#define KEY_SPACE              32
#define KEY_APOSTROPHE         39  /* ' */
#define KEY_COMMA              44  /* , */
#define KEY_MINUS              45  /* - */
#define KEY_PERIOD             46  /* . */
#define KEY_SLASH              47  /* / */
#define KEY_0                  48
#define KEY_1                  49
#define KEY_2                  50
#define KEY_3                  51
#define KEY_4                  52
#define KEY_5                  53
#define KEY_6                  54
#define KEY_7                  55
#define KEY_8                  56
#define KEY_9                  57
#define KEY_SEMICOLON          59  /* ; */
#define KEY_EQUAL              61  /* = */
#define KEY_A                  65
#define KEY_B                  66
#define KEY_C                  67
#define KEY_D                  68
#define KEY_E                  69
#define KEY_F                  70
#define KEY_G                  71
#define KEY_H                  72
#define KEY_I                  73
#define KEY_J                  74
#define KEY_K                  75
#define KEY_L                  76
#define KEY_M                  77
#define KEY_N                  78
#define KEY_O                  79
#define KEY_P                  80
#define KEY_Q                  81
#define KEY_R                  82
#define KEY_S                  83
#define KEY_T                  84
#define KEY_U                  85
#define KEY_V                  86
#define KEY_W                  87
#define KEY_X                  88
#define KEY_Y                  89
#define KEY_Z                  90
#define KEY_LEFT_BRACKET       91  /* [ */
#define KEY_BACKSLASH          92  /* \ */
#define KEY_RIGHT_BRACKET      93  /* ] */
#define KEY_GRAVE_ACCENT       96  /* ` */
#define KEY_WORLD_1            161 /* non-US #1 */
#define KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define KEY_ESCAPE             256
#define KEY_ENTER              257
#define KEY_TAB                258
#define KEY_BACKSPACE          259
#define KEY_INSERT             260
#define KEY_DELETE             261
#define KEY_RIGHT              262
#define KEY_LEFT               263
#define KEY_DOWN               264
#define KEY_UP                 265
#define KEY_PAGE_UP            266
#define KEY_PAGE_DOWN          267
#define KEY_HOME               268
#define KEY_END                269
#define KEY_CAPS_LOCK          280
#define KEY_SCROLL_LOCK        281
#define KEY_NUM_LOCK           282
#define KEY_PRINT_SCREEN       283
#define KEY_PAUSE              284
#define KEY_F1                 290
#define KEY_F2                 291
#define KEY_F3                 292
#define KEY_F4                 293
#define KEY_F5                 294
#define KEY_F6                 295
#define KEY_F7                 296
#define KEY_F8                 297
#define KEY_F9                 298
#define KEY_F10                299
#define KEY_F11                300
#define KEY_F12                301
#define KEY_F13                302
#define KEY_F14                303
#define KEY_F15                304
#define KEY_F16                305
#define KEY_F17                306
#define KEY_F18                307
#define KEY_F19                308
#define KEY_F20                309
#define KEY_F21                310
#define KEY_F22                311
#define KEY_F23                312
#define KEY_F24                313
#define KEY_F25                314
#define KEY_KP_0               320
#define KEY_KP_1               321
#define KEY_KP_2               322
#define KEY_KP_3               323
#define KEY_KP_4               324
#define KEY_KP_5               325
#define KEY_KP_6               326
#define KEY_KP_7               327
#define KEY_KP_8               328
#define KEY_KP_9               329
#define KEY_KP_DECIMAL         330
#define KEY_KP_DIVIDE          331
#define KEY_KP_MULTIPLY        332
#define KEY_KP_SUBTRACT        333
#define KEY_KP_ADD             334
#define KEY_KP_ENTER           335
#define KEY_KP_EQUAL           336
#define KEY_LEFT_SHIFT         340
#define KEY_LEFT_CONTROL       341
#define KEY_LEFT_ALT           342
#define KEY_LEFT_SUPER         343
#define KEY_RIGHT_SHIFT        344
#define KEY_RIGHT_CONTROL      345
#define KEY_RIGHT_ALT          346
#define KEY_RIGHT_SUPER        347
#define KEY_MENU               348
#define KEY_LAST               KEY_MENU

// Input Plugin
class IInput : public IPlugin
{
public:

	//note about the once parameter:
	//true: only process the event once,
	//false: returns true the entire period of the event

	// Processes input events
	virtual void Poll() = 0;

	// ----- Keyboard -----

	/*
	Loads a Keybind file
	format:
	bind NEW_KEY OLD_KEY
	*/
	virtual bool LoadKeyBindFile(const std::string& file) = 0;

	// Returns true if Key is pressed, false otherwise
	virtual bool KeyPress(int key, bool once = true) const = 0;

	// Returns true if Key is released, false otherwise
	virtual bool KeyRelease(int key, bool once = true) const = 0;

	// Returns true if there is a character pressed, which is outputted through the parameter out
	// note: this method should only be used for text input
	virtual bool CharKeyDown(char& out) const = 0;

	virtual void RemapKey(int key, int newKey) = 0;

	// ----- Cursor -----

	// Returns true if the mouse button(0 - 7) is clicked, false otherwise
	virtual bool MouseClick(int button, bool once = true) const = 0;

	// Returns true if the mouse button(0 - 7) is released, false otherwise
	virtual bool MouseRelease(int button, bool once = true) const = 0;

	// Returns the cursor position in screen space
	// Origin: Bottom left hand corner
	// range: [(0, 0), (width, height)]
	// Note: if the cursor is disabled via ShowCursor(), the cursors position will not be updated
	virtual const glm::ivec2& GetCursorPos() const = 0;

	// Moves the cursor to pos
	virtual void SetCursorPos(glm::ivec2 pos) = 0;

	// Returns true if the cursor is shown, false otherwise
	virtual bool IsCursorShown() const = 0;

	// Returns true if the cursor is within the main window
	virtual bool IsCursorEntered() const = 0;

	// Shows the cursor if bShow is true
	// Disables the cursor if bShow is false
	virtual void ShowCursor(bool bShow) = 0;

	// Returns Cursor Acceleration
	virtual glm::ivec2 CursorAcceleration() const = 0;

	// Scroll change
	virtual double MouseZ() const = 0;

	// Returns true if user clicks, out is the current selection box
	virtual bool GetSelectedRect(glm::ivec2& min, glm::ivec2& max) = 0;

	// ----- Joysticks -----

	// Returns true if a joystick is connected, false otherwise
	virtual bool IsValidJoystickConnected() const = 0;

	// Returns the name of the connected joystick
	// Note: If a joystick is not connected, an empty string is returned
	virtual std::string GetJoystickName() const = 0;

	// Sets the dead zone for the specified joystick axes
	// Dead zones are an area around the center of the joystick in which the axes will be zeroed
	virtual void SetJoystickAxesDeadZone(JoystickAxes i, float deadZone) = 0;

	// Returns the axes and direction of the joystick axes being pushed via parameters
	// The function returns true when any axes is pressed, else false is returned
	// todo: replace the ints with something more meaningful
	virtual bool GetMovingJoystickAxes(int& axes, int& dir) const = 0;

	// Returns the current value of the joystick axis
	// Note: If a joystick is not connected, a zeroed vec2 is returned
	virtual glm::vec2 GetJoystickAxes(JoystickAxes i) const = 0;

	// Returns the number of buttons on the joystick
	virtual int GetNumJoystickButtons() const = 0;

	// Returns true if the button is pressed, else false
	virtual bool JoystickButtonPress(int button, bool once = true) const = 0;

	// Returns true if the button is released, else false
	virtual bool JoystickButtonRelease(int button, bool once = true) const = 0;
	
protected:

	virtual ~IInput() {}

};

#endif // _IINPUT_

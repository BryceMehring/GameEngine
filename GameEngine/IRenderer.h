#ifndef _IRENDERER_
#define _IRENDERER_

#include "IPlugin.h"
#include "IResourceManager.h"
#include "VecMath.h"
#include <string>
#include "glm/glm.hpp"

class IRenderer : public IPlugin
{
public:

	// clears the screen
	virtual void ClearScreen() = 0;

	// draw everything to screen
	virtual void Present() = 0;

	//-------- TODO: REMOVE THESE, NOT USED ANYMORE -------
	virtual void Reset() = 0;
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
	virtual bool IsDeviceLost() = 0;
	//-------- TODO: REMOVE THESE, NOT USED ANYMORE -------

	// whenever an ISpatialObject is passed to the renderer,
	// the renderer deals with rendering the object so the application does not have to worry about it.
	
	// Returns the resource manager
	virtual IResourceManager& GetResourceManager() = 0;

	// Draws lines
	virtual void DrawLine(const glm::vec3* pArray, // array of 3d vertices to draw
						  unsigned int length, // number of vertices
						  const glm::vec4& color = glm::vec4(1.0f), // color of the line
						  const glm::mat4& t = glm::mat4(1.0f)) = 0; // transformation to apply to the line

	// Fonts
	virtual void GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const = 0;

	virtual void DrawString(const char* str, // the string that gets drawn
							const glm::vec2& pos, // World pos of the text, where the text starts getting drawn from
							const glm::vec2& scale = glm::vec2(1.0f), // size of the text
							const glm::vec3& color = glm::vec3(1.0f), // color of the text blended together with the texture
							const char* font = nullptr // the desired font, may be null if you wish to use the default font
			) = 0; // world space

	virtual void DrawSprite(const std::string& texture, // texture used to draw the sprite
							const glm::mat4& transformation, // transformation applied to the sprite
							const glm::vec2& tiling = glm::vec2(1.0f), // the amount of tiling, 1.0 means the texture will be stretched across the whole polygon
							unsigned int iCellId = 0 // cellId if multiple frames are stored together in the same sprite image
			) = 0;

	virtual void SetShaderValue(const std::string& shader, const std::string& location, float value ) = 0;
	virtual void SetShaderValue(const std::string& shader, const std::string& location, const glm::vec2& value ) = 0;

	// ignore for now, working on this
	virtual void SetCamera(class Camera*) = 0;


	// config
	virtual void EnableVSync(bool) = 0; // todo: need to implement

	// Queries the computer to get a list of valid display modes supported by their vid card
	// 0 = Highest Res
	// N = Lowest Res where N is the number of display modes
	virtual void EnumerateDisplayAdaptors() = 0;
	virtual unsigned int GetNumDisplayModes() const = 0; // returns the number of video modes from EnumerateDisplayAdaptors()
	virtual unsigned int GetCurrentDisplayMode() const = 0; // returns the current display mode
	virtual void SetDisplayMode(unsigned int i) = 0; // set a display mode, i being the index into the displayModeList.
	virtual const std::string& GetDisplayModeStr(unsigned int i) const = 0; // get the display mode str
	virtual void ToggleFullscreen() = 0; // todo: need to implement
	
	///add more functions...
protected:

	virtual ~IRenderer() {}

};


#endif //_IRENDERER_

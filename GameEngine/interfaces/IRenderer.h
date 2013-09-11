#ifndef _IRENDERER_
#define _IRENDERER_

#include "IPlugin.h"
#include "IResourceManager.h"
#include "VecMath.h"
#include <string>
#include <glm/glm.hpp>

enum class FontAlignment
{
	Left,
	Center,
	Right
};

// Renderer plugin interface
class IRenderer : public IPlugin
{
public:

	// clears the screen
	virtual void ClearScreen() = 0;

	// draw everything to screen
	virtual void Present() = 0;

	// Returns the resource manager
	virtual IResourceManager& GetResourceManager() = 0;

	// Lines

	virtual void GetLineWidthRange(glm::vec2& out) const = 0; // Gets the range of the width of the lines supported on the current hardware, x = min, y = max

	virtual void DrawLine(const glm::vec3* pArray, // array of 3d vertices to draw
						  unsigned int length, // number of vertices
						  float fWidth = 3.0f, // the width of the line
						  const glm::vec4& color = glm::vec4(1.0f), // color of the line
						  const glm::mat4& t = glm::mat4(1.0f)) = 0; // transformation to apply to the line

	// Fonts
	virtual void GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const = 0;
	virtual void DrawString(const char* str, // the string that gets drawn
							const glm::vec3& pos, // pos of the text in world space
							const glm::vec2& scale = glm::vec2(10.0f), // scaling the text
							const glm::vec3& color = glm::vec3(1.0f), // color of the text blended together with the texture
							const char* font = nullptr, // the desired font, may be null if you wish to use the default font
							FontAlignment options = FontAlignment::Left
							) = 0;

	virtual void DrawSprite(const std::string& texture, // texture used to draw the sprite
							const glm::mat4& transformation, // transformation applied to the sprite
							const glm::vec2& tiling = glm::vec2(1.0f), // the amount of tiling, 1.0 means the texture will be stretched across the whole polygon
							unsigned int iCellId = 0, // cellId if multiple frames are stored together in the same sprite image
							const std::string& tech = "sprite"
							) = 0;

	virtual bool SetShaderValue(const std::string& shader, const std::string& location, float value ) = 0;
	virtual bool SetShaderValue(const std::string& shader, const std::string& location, const glm::vec2& value ) = 0;

	// ignore for now, working on this
	virtual void SetCamera(class Camera*) = 0;


	// config
	virtual void EnableVSync(bool) = 0;
	virtual int  GetNumMonitors() const = 0;
	virtual int  GetNumDisplayModes(int monitor) const = 0; // returns the number of video modes for the given monitor
	virtual void GetCurrentDisplayMode(int& monitor, int& mode) const = 0; // returns the current display mode given the monitor
	virtual void SetDisplayMode(int mode) = 0; // sets a display mode
	virtual bool GetDisplayMode(int monitor, int mode, int& width, int& height) const = 0; // get the display mode, return true if success, false if error

protected:

	virtual ~IRenderer() {}

};


#endif //_IRENDERER_

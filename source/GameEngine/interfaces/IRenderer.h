#ifndef _IRENDERER_
#define _IRENDERER_

#include "IPlugin.h"
#include "IResourceManager.h"
#include "VecMath.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

enum class FontAlignment
{
	Left,
	Center,
	Right
};

enum RenderSpace
{
	World,
	Screen
};

// Renderer plugin interface
class IRenderer : public IPlugin
{
public:

	// DrawLine() caches a line to be drawn by Present()
	// Note: if pArray is NULL, then DrawLine() terminates
	virtual void DrawLine(const glm::vec3* pArray, // array of 3d vertices to draw
						  unsigned int length, // number of vertices
						  float fWidth = 3.0f, // the width of the line
						  const glm::vec4& color = glm::vec4(1.0f), // color of the line
						  const glm::mat4& t = glm::mat4(1.0f)) = 0; // transformation to apply to the line

	// DrawCircle() caches a circle to be drawn be Present()
	virtual void DrawCircle(const glm::vec3& center, // center of the circle
							float radius, // radius of the circle
							float thickness, // thickness of the circle
							unsigned int segments, // the number of segments to draw the circle in
							const glm::vec4& color // the color of the circle
							) = 0;

	// DrawString() caches a string to be drawn by Present()
	// Note: if either str or font is NULL, then DrawString() terminates
	virtual void DrawString(const char* str, // the string that gets drawn
							const glm::vec3& pos, // pos of the text in the current render space
							const glm::vec4& color = glm::vec4(1.0f), // color of the text blended together with the texture
							float scale = 50.0f, // height of the text, the width of the text gets scaled accordingly
							const char* font = nullptr, // the desired font, may be null if you wish to use the default font
							FontAlignment alignment = FontAlignment::Left
							) = 0;

	// DrawSprite() caches a single sprite to be drawn by Present()
	virtual void DrawSprite(const std::string& texture, // texture used to draw the sprite
							const glm::mat4& transformation, // transformation applied to the sprite
							const glm::vec4& color = glm::vec4(1.0f), // color that gets blended together with the sprite
							const glm::vec2& tiling = glm::vec2(1.0f), // the amount of tiling, 1.0 means the texture will be stretched across the whole polygon
							unsigned int iCellId = 0, // cellId if multiple frames are stored together in the same sprite image
							const std::string& tech = "sprite"
							) = 0;

	// Draws a single sprite with a white texture
	virtual void DrawSprite(const glm::mat4& transformation, // transformation applied to the sprite
							const glm::vec4& color = glm::vec4(1.0f), // color that gets blended together with the sprite
							const glm::vec2& tiling = glm::vec2(1.0f), // the amount of tiling, 1.0 means the texture will be stretched across the whole polygon
							unsigned int iCellId = 0, // cellId if multiple frames are stored together in the same sprite image
							const std::string& tech = "sprite"
							) = 0;

	// Manage cursor creation
	virtual int CreateCursor(const std::string& texture, int xhot, int yhot) = 0;
	virtual void DestroyCursor(int cursor) = 0;
	virtual void SetCursor(int cursor) = 0;

	// Returns the resource manager
	virtual IResourceManager& GetResourceManager() = 0;

	// todo: add the ability to specify which component is being read
	// Returns depth value in the buffer at a single point in screen space
	virtual float ReadPixels(const glm::ivec2& pos) const = 0;

	// Get the display mode on the specified monitor, return true on success, false on error
	// either width or height may be null if their values are not needed
	virtual bool GetDisplayMode(int monitor, int mode, int* width, int* height) const = 0; 

	// Get the current display mode, return true on success, false on error
	// either width, height, or vsync may be null if their values are not needed
	virtual bool GetDisplayMode(int* width, int* height, bool* vsync = nullptr) const = 0; 

	// Returns the number of monitors active on the current system
	virtual int GetNumMonitors() const = 0;

	// Returns the number of video modes for the given monitor
	virtual int GetNumDisplayModes(int monitor) const = 0; 

	// Returns a tight bounding box that fits around the text,
	// if out is expected to be in user space, set out's topLeft position vector before calling this method, 
	// else, the bottomRight vector of out will be the width and height of the string and the topLeft vector will be zeroed
	// Note: if str is NULL, then GetStringRec() terminates without modifying out
	virtual void GetStringRect(const char* str, float scale, FontAlignment alignment, Math::FRECT& out) const = 0;

	// Sets the world space camera to use
	virtual void SetCamera(class PerspectiveCamera*) = 0;

	// Sets the color of the screen after it gets cleared
	virtual void SetClearColor(const glm::vec3& color) = 0;
	virtual void EnableColorClearing(bool bEnable) = 0;

	// Sets the display mode
	virtual void SetDisplayMode(int mode) = 0;

	// Sets the coordinate system to render all objects in(screen space or world space)
	virtual void SetRenderSpace(RenderSpace) = 0; 

	// todo: add comments here
	virtual void SetShaderValue(const std::string& shader, const std::string& location, float value ) = 0;
	virtual void SetShaderValue(const std::string& shader, const std::string& location, const glm::vec2& value ) = 0;

	// True enables VSync
	// False disables VSync
	virtual void EnableVSync(bool) = 0;

	// Returns true if the window is iconified
	virtual bool IsIconified() const = 0;

	// Render everything that has been cached so far to the back buffer and then swap the back buffer with the front buffer
	virtual void Present() = 0;

protected:

	virtual ~IRenderer() {}

};


#endif //_IRENDERER_

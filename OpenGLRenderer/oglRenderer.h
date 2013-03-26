#ifndef _OGLRENDERER_
#define _OGLRENDERER_

#define PLUGIN_EXPORTS
#define GLFW_DLL

#include "IRenderer.h"
#include "FontEngine.h"
#include "LineEngine.h"
#include "ResourceManager.h"
#include "PluginManager.h"
#include "Camera.h"
#include <angelscript.h>

#define GLFW_NO_GLU
#include <GL\glew.h>
#include <GL\glfw.h>

class oglRenderer : public IRenderer
{
public:

	oglRenderer(asIScriptEngine*);
	virtual ~oglRenderer();

	// IPlugin
	virtual DLLType GetPluginType() const;
	virtual const char* GetName() const;
	virtual int GetVersion() const;

	// IRenderer

	virtual void ClearScreen();
	virtual void Present();

	// todo: need to implement
	virtual void Reset() {}
	virtual void OnLostDevice() {}
	virtual void OnResetDevice() {}
	virtual bool IsDeviceLost() { return false; }

	// textures
	virtual IResourceManager& GetResourceManager();

	// Line
	virtual void DrawLine(const glm::vec3* pArray, unsigned int length, const glm::vec4& color, const glm::mat4& t);

	// Fonts
	virtual void GetStringRec(const char* str, const glm::vec2& pos, const glm::vec2& scale, Math::FRECT& out);

	virtual void DrawString(const char* str, // the string that gets drawn
							const glm::vec2& pos, // World pos of the text, where the text starts getting drawn from
							const char* font = nullptr, // the desired font, may be null if you wish to use the default font
							const glm::vec4& color = glm::vec4(1.0f,1.0f,1.0f,1.0f), // color of the text blended together with the texture
							const glm::vec2& scale = glm::vec2(1.0f,1.0f) // size of the text
							); // world space

	virtual void DrawSprite(const glm::mat4& transformation, // transformation applied to the sprite
							const std::string& texture, // texture used to draw the sprite
							unsigned int iCellId = 0, // cellId if multiple frames are stored together in the same sprite image 
							float dx = 1.0f, // the amount of tiling in the x direction, 1.0 means the texture will be stretched across the whole polygon 
							float dy = 1.0f // " y
							);

	virtual void SetCamera(class Camera*);

	// config
	virtual void EnableVSync(bool);
	virtual void EnumerateDisplayAdaptors();
	virtual unsigned int GetNumDisplayModes() const;
	virtual unsigned int GetCurrentDisplayMode() const;
	virtual void SetDisplayMode(unsigned int i);
	virtual const std::string& GetDisplayModeStr(unsigned int i) const;
	virtual void ToggleFullscreen();

private:

	asIScriptEngine* m_as;

	Camera* m_pCamera;

	ResourceManager m_rm;
	FontEngine* m_pFonts;
	LineEngine* m_pLines;

	std::vector<GLFWvidmode> m_VideoModes;
	std::vector<std::string> m_VideoModeStr;

	unsigned int m_uiCurrentDisplayMode;
	bool m_bFullscreen;

	void GLFWOpenWindowHints();
};

#endif // _OGLRENDERER_
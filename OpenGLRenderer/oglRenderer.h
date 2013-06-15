#ifndef _OGLRENDERER_
#define _OGLRENDERER_

#define PLUGIN_EXPORTS
#define GLFW_DLL

#include "IRenderer.h"
#include "FontEngine.h"
#include "LineEngine.h"
#include "SpriteEngine.h"
#include "ResourceManager.h"
#include "PluginManager.h"
#include "Camera.h"
#include <angelscript.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class oglRenderer : public IRenderer
{
public:

	oglRenderer();
	virtual ~oglRenderer();

	// IPlugin
	virtual DLLType GetPluginType() const;
	virtual const char* GetName() const;
	virtual int GetVersion() const;

	virtual void Init(class asIScriptEngine*);
	virtual void Destroy(class asIScriptEngine*);

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

	// Lines
	virtual void GetLineWidthRange(glm::vec2& out) const;
	virtual void DrawLine(const glm::vec3* pArray, unsigned int length, float width, const glm::vec4& color, const glm::mat4& t);
	

	// Fonts
	virtual void GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const;

	virtual void DrawString(const char* str, // the string that gets drawn
							const glm::vec2& pos, // World pos of the text, where the text starts getting drawn from
							const glm::vec2& scale, // size of the text
							const glm::vec3& color, // color of the text blended together with the texture
							const char* font, // the desired font, may be null if you wish to use the default font
							FontAlignment options
							);

	virtual void DrawSprite(const std::string& texture, // texture used to draw the sprite
							const glm::mat4& transformation, // transformation applied to the sprite
							const glm::vec2& tiling = glm::vec2(1.0f), // the amount of tiling, 1.0 means the texture will be stretched across the whole polygon
							unsigned int iCellId = 0 // cellId if multiple frames are stored together in the same sprite image
							);

	virtual void SetShaderValue(const std::string& shader, const std::string& location, float value );
	virtual void SetShaderValue(const std::string& shader, const std::string& location, const glm::vec2& value );


	virtual void SetCamera(class Camera*);

	// config
	virtual void EnableVSync(bool);
	virtual void EnumerateDisplayAdaptors();
	virtual int GetNumDisplayModes() const;
	virtual int GetCurrentDisplayMode() const;
	virtual void SetDisplayMode(unsigned int i);
	virtual const std::string& GetDisplayModeStr(unsigned int i) const;
	virtual void ToggleFullscreen();

private:

	Camera* m_pCamera;

	GLFWwindow* m_pWindow;

	ResourceManager m_rm;
	FontEngine* m_pFonts;
	LineEngine* m_pLines;
	SpriteEngine* m_pSprites;

	// todo: turn this into a vector?
	const GLFWvidmode* m_pVideoModes;
	int m_iNumVideoModes;

	std::vector<std::string> m_VideoModeStr;

	int m_uiCurrentDisplayMode;
	bool m_bFullscreen;

	void GLFWOpenWindowHints();
};

#endif // _OGLRENDERER_

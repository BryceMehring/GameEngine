#ifndef _OGLRENDERER_
#define _OGLRENDERER_

#ifdef _WIN32
#include <Windows.h>
#endif

#include "IRenderer.h"
#include "PluginManager.h"
#include "FontEngine.h"
#include "LineEngine.h"
#include "SpriteEngine.h"
#include "ResourceManager.h"

#include "Camera.h"
#include <angelscript.h>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class oglRenderer : public IRenderer
{
public:

	static void MonitorCallback(GLFWmonitor*,int);

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

	// textures
	virtual IResourceManager& GetResourceManager();

	// Lines
	virtual void GetLineWidthRange(glm::vec2& out) const;
	virtual void DrawLine(const glm::vec3* pArray, unsigned int length, float width, const glm::vec4& color, const glm::mat4& t);


	// Fonts
	virtual void GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const;

	//virtual void DrawString(const char* str, const glm::vec3& pos, const DrawTextInfo&);
	virtual void DrawString(const char* str, // the string that gets drawn
							const glm::vec3& pos, // World pos of the text, where the text starts getting drawn from
							const glm::vec2& scale, // size of the text
							const glm::vec3& color, // color of the text blended together with the texture
							const char* font, // the desired font, may be null if you wish to use the default font
							FontAlignment options
							);

	virtual void DrawSprite(const std::string& texture, // texture used to draw the sprite
							const glm::mat4& transformation, // transformation applied to the sprite
							const glm::vec2& tiling, // the amount of tiling, 1.0 means the texture will be stretched across the whole polygon
							unsigned int iCellId, // cellId if multiple frames are stored together in the same sprite image
							const std::string& tech
							);

	virtual bool SetShaderValue(const std::string& shader, const std::string& location, float value );
	virtual bool SetShaderValue(const std::string& shader, const std::string& location, const glm::vec2& value );


	virtual void SetCamera(class Camera*);

	// config
	virtual void EnableVSync(bool);
	virtual int  GetNumMonitors() const;
	virtual int  GetNumDisplayModes(int monitor) const;
	virtual void GetCurrentDisplayMode(int& monitor, int& mode) const;
	virtual void SetDisplayMode(int mode);
	virtual bool GetDisplayMode(int monitor, int mode, int& width, int& height) const;

private:

	static oglRenderer* s_pThis;

	Camera* m_pCamera;

	GLFWwindow* m_pWindow;

	ResourceManager m_rm;
	std::auto_ptr<FontEngine> m_pFonts;
	std::auto_ptr<LineEngine> m_pLines;
	std::auto_ptr<SpriteEngine> m_pSprites;

	GLFWmonitor** m_pMonitors;
	int m_iMonitorCount;

	std::vector<std::pair<const GLFWvidmode*,int>> m_videoModes;

	int m_iCurrentMonitor;
	int m_iCurrentDisplayMode;

	bool m_bFullscreen;

	// Helper functions
	void ConfigureGLFW();
	void ConfigureOpenGL();
	void EnumerateDisplayAdaptors();
	void GLFWOpenWindowHints();
	bool CheckShader(const std::string& shader, const std::string& location, GLuint& shaderID, GLuint& outLocation) const;
	void ParseVideoSettingsFile();
	void SaveDisplayList();
};

#endif // _OGLRENDERER_

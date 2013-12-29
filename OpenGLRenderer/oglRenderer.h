#ifndef _OGLRENDERER_
#define _OGLRENDERER_

#include "IRenderer.h"
#include "PluginManager.h"
#include "FontEngine.h"
#include "LineEngine.h"
#include "SpriteEngine.h"
#include "ResourceManager.h"
#include "VertexBuffer.h"

#include "Camera.h"
#include <angelscript.h>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class oglRenderer : public IRenderer
{
public:

	oglRenderer();
	~oglRenderer();

	// IPlugin
	DLLType GetPluginType() const override;
	const char* GetName() const override;
	int GetVersion() const override;

	void Init(class asIScriptEngine*) override;
	void Destroy(class asIScriptEngine*) override;

	// IRenderer

	// DrawLine() caches a line to be drawn by Present()
	// Note: if pArray is NULL, then DrawLine() terminates
	virtual void DrawLine(const glm::vec3* pArray, // array of 3d vertices to draw
						  unsigned int length, // number of vertices
						  float fWidth = 3.0f, // the width of the line
						  const glm::vec4& color = glm::vec4(1.0f), // color of the line
						  const glm::mat4& t = glm::mat4(1.0f)) override; // transformation to apply to the line

	// DrawString() caches a string to be drawn by Present()
	// Note: if either str or font is NULL, then DrawString() terminates
	virtual void DrawString(const char* str, // the string that gets drawn
							const glm::vec3& pos, // pos of the text in the current render space
							float scale = 1.0f, // scaling the text, 1.0 = 1-1 mapping with the font
							const glm::vec4& color = glm::vec4(1.0f), // color of the text blended together with the texture
							const char* font = nullptr, // the desired font, may be null if you wish to use the default font
							FontAlignment alignment = FontAlignment::Left
							) override;

	// DrawSprite() caches a single sprite to be drawn by Present()
	virtual void DrawSprite(const std::string& texture, // texture used to draw the sprite
							const glm::mat4& transformation, // transformation applied to the sprite
							const glm::vec4& color = glm::vec4(1.0f), // color that gets blended together with the sprite
							const glm::vec2& tiling = glm::vec2(1.0f), // the amount of tiling, 1.0 means the texture will be stretched across the whole polygon
							unsigned int iCellId = 0, // cellId if multiple frames are stored together in the same sprite image
							const std::string& tech = "sprite"
							) override;

	// Returns the resource manager
	virtual IResourceManager& GetResourceManager() override;

	// Get the display mode, return true on success, false on error
	virtual bool GetDisplayMode(int monitor, int mode, int& width, int& height) const override;

	// Get the current display mode, return true on success, false on error
	virtual bool GetDisplayMode(int& width, int& height) const override;

	// Returns the number of monitors active on the current system
	virtual int GetNumMonitors() const override;

	// Returns the number of video modes for the given monitor
	virtual int GetNumDisplayModes(int monitor) const override;

	// Returns a tight bounding box that fits around the text,
	// if out is expected to be in user space, set out's topLeft position vector before calling this method, 
	// else, the bottomRight vector of out will be the width and height of the string and the topLeft vector will be zeroed
	// Note: if str is NULL, then GetStringRec() terminates without modifying out
	virtual void GetStringRec(const char* str, float scale, FontAlignment alignment, Math::FRECT& inout) const override;

	// Sets the world space camera to use
	virtual void SetCamera(class Camera*) override;

	// Sets the color of the screen after it gets cleared
	virtual void SetClearColor(const glm::vec3& color) override;

	// Sets the display mode
	virtual void SetDisplayMode(int mode) override;

	// Sets the coordinate system to render all objects in(screen space or world space)
	virtual void SetRenderSpace(RenderSpace) override;

	// todo: add comments here
	virtual bool SetShaderValue(const std::string& shader, const std::string& location, float value) override;
	virtual bool SetShaderValue(const std::string& shader, const std::string& location, const glm::vec2& value) override;

	// True enables VSync
	// False disables VSync
	virtual void EnableVSync(bool) override;

	// Render everything that has been cached so far to the back buffer and then swap the back buffer with the front buffer
	virtual void Present() override;

	static void MonitorCallback(GLFWmonitor*, int);

private:

	static oglRenderer* s_pThis;

	Camera m_OrthoCamera;

	GLFWwindow* m_pWindow;

	ResourceManager m_rm;

	std::unique_ptr<FontEngine> m_pWorldSpaceFonts;
	std::unique_ptr<FontEngine> m_pScreenSpaceFonts;

	std::unique_ptr<LineEngine> m_pWorldSpaceLines;
	std::unique_ptr<LineEngine> m_pScreenSpaceLines;

	std::unique_ptr<SpriteEngine> m_pWorldSpaceSprites;
	std::unique_ptr<SpriteEngine> m_pScreenSpaceSprites;

	GLFWmonitor** m_pMonitors;
	int m_iMonitorCount;

	std::vector<std::pair<const GLFWvidmode*,int>> m_videoModes;

	int m_iCurrentMonitor;
	int m_iCurrentDisplayMode;

	RenderSpace m_renderSpace;

	bool m_bFullscreen;

	std::vector<VertexBuffer*> m_vertexBuffers;

	// Helper functions
	void GLFWOpenWindowHints();
	void ConfigureGLFW();
	void ConfigureOpenGL();
	void EnumerateDisplayAdaptors();
	bool CheckShader(const std::string& shader, const std::string& location, GLuint& shaderID, GLuint& outLocation) const;
	void ParseVideoSettingsFile();
	void SaveDisplayList();
	void BuildCamera();

	oglRenderer(const oglRenderer&) = delete;
	oglRenderer& operator = (const oglRenderer&) = delete;
};

#endif // _OGLRENDERER_

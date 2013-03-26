#ifndef _IRENDERER_
#define _IRENDERER_

#include "IPlugin.h"
#include "IResourceManager.h"
#include "VecMath.h"
#include <string>
#include <glm\glm.hpp>

class __declspec(novtable) I3DRenderer
{
public:

	virtual ~I3DRenderer() {}

	// todo: need to create an abstract form of vertex declarations

	/*virtual UINT CreateBillboarderdMesh(unsigned int n, const D3DXVECTOR3* pPosArray, unsigned int uiTech, const std::string& texture) = 0;
	virtual UINT CreateEffectFromFile(const char* file) = 0;
	virtual UINT GetTechnique(UINT uiEffect, const char* Tech) = 0;
	virtual void SetValue(UINT uiEffect, void* pData, UINT bytes) = 0;

	virtual UINT CreateMeshFromFile(const char* file) = 0;
	virtual UINT CreateTriGridMesh() = 0;

	virtual void RenderMesh(UINT) = 0;*/

};

class __declspec(novtable) IRenderer : public IPlugin
{
public:

	// mesh functions
	/*virtual int LoadMesh(char* pFile) = 0;
	virtual void DrawMesh(int id, int x, int y, int z) = 0;

	//shaders
	virtual int LoadShader(char* pFile) = 0;
	*/
	virtual void ClearScreen() = 0;
	virtual void Present() = 0;
	//virtual void DrawSprite();

	virtual void Reset() = 0;
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
	virtual bool IsDeviceLost() = 0;

	// whenever an ISpatialObject is passed to the renderer,
	// the renderer deals with rendering the object so the application does not have to worry about it.
	
	// textures
	virtual IResourceManager& GetResourceManager() = 0;

	// Line
	virtual void DrawLine(const glm::vec3* pArray, unsigned int length, const glm::vec4& color = glm::vec4(1.0f), const glm::mat4& t = glm::mat4(1.0f)) = 0;

	// Fonts
	virtual void GetStringRec(const char* str, const glm::vec2& pos, const glm::vec2& scale, Math::FRECT& out) = 0;

	virtual void DrawString(const char* str, // the string that gets drawn
							const glm::vec2& pos, // World pos of the text, where the text starts getting drawn from
							const char* font = nullptr, // the desired font, may be null if you wish to use the default font
							const glm::vec4& color = glm::vec4(1.0f,1.0f,1.0f,1.0f), // color of the text blended together with the texture
							const glm::vec2& scale = glm::vec2(1.0f,1.0f) // size of the text
							) = 0; // world space

	// sprites
	//virtual bool CreateSpritePool(const string& tech,const std::string& texture, unsigned int& id) = 0;
	//virtual bool DeleteSpritePool(unsigned int id) = 0;
	//virtual void DrawSprite(unsigned int iPool, const D3DXMATRIX& transformation) = 0;

	virtual void DrawSprite(const glm::mat4& transformation, // transformation applied to the sprite
							const std::string& texture, // texture used to draw the sprite
							unsigned int iCellId = 0, // cellId if multiple frames are stored together in the same sprite image 
							float dx = 1.0f, // the amount of tiling in the x direction, 1.0 means the texture will be stretched across the whole polygon 
							float dy = 1.0f // " y
							) = 0;

	virtual void SetCamera(class Camera*) = 0;


	// config
	virtual void EnableVSync(bool) = 0;
	virtual void EnumerateDisplayAdaptors() = 0;
	virtual unsigned int GetNumDisplayModes() const = 0;
	virtual unsigned int GetCurrentDisplayMode() const = 0;
	virtual void SetDisplayMode(unsigned int i) = 0; // full screen mode
	virtual const std::string& GetDisplayModeStr(unsigned int i) const = 0;
	virtual void ToggleFullscreen() = 0;
	
	///add more functions...
protected:

	virtual ~IRenderer() {}

};


#endif //_IRENDERER_

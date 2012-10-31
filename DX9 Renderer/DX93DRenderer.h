#ifndef _DX93DRENDERER_
#define _DX93DRENDERER_

#include "IRenderer.h"
#include "TextureManager.h"
#include <vector>

class DX93DRenderer : public I3DRenderer
{
public:

	DX93DRenderer(IDirect3DDevice9* pDevice, TextureManager* pTm);
	virtual ~DX93DRenderer();

	// todo: need to create an abstract form of vertex declarations

	virtual UINT CreateBillboarderdMesh(unsigned int n, const D3DXVECTOR3* pPosArray, unsigned int uiTech, const std::string& texture);
	virtual UINT CreateEffectFromFile(const char* file);
	virtual UINT GetTechnique(UINT uiEffect, const char* Tech);
	virtual void SetValue(UINT uiEffect, void* pData, UINT bytes);

	virtual UINT CreateMeshFromFile(const char* file);
	virtual UINT CreateTriGridMesh();

	virtual void RenderMesh(UINT);

private:

	//std::vector<


};

#endif // _DX93DRENDERER_
#include "DX92DRenderer.h"
#include "Streams.h"
#include "Camera.h"
#include "ResourceManager.h"

DX92DRenderer::DX92DRenderer(IDirect3DDevice9* pDevice, ResourceManager* pTm)
	: m_pDevice(pDevice), m_pResourceManager(pTm), m_pCamera(nullptr), m_fonts(pDevice,pTm,1024), m_lines(pDevice,pTm,512)
{
	InitializeSprite();
}


DX92DRenderer::~DX92DRenderer()
{
	ReleaseCamera(m_pCamera);
	m_pMesh->Release();
	m_pDevice->Release();
}

void DX92DRenderer::OnLostDevice()
{
	m_fonts.OnLostDevice();
	m_lines.OnLostDevice();
}

void DX92DRenderer::OnResetDevice()
{
	m_fonts.OnResetDevice();
	m_lines.OnResetDevice();
}

void DX92DRenderer::Begin()
{
}

void DX92DRenderer::End()
{
	Render();
}

void DX92DRenderer::SetCamera(Camera* pCam)
{
	if(m_pCamera != nullptr)
	{
		m_pCamera->Release();
	}

	m_fonts.SetCamera(pCam);
	m_lines.SetCamera(pCam);

	m_pCamera = pCam;
	m_pCamera->AddRef();
}

// Line
void DX92DRenderer::DrawLine(const D3DXVECTOR2* pVertexList, DWORD dwVertexListCount)
{
	m_lines.DrawLine(pVertexList,dwVertexListCount,D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f),0.0f);
}
void DX92DRenderer::DrawLine(const D3DXVECTOR3* pVertexList, DWORD dwVertexListCount, float angle, D3DCOLOR color)
{
	//m_lines.DrawLine(
}

// Fonts

void DX92DRenderer::GetStringRec(const char* str, const D3DXVECTOR2& pos, const D3DXVECTOR2& scale, Math::FRECT& out)
{
	m_fonts.GetStringRect(str,pos,scale,out);
	//m_pFont->DrawText(0,str,-1,&out,DT_CALCRECT | DT_TOP | DT_LEFT | DT_WORDBREAK,0);
}
void DX92DRenderer::DrawString(const char* str, const D3DXVECTOR2& pos,  const D3DXVECTOR2& scale, const D3DXVECTOR4& color) // not clipped
{
	m_fonts.DrawString(str,pos,scale,color);
}

// sprites
/*void DX92DRenderer::AddSprite(::ISpatialObject* pObj, const std::string& texture)
{
	m_quadTree.Insert(*pObj);
}*/
void DX92DRenderer::InitializeSprite()
{
	// Number of objs
	const UINT iNum = 1;

	ID3DXMesh* pTempMesh = 0;
	D3DVERTEXELEMENT9 elements[64];
	UINT iElements = 0;
	VertexPT::m_pVertexDecl->GetDeclaration(elements,&iElements);

	// Create in system mem, then copy over to the graphics card.
	D3DXCreateMesh(2,4,D3DXMESH_SYSTEMMEM,elements,m_pDevice,&pTempMesh);

	VertexPT* v = 0;
	pTempMesh->LockVertexBuffer(0,(void**)&v);

	for(UINT i = 0; i < iNum; ++i)
	{
		v[i*4].pos = D3DXVECTOR3(-0.5f,0.5f,0.0);
		v[i*4].tex = D3DXVECTOR2(0,0); // todo: to crop the texture, change the texture cords

		v[i*4 + 1].pos = D3DXVECTOR3(-0.5f,-0.5f,0.0);
		v[i*4 + 1].tex = D3DXVECTOR2(0,1);

		v[i*4 + 2].pos = D3DXVECTOR3(0.5f,0.5f,0.0);
		v[i*4 + 2].tex = D3DXVECTOR2(1,0);

		v[i*4 + 3].pos = D3DXVECTOR3(0.5f,-0.5f,0.0);
		v[i*4 + 3].tex = D3DXVECTOR2(1,1);
	}

	pTempMesh->UnlockVertexBuffer();
	v = 0;

	// Create index data
	WORD* index = 0;
	pTempMesh->LockIndexBuffer(0,(void**)&index);

	for(UINT i = 0; i < iNum; ++i)
	{
		// Tri 1
		index[i*6] = 4*i + 2;
		index[i*6 + 1] =  4*i + 1;
		index[i*6 + 2] = 4*i;

		//Tri 2
		//
		index[i*6 + 3] = 4*i + 2;
		index[i*6 + 4] = 4*i + 3;
		index[i*6 + 5] = 4*i + 1;

	}

	pTempMesh->CloneMesh(D3DXMESH_MANAGED,elements,m_pDevice,&m_pMesh);
	pTempMesh->Release();
}
void DX92DRenderer::DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iCellId, float dx, float dy, DWORD color)
{
	m_sprites.push_back(Sprite(transformation,texture,iCellId,dx,dy,color));
}

void DX92DRenderer::RenderSprites()
{
	static ResourceManager::Shader shader;
	m_pResourceManager->GetResource("2dshader",shader);

	while(!m_sprites.empty())
	{
		ResourceManager::Texture tex;
		m_pResourceManager->GetResource(m_sprites.back().texture,tex);

		// Set parameters in shader
		shader.pEffect->SetTechnique(shader.tech["ColorAnimatedSprite"]);
		shader.pEffect->SetTexture(shader.parameters["gTex"],tex.pTexture);
		shader.pEffect->SetFloat(shader.parameters["gXScale"],m_sprites.back().dx);
		shader.pEffect->SetFloat(shader.parameters["gYScale"],m_sprites.back().dy);
		shader.pEffect->SetInt(shader.parameters["gCurrentFrame"],m_sprites.back().uiCell);
		shader.pEffect->SetInt(shader.parameters["gSpriteWidth"],tex.uiCellsWidth);
		shader.pEffect->SetInt(shader.parameters["gSpriteHeight"],tex.uiCellsHeight);
		shader.pEffect->SetMatrix(shader.parameters["gWorldViewProj"],&(m_sprites.back().T * m_pCamera->viewProj()));

		// Start
		UINT i = 0;
		shader.pEffect->Begin(&i,0);

		// Begin rendering all passes through the shader
		for(UINT p = 0; p < i; ++p)
		{
			shader.pEffect->BeginPass(p);
		
			m_pMesh->DrawSubset(0);

			shader.pEffect->EndPass();
		}

		// End
		shader.pEffect->End();

		m_sprites.pop_back();
	}
}

void DX92DRenderer::Render()
{
	m_fonts.Render();
	m_lines.Render();
	RenderSprites();
}
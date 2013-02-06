#include "DX92DRenderer.h"
#include "Streams.h"
#include "Camera.h"
#include "ResourceManager.h"

DX92DRenderer::DX92DRenderer(IDirect3DDevice9* pDevice, ResourceManager* pTm)
	: m_pDevice(pDevice), m_pResourceManager(pTm), m_pCamera(nullptr), m_fonts(pDevice,pTm,1024), m_lines(pDevice,pTm,1024), m_sprites(pDevice,pTm,1024)
{
}


DX92DRenderer::~DX92DRenderer()
{
	ReleaseCamera(m_pCamera);
	m_pDevice->Release();
}

void DX92DRenderer::OnLostDevice()
{
	m_fonts.OnLostDevice();
	m_lines.OnLostDevice();
	m_sprites.OnLostDevice();
}

void DX92DRenderer::OnResetDevice()
{
	m_fonts.OnResetDevice();
	m_lines.OnResetDevice();
	m_sprites.OnResetDevice();
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
	m_sprites.SetCamera(pCam);

	m_pCamera = pCam;
	m_pCamera->AddRef();
}

// Line
void DX92DRenderer::DrawLine(const D3DXVECTOR3* pArray, unsigned int length)
{
	::D3DXVECTOR4 color(1.0f,1.0f,1.0f,1.0f);
	::D3DXMATRIX I;
	::D3DXMatrixIdentity(&I);

	DrawLine(pArray,length,color,I);
}
void DX92DRenderer::DrawLine(const D3DXVECTOR3* pArray, unsigned int length, const D3DXVECTOR4& color, const D3DXMATRIX& t)
{
	m_lines.DrawLine(pArray,length,color,t);
}

// Fonts

void DX92DRenderer::GetStringRec(const char* str, const D3DXVECTOR2& pos, const D3DXVECTOR2& scale, Math::FRECT& out)
{
	m_fonts.GetStringRect(str,pos,scale,out);
	//m_pFont->DrawText(0,str,-1,&out,DT_CALCRECT | DT_TOP | DT_LEFT | DT_WORDBREAK,0);
}
void DX92DRenderer::DrawString(const char* str, const D3DXVECTOR2& pos, const char* font, const D3DXVECTOR4& color, const D3DXVECTOR2& scale) // not clipped
{
	m_fonts.DrawString(str,font,pos,scale,color);
}

void DX92DRenderer::DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iCellId, float dx, float dy, DWORD color)
{
	m_sprites.DrawSprite("ColorAnimatedSprite",transformation,texture,iCellId,dx,dy);
}

void DX92DRenderer::Render()
{
	m_sprites.Render();
	m_fonts.Render();
	m_lines.Render();
}
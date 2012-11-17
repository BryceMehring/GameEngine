#include "DX92DRenderer.h"

DX92DRenderer::DX92DRenderer(IDirect3DDevice9* pDevice, TextureManager* pTm) : m_pDevice(pDevice), m_pTextureManager(pTm)
{
	InitializeFont();
	InitializeLine();
	InitializeSprite();
}


DX92DRenderer::~DX92DRenderer()
{
	m_pSprite->Release();
	m_pLine->Release();
	m_pFont->Release();
	m_pDevice->Release();
}

void DX92DRenderer::InitializeLine()
{
	D3DXCreateLine(m_pDevice,&m_pLine);
	m_pLine->SetAntialias(true);
	m_pLine->SetWidth(1.0f);
	
}
void DX92DRenderer::InitializeSprite()
{
	D3DXCreateSprite(m_pDevice,&m_pSprite);

	m_pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);

	m_pDevice->SetRenderState(D3DRS_LIGHTING,false);
}

void DX92DRenderer::InitializeFont()
{
	D3DXFONT_DESC desc;
	ZeroMemory(&desc,sizeof(desc));
	desc.Height = 15; // 15
	desc.Width = 7; // 8
	desc.Weight = 500;
	desc.Quality = 255;
	desc.MipLevels = 1;
	//::sprintf_s(desc.FaceName,"Browallia New");
	//_tcscpy(desc.FaceName, _T("Papyrus"))

	D3DXCreateFontIndirect(m_pDevice,&desc,&m_pFont);
	
}


void DX92DRenderer::OnLostDevice()
{
	m_pSprite->OnLostDevice();
	m_pLine->OnLostDevice();
	m_pFont->OnLostDevice();
}

void DX92DRenderer::OnResetDevice()
{
	m_pSprite->OnResetDevice();
	m_pLine->OnResetDevice();
	m_pFont->OnResetDevice();
}

void DX92DRenderer::Begin()
{
	m_pLine->Begin();
}

void DX92DRenderer::End()
{
	Render();
}

// Line
void DX92DRenderer::DrawLine(const D3DXVECTOR2* pVertexList, DWORD dwVertexListCount, D3DCOLOR color)
{
	m_pLine->Draw(pVertexList,dwVertexListCount,color);
}
void DX92DRenderer::DrawLine(const D3DXVECTOR3* pVertexList, DWORD dwVertexListCount, float angle, D3DCOLOR color)
{
	::D3DXMATRIX R;
	::D3DXMatrixRotationZ(&R,angle);

	m_pLine->DrawTransform(pVertexList,dwVertexListCount,&R,color);
}

// Fonts
void DX92DRenderer::GetStringRec(const char* str, RECT& out)
{
	m_pFont->DrawText(this->m_pSprite,str,-1,&out,DT_CALCRECT | DT_TOP | DT_LEFT | DT_WORDBREAK,0);
}
void DX92DRenderer::DrawString(const char* str, POINT P, DWORD color) // not clipped
{
	RECT R = {P.x,P.y};
	//DrawTextInfo info = {str,R,color,DT_NOCLIP};
	//m_text.push_back(DrawTextInfo(str,R,color,format));
	m_text.push_back(DrawTextInfo(str,R,color,DT_NOCLIP));
}
void DX92DRenderer::DrawString(const char* str, RECT& R, DWORD color, bool calcRect) // clipped to rect
{
	if(calcRect)
	{
		GetStringRec(str,R);
	}

	m_text.push_back(DrawTextInfo(str,R,color,DT_NOCLIP));
	//int Height = this->m_pFont->DrawText(NULL,str,50,&R,DT_TOP | DT_LEFT | DT_WORDBREAK,color);
	//DrawTextInfo info = {str,R,color,DT_TOP | DT_LEFT | DT_WORDBREAK};
	//DrawTextInfo info = {str,R,color,DT_NOCLIP};
	//m_text.push_back(info);
}

// sprites
void DX92DRenderer::DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iPriority, unsigned int iCellId, DWORD color)
{
	m_sprites.push_back(Sprite(transformation,texture,color,iPriority,iCellId));
}
//virtual void DrawSpriteAnimation(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iPriority, DWORD color = 0xffffffff);

void DX92DRenderer::RenderText()
{
	while(!m_text.empty())
	{
		DrawTextInfo& info = m_text.back();
		m_pFont->DrawText(m_pSprite,info.text.c_str(),-1,&info.R,info.format,info.color);
		m_text.pop_back();
	}
}

void DX92DRenderer::RenderSprites()
{
	m_sprites.sort(SpriteSorter());

	for(auto iter = m_sprites.begin(); iter != m_sprites.end(); )
	{
		// top sprite in the queue
		const Sprite& top = *iter;

		// Get texture for the sprite
		TextureManager::Texture texture;
		if(m_pTextureManager->GetTexture(top.texture,texture))
		{
			m_pSprite->SetTransform(&(top.T));

			const unsigned int uiWidth = texture.uiWidth / texture.uiCells;
			const unsigned int uiCell = top.uiCell;
			const RECT R = {uiCell * uiWidth,0,(uiCell+1) * uiWidth,texture.uiHeight};

			// calculate center of texture
			D3DXVECTOR3 center = D3DXVECTOR3((R.right - R.left) / 2.0f,(R.bottom - R.top) / 2.0f,0.0f);

			m_pSprite->Draw(texture.pTexture,&R,&center,0,top.Color);
		}

		iter = m_sprites.erase(iter);
	}

	m_pSprite->Flush();

	// reset to normal transform matrix after rendering

	D3DXMATRIX T;
	D3DXMatrixIdentity(&T);
	
	m_pSprite->SetTransform(&T);
}

void DX92DRenderer::Render()
{
	m_pSprite->Begin(0);

	RenderText();
	RenderSprites();

	m_pSprite->End();
	m_pLine->End();
}
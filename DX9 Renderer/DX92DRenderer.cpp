#include "DX92DRenderer.h"
#include "Streams.h"
#include "Camera.h"

DX92DRenderer::DX92DRenderer(IDirect3DDevice9* pDevice, ResourceManager* pTm)
	: m_pDevice(pDevice), m_pResourceManager(pTm), m_pCamera(nullptr)
{
	InitializeFont();
	InitializeLine();
	InitializeSprite();
}


DX92DRenderer::~DX92DRenderer()
{
	ReleaseCamera(m_pCamera);
	m_pMesh->Release();
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
	m_pLine->OnLostDevice();
	m_pFont->OnLostDevice();
}

void DX92DRenderer::OnResetDevice()
{
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

void DX92DRenderer::SetCamera(Camera* pCam)
{
	if(m_pCamera != nullptr)
	{
		m_pCamera->Release();
	}

	m_pCamera = pCam;
	m_pCamera->AddRef();
}

// Line
/*void DX92DRenderer::DrawLine(const D3DXVECTOR2* pVertexList, DWORD dwVertexListCount, D3DCOLOR color)
{
	m_pLine->Draw(pVertexList,dwVertexListCount,color);
}*/
void DX92DRenderer::DrawLine(const D3DXVECTOR3* pVertexList, DWORD dwVertexListCount, float angle, D3DCOLOR color)
{
	// todo: get rid of m_pLine interfaces
	::D3DXMATRIX R;
	::D3DXMatrixRotationZ(&R,angle);

	m_pLine->DrawTransform(pVertexList,dwVertexListCount,&R,color);
}

// Fonts
void DX92DRenderer::GetStringRec(const char* str, RECT& out)
{
	m_pFont->DrawText(0,str,-1,&out,DT_CALCRECT | DT_TOP | DT_LEFT | DT_WORDBREAK,0);
}
void DX92DRenderer::DrawString(const char* str, D3DXVECTOR2 pos, DWORD color) // not clipped
{
	//transform point in world space into screens pace
	D3DXVec2TransformCoord(&pos,&pos,&(m_pCamera->viewProj()));

	RECT R;
	GetClientRect(GetActiveWindow(),&R);

	pos.x = (pos.x * 0.5f + 0.5f) * (R.right - R.left);
	pos.y = (1.0f - (pos.y * 0.5f + 0.5f)) * (R.bottom - R.top);

	POINT P = {pos.x,pos.y};
	DrawString(str,P,color);
}

void DX92DRenderer::DrawString(const char* str, const POINT& pos, DWORD color) // not clipped
{
	RECT R = {pos.x,pos.y,0,0};
	m_text.push_back(DrawTextInfo(str,R,color,DT_NOCLIP));
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

	// todo: this could be moved elsewhere
	//ResourceManager::Shader meshShader;
	//m_pResourceManager->GetResource("2DShader",meshShader);

	/*D3DXHANDLE hTech = m_pEffect->GetTechniqueByName("Blending");

	Mtrl* pMtrl =  new Mtrl;
	pMtrl->diffuse = D3DXCOLOR(0.9f,0.9f,0.9f,0.0f);
	pMtrl->ambient = D3DXCOLOR(0.0f,0.0f,0.0f,0.0f);
	pMtrl->spec = D3DXCOLOR(0.2f,0.2f,0.2f,0.0f);*/

	// Create Billboard object 
	//m_pEffect->AddRef();
	//m_Objects.push_back(new UserControlledMesh(pMesh,m_pEffect,pTex,pMtrl,hTech,pos));
}
void DX92DRenderer::DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iCellId, DWORD color)
{
	m_sprites.push_back(Sprite(transformation,texture,color,iCellId));
}

void DX92DRenderer::RenderText()
{
	while(!m_text.empty())
	{
		DrawTextInfo& info = m_text.back();
		m_pFont->DrawText(0,info.text.c_str(),-1,&info.R,info.format,info.color);
		m_text.pop_back();
	}
}

void DX92DRenderer::RenderSprites()
{
	ResourceManager::Shader shader;
	m_pResourceManager->GetResource("2dshader",shader);
	D3DXHANDLE tecHandle = shader.pEffect->GetTechniqueByName("AnimatedSprite");

	while(!m_sprites.empty())
	{
		ResourceManager::Texture tex;
		m_pResourceManager->GetResource(m_sprites.back().texture,tex);

		// Set parameters in shader
		shader.pEffect->SetTechnique(tecHandle);
		shader.pEffect->SetTexture("gTex",tex.pTexture); 
		shader.pEffect->SetInt("gCurrentFrame",m_sprites.back().uiCell);
		shader.pEffect->SetInt("gSpriteWidth",tex.uiCellsWidth);
		shader.pEffect->SetInt("gSpriteHeight",tex.uiCellsHeight);
		shader.pEffect->SetMatrix("gWorldViewProj",&(m_sprites.back().T * m_pCamera->viewProj()));

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
	RenderSprites();
	RenderText();

	m_pLine->End();
}
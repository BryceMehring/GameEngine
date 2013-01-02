#include "DX92DRenderer.h"
#include "Streams.h"
#include "Camera.h"
#include "ResourceManager.h"
#include <DxErr.h>

FontEngine::FontEngine(IDirect3DDevice9* pDevice, ResourceManager* pTm, int maxLength)
	: m_pDevice(pDevice), m_pRM(pTm), m_pVertexBuffer(nullptr), m_pIndexBuffer(nullptr)
	, m_iMaxLength(maxLength), m_pCamera(nullptr)
{
	CreateBuffers();
}

FontEngine::~FontEngine()
{
	if(m_pVertexBuffer != nullptr)
	{
		m_pVertexBuffer->Release();
	}

	if(m_pIndexBuffer != nullptr)
	{
		m_pIndexBuffer->Release();
	}
	//m_pDevice->Release();
}

void FontEngine::DrawString(const char* str, const D3DXVECTOR2& pos, const D3DXVECTOR4& color)
{
	if(str == nullptr)
		return;

	m_textSubsets.push_back(DrawTextInfo(str,pos,color));
}

void FontEngine::FillVertexBuffer()
{
	VertexPT* v = nullptr;
	m_pVertexBuffer->Lock(0,0,(void**)&v,D3DLOCK_DISCARD);

	unsigned int iCurrentVerts = 0;

	for(auto iter = m_textSubsets.begin(); iter != m_textSubsets.end(); ++iter)
	{
		const char* str = iter->text.c_str();

		unsigned int i = 0;
		unsigned int j = 0;
		unsigned int iVert = iCurrentVerts;

		while((iVert < m_iMaxLength) && *str)
		{
			if(*str == '\n')
			{
				i = 0;
				j++;
				str++;
				continue;
			}

			int x = *str % 16; // Column
			int y = *str / 16; // Row

			// tex coords
			D3DXVECTOR2 topLeft(x / 16.0f,y / 16.0f);
			D3DXVECTOR2 bottomRight((x+1) / 16.0f,(y+1) / 16.0f);

			D3DXVECTOR3 posW(iter->pos.x + i / 2.0,iter->pos.y - j,0.0f);

			unsigned int index = iVert * 4;

			v[index].pos = D3DXVECTOR3(-0.5f,0.5f,0.0f) + posW;
			v[index].tex = topLeft;

			v[index + 1].pos = D3DXVECTOR3(-0.5f,-0.5f,0.0f) + posW;
			v[index + 1].tex = D3DXVECTOR2(topLeft.x,bottomRight.y);

			v[index + 2].pos = D3DXVECTOR3(0.5f,0.5f,0.0f) + posW;
			v[index + 2].tex = D3DXVECTOR2(bottomRight.x,topLeft.y);

			v[index + 3].pos = D3DXVECTOR3(0.5f,-0.5f,0.0f) + posW;
			v[index + 3].tex = bottomRight;

			++i;
			++iVert;
			++str;
		}

		iter->length = iVert - iCurrentVerts;
		iCurrentVerts += iter->length;
	}

	m_pVertexBuffer->Unlock();

}

void FontEngine::Render()
{
	if(m_textSubsets.empty())
		return;

	FillVertexBuffer();

	m_pDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(VertexPT));
	m_pDevice->SetIndices(m_pIndexBuffer);
	m_pDevice->SetVertexDeclaration(VertexPT::m_pVertexDecl);

	ResourceManager::Shader shader;
	m_pRM->GetResource("2dshader",shader);

	ResourceManager::Texture texture;
	m_pRM->GetResource("font",texture);

	::D3DXMATRIX S;
	::D3DXMatrixScaling(&S,4.0f,4.0f,1.0f);

	shader.pEffect->SetTechnique(shader.tech[2]);
	shader.pEffect->SetTexture(shader.parameters[5],texture.pTexture);
	shader.pEffect->SetMatrix(shader.parameters[3],&(S* m_pCamera->viewProj()));

	UINT p = 0;
	shader.pEffect->Begin(&p,0);

	for(UINT i = 0; i < p; ++i)
	{
		shader.pEffect->BeginPass(i);

		int uiStartingPoint = 0;

		for(unsigned int j = 0; j < m_textSubsets.size(); ++j)
		{
			shader.pEffect->SetVector(shader.parameters[11],&m_textSubsets[j].color);
			shader.pEffect->CommitChanges();

			assert(m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,4*(m_textSubsets[j].length),6*uiStartingPoint,2*(m_textSubsets[j].length)) == S_OK);

			uiStartingPoint += m_textSubsets[j].length;
		}

		shader.pEffect->EndPass();
	}

	shader.pEffect->End();

	m_textSubsets.clear();
}

void FontEngine::OnLostDevice()
{
	if(m_pVertexBuffer != nullptr)
	{
		m_pVertexBuffer->Release();
		m_pIndexBuffer->Release();
		m_pIndexBuffer = nullptr;
		m_pVertexBuffer = nullptr;
	}
}

void FontEngine::OnResetDevice()
{
	if(m_pVertexBuffer == nullptr)
	{
		CreateBuffers();
	}
}

void FontEngine::CreateVertexBuffer()
{
	if(m_pVertexBuffer == nullptr)
	{
		assert(m_pDevice->CreateVertexBuffer(m_iMaxLength*4 * sizeof(VertexPT),D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&m_pVertexBuffer,0) == S_OK);
	}
}

void FontEngine::CreateIndexBuffer()
{
	if(m_pIndexBuffer == nullptr)
	{
		assert(m_pDevice->CreateIndexBuffer(m_iMaxLength*6 * sizeof(WORD),0,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&m_pIndexBuffer,0) == S_OK);

		WORD* index = nullptr;
		m_pIndexBuffer->Lock(0,0,(void**)&index,0);

		for(UINT i = 0; i < m_iMaxLength; ++i)
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
		m_pIndexBuffer->Unlock();
	}
}

void FontEngine::CreateBuffers()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
}

DX92DRenderer::DX92DRenderer(IDirect3DDevice9* pDevice, ResourceManager* pTm)
	: m_pDevice(pDevice), m_pResourceManager(pTm), m_pCamera(nullptr), m_fonts(pDevice,pTm,200)
{
	InitializeLine();
	InitializeSprite();
}


DX92DRenderer::~DX92DRenderer()
{
	ReleaseCamera(m_pCamera);
	m_pMesh->Release();
	m_pLine->Release();
	m_pDevice->Release();
}

void DX92DRenderer::InitializeLine()
{
	D3DXCreateLine(m_pDevice,&m_pLine);
	m_pLine->SetAntialias(true);
	m_pLine->SetWidth(1.0f);
	
}

void DX92DRenderer::OnLostDevice()
{
	m_fonts.OnLostDevice();
	m_pLine->OnLostDevice();
}

void DX92DRenderer::OnResetDevice()
{
	m_fonts.OnResetDevice();
	m_pLine->OnResetDevice();
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

	m_fonts.SetCamera(pCam);

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
	//m_pFont->DrawText(0,str,-1,&out,DT_CALCRECT | DT_TOP | DT_LEFT | DT_WORDBREAK,0);
}
void DX92DRenderer::DrawString(const char* str, D3DXVECTOR2 pos, const D3DXVECTOR4& color) // not clipped
{
	m_fonts.DrawString(str,pos,color);
	//transform point in world space into screens pace
	/*D3DXVec2TransformCoord(&pos,&pos,&(m_pCamera->viewProj()));

	RECT R;
	GetClientRect(GetActiveWindow(),&R);

	pos.x = (pos.x * 0.5f + 0.5f) * (R.right - R.left);
	pos.y = (1.0f - (pos.y * 0.5f + 0.5f)) * (R.bottom - R.top);

	POINT P = {pos.x,pos.y};
	DrawString(str,P,color);*/
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

	//D3DXLoadMeshFromX("..\\textures\\sprite.x",D3DXMESH_MANAGED,m_pDevice,0,0,0,0,&m_pMesh);
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
		shader.pEffect->SetTechnique(shader.tech[1]);
		shader.pEffect->SetTexture(shader.parameters[5],tex.pTexture);
		shader.pEffect->SetFloat(shader.parameters[6],m_sprites.back().dx);
		shader.pEffect->SetFloat(shader.parameters[7],m_sprites.back().dy);
		shader.pEffect->SetInt(shader.parameters[10],m_sprites.back().uiCell);
		shader.pEffect->SetInt(shader.parameters[8],tex.uiCellsWidth);
		shader.pEffect->SetInt(shader.parameters[9],tex.uiCellsHeight);
		shader.pEffect->SetMatrix(shader.parameters[3],&(m_sprites.back().T * m_pCamera->viewProj()));

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
	RenderSprites();

	m_pLine->End();
}
// Programmed by Bryce Mehring

#include "StdAfx.h"
#include "BFont.h"
#include "BEngine.h"

IMPL_SINGLETON(BFont);

BFont::BFont()
{
	D3DXFONT_DESC desc;

	desc.Height = 15;
	desc.Width = 5;
	desc.Weight = 50;
	desc.MipLevels = 1;
	desc.Italic = false;
	desc.CharSet = 0;

	CreateFont("default",desc);

}

BFont::~BFont()
{
	for(FontType::iterator iter = m_fonts.begin(); iter != m_fonts.end(); ++iter)
	{
		iter->second->Release();
	}
}

void BFont::CreateFont(const string& name, const D3DXFONT_DESC& desc)
{
	BEngine* pEngine = BEngine::GetInstance();
	IDirect3DDevice9* pDevice = pEngine->GetDevice();
	
	ID3DXFont* pFont = NULL;
	D3DXCreateFontIndirect(pDevice,&desc,&pFont);
	pDevice->Release();

	m_fonts.insert(make_pair(name,pFont));
}

ID3DXFont* BFont::GetFont(const string& name)
{
	FontType::iterator iter = m_fonts.find(name);

	if(iter != m_fonts.end())
	{
		iter->second->AddRef();
	}

	return iter->second;
}
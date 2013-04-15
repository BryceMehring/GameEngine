
// Programmed by Bryce Mehring

#pragma once
#include "Singleton.h"

using namespace stdext;
using namespace std;

class BFont
{
public:

	DECLARE_SINGLETON(BFont);

	void CreateFont(const string& name, const D3DXFONT_DESC& desc);
	ID3DXFont* GetFont(const string& name);

private:

	typedef hash_map<string,ID3DXFont*> FontType;
	FontType m_fonts;

	BFont();
	~BFont();


};
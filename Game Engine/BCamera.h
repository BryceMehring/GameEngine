
#pragma once
#include <d3dx9math.h>

class BCamera
{
public:

	// Initializes DirextX along with Windows
	static bool Initialize();
	
	// Deletes everything from memory
	static bool Delete();

	static BCamera* GetInstance();
	
	void BuildProjectMatrix(float X, float Y);
	void BuildViewMatrix(const D3DXVECTOR3& pos);

	// Get Matrices
	void GetMatrices(const D3DXMATRIX** ppVPOut) const;

private:

	static BCamera* s_pInstance;

	D3DXMATRIX m_View;
	D3DXMATRIX m_Proj;

	BCamera();
	~BCamera();


};


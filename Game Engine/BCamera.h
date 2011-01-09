
#pragma once
#include "Singleton.h"

class BCamera
{
public:

	DECLARE_SINGLETON(BCamera);
	
	void BuildProjectMatrix(float X, float Y);
	void BuildViewMatrix(const D3DXVECTOR3& pos);

	// Get Matrices
	void GetMatrices(const D3DXMATRIX** ppVPOut) const;

private:

	D3DXMATRIX m_View;
	D3DXMATRIX m_Proj;

	BCamera();
	~BCamera();


};


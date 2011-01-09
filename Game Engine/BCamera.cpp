#include "StdAfx.h"
#include "BCamera.h"

IMPL_SINGLETON(BCamera);

void BCamera::BuildProjectMatrix(float X, float Y)
{
	// create transformation matrices
	D3DXMatrixPerspectiveFovLH(&m_Proj,D3DX_PI * 0.25f,X/Y,1.0f,5000.0f);
}

void BCamera::BuildViewMatrix(const D3DXVECTOR3& pos)
{
	static D3DXVECTOR3 at(0,0,0);
	static D3DXVECTOR3 up(0,1,0);

	D3DXMatrixLookAtLH(&m_View,&pos,&at,&up);
}

void BCamera::GetMatrices(const D3DXMATRIX** ppVPOut) const
{
	*ppVPOut = &(m_View * m_Proj);
}


BCamera::BCamera()
{
}


BCamera::~BCamera()
{
}

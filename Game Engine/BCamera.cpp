#include "BCamera.h"

BCamera* BCamera::s_pInstance = 0;

BCamera* BCamera::Initialize()
{
	if(s_pInstance == NULL)
	{
		s_pInstance = new BCamera();
		return s_pInstance;
	}

	return 0;
}

bool BCamera::Delete()
{
	if(s_pInstance)
	{
		delete s_pInstance;

		return true;
	}

	return false;
}

BCamera* BCamera::GetInstance()
{
	return s_pInstance;
}

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

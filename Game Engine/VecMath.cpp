#include "VecMath.h"


D3DXVECTOR2 Reflect(const D3DXVECTOR2& dir, const D3DXVECTOR2& normal)
{
	return 2*normal*D3DXVec2Dot(&dir,&normal) - dir;
}

D3DXVECTOR3 Reflect(const D3DXVECTOR3& dir, const D3DXVECTOR3& normal)
{
	return 2*normal*D3DXVec3Dot(&dir,&normal) - dir;
}
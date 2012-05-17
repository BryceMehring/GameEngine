#include "VecMath.h"


D3DXVECTOR2 Reflect(const D3DXVECTOR2& dir, const D3DXVECTOR2& normal)
{
	return 2*normal*D3DXVec2Dot(&dir,&normal) - dir;
}

D3DXVECTOR3 Reflect(const D3DXVECTOR3& dir, const D3DXVECTOR3& normal)
{
	return 2*normal*D3DXVec3Dot(&dir,&normal) - dir;
}

bool IsPointInPolygon(const D3DXVECTOR2* pArray, unsigned int length, POINT P)
{
	int j= length - 1;
	bool oddNodes = false;

	for(unsigned int i=0; i < length; i++)
	{
		if ((pArray[i].y< P.y && pArray[j].y>= P.y ||   pArray[j].y< P.y && pArray[i].y>=P.y) &&  (pArray[i].x<=P.x || pArray[j].x<=P.x))
		{
			oddNodes^=(pArray[i].x+(P.y-pArray[i].y)/(pArray[j].y-pArray[i].y)*(pArray[j].x-pArray[i].x)<P.x);
		}
		j=i;
	}

	return oddNodes; 
}
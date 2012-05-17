#ifndef _VECMATH_
#define _VECMATH_

#include "d3dx9math.h"

D3DXVECTOR2 Reflect(const D3DXVECTOR2& dir, const D3DXVECTOR2& normal);
D3DXVECTOR3 Reflect(const D3DXVECTOR3& dir, const D3DXVECTOR3& normal);

// ray casting algorithm
bool IsPointInPolygon(const D3DXVECTOR2* pArray, unsigned int length, POINT P);

// todo: need to implement
bool IsPointInPolygon(const D3DXVECTOR3* pArray, unsigned int length, POINT P);


#endif // _VECMATH_
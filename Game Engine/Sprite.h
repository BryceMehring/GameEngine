#ifndef _SPRITE_
#define _SPRITE_
#include "VecMath.h"

// todo maybe move this to another namespace
namespace Math
{

class Sprite : public FRECT
{
public:

	Sprite() {}
	Sprite(float width, float height, const D3DXVECTOR2& pos) 
	: FRECT(D3DXVECTOR2(pos.x - width / 2.0f,pos.y + height / 2.0f), D3DXVECTOR2(pos.x + width / 2.0f,pos.y - height / 2.0f))
	{
	}
};

}

#endif // _SPRITE_
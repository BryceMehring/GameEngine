// Programmed by Bryce Mehring

#include "VecMath.h"

// function declarations 
bool Intersects(const Circle& c1, const FRECT& R1);
bool Intersects(const Circle& c1, const Circle& c2);
bool Intersects(const FRECT& c1, const FRECT& c2);

/*bool ICollisionPolygon::Intersects(const ICollisionPolygon* pOther) const
{
	if(pOther == this) { return true; }

	Type thisType = this->GetType();
	Type otherType = pOther->GetType();

	switch(thisType)
	{
		case ICollisionPolygon::CircleType:
		{
			break;
		}
		case ICollisionPolygon::RectangleType:
		{
			break;
		}
	}
}*/

// ----- CCircle -----

CCircle::CCircle(const ::Circle& circle) : m_circle(circle)
{
}

bool CCircle::Intersects(const ICollisionPolygon* pOther) const
{
	// exclude collision with the same object 
	if(pOther == this) { return true; }

	// Get the type of the other object
	ICollisionPolygon::Type polyType = pOther->GetType();
	bool bSuccess = false;

	switch(polyType)
	{
		case ICollisionPolygon::CircleType:
		{
			const CCircle* pCircle = static_cast<const CCircle*>(pOther);
			bSuccess = ::Intersects(m_circle,pCircle->m_circle);
			break;
		}

		case ICollisionPolygon::RectangleType:
		{
			const CRectangle* pRectangle = static_cast<const CRectangle*>(pOther);
			bSuccess = ::Intersects(m_circle,pRectangle->GetRect());
			break;
		}
	}

	return bSuccess;
}

// ----- CCircle -----

CRectangle::CRectangle(const FRECT& rect) : m_rect(rect)
{
}

bool CRectangle::Intersects(const ICollisionPolygon* pOther) const
{
	if(pOther == this) { return true; }

	ICollisionPolygon::Type polyType = pOther->GetType();
	bool bSuccess = false;

	switch(polyType)
	{
		case ICollisionPolygon::CircleType:
		{
			const CCircle* pCircle = static_cast<const CCircle*>(pOther);
			bSuccess = ::Intersects(pCircle->GetCircle(),m_rect);
			break;
		}

		case ICollisionPolygon::RectangleType:
		{
			const CRectangle* pRectangle = static_cast<const CRectangle*>(pOther);
			bSuccess = ::Intersects(m_rect,pRectangle->m_rect);
			break;
		}
	}

	return bSuccess;
}


D3DXVECTOR2 Reflect(const D3DXVECTOR2& dir, const D3DXVECTOR2& normal)
{
	return 2*normal*D3DXVec2Dot(&dir,&normal) - dir;
}

D3DXVECTOR3 Reflect(const D3DXVECTOR3& dir, const D3DXVECTOR3& normal)
{
	return 2*normal*D3DXVec3Dot(&dir,&normal) - dir;
}

/*void NormalizeScreenRect(const RECT& R, FRECT& out)
{
	RECT window;
	GetWindowRect(GetActiveWindow(),&window);

	float fWidth = (float)window.right - window.left;
	float fHeight = (float)window.bottom - window.top;
	float fAspect = fWidth / fHeight;
	
	// todo: create a function that uses this algorithm to reduce code
	out.left = (R.left/(fWidth*0.5f)-1.0f/fAspect);
	out.top = (1.0f-(R.top/(fHeight*0.5f)));
	out.right = (R.right/(fWidth*0.5f)-1.0f/fAspect);
	out.bottom = 1.0f-(R.bottom/(fHeight*0.5f));
}
*/
/*double intersectRaySphere(D3DVECTOR rO, D3DVECTOR rV, D3DVECTOR sO, double sR) {
	
   D3DVECTOR Q = sO-rO;
   double c = lengthOfVector(Q);
   double v = dot(Q,rV);
   double d = sR*sR - (c*c - v*v);

   // If there was no intersection, return -1
   if (d < 0.0) return (-1.0f);

   // Return the distance to the [first] intersecting point
   return (v - sqrt(d));
}
*/
float RayCircleInsersection(const D3DXVECTOR2& center, float r, const D3DXVECTOR2& pos, const D3DXVECTOR2& dir)
{
	D3DXVECTOR2 Q = center - pos;
	float c = D3DXVec2Length(&Q);
	float v = D3DXVec2Dot(&Q,&dir);
	float d = r * r - (c * c - v * v);

	if(d < 0.0f) { return -1.0f; }

	return (v - sqrt(d));
}

bool Intersects(const Circle& c1, const FRECT& R1)
{
	// todo: rewrite this with directx vectors

	float closestX = Clamp(c1.center.x, R1.topLeft.x, R1.bottomRight.x);
	float closestY = Clamp(c1.center.y, R1.topLeft.y, R1.bottomRight.y);

	// Calculate the distance between the circle's center and this closest point
	float distanceX = c1.center.x - closestX;
	float distanceY = c1.center.y - closestY;

	// If the distance is less than the circle's radius, an intersection occurs
	float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
	return distanceSquared < (c1.r * c1.r);

	/*return R1.IsPointWithin(c1.center) || 
		c1.IsPointWithin(R1.topLeft) ||
		c1.IsPointWithin(R1.bottomRight) ||
		c1.IsPointWithin(D3DXVECTOR2(R1.topLeft.y,R1.bottomRight.x)) ||
		c1.IsPointWithin(D3DXVECTOR2(R1.bottomRight.y,R1.topLeft.x));*/
}

//Returns true if the circles are touching, or false if they are not
bool Intersects(const Circle& c1, const Circle& c2)
{
	// todo: check to make sure this is correct
	float fSum = c1.r + c2.r;
	return D3DXVec2LengthSq(&(c1.center - c2.center)) < (fSum * fSum);
	///float dist = D3DXVec2LengthSq(&(c1.center - c2.center)) - (c1.r + c2.r);
	//return (dist < 0);
}

bool Intersects(const FRECT& rect1, const FRECT& rect2)
{
	return rect2.IsPointWithin(rect1.topLeft) || 
		rect2.IsPointWithin(rect1.bottomRight) ||
		rect2.IsPointWithin(D3DXVECTOR2(rect1.topLeft.x,rect1.bottomRight.y)) ||
		rect2.IsPointWithin(D3DXVECTOR2(rect1.bottomRight.x,rect1.topLeft.y));
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

float GetRandFloat(float a, float b)
{
	// todo: need to check this
	float fRand = (rand() % 10001) * 0.0001f;
	return fRand*(b - a) + a;
}

float Clamp(float x, float a, float b)
{
	return x < a ? a : (x > b ? b : x);
}
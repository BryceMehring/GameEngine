// Programmed by Bryce Mehring

#include "VecMath.h"
#include "asVM.h"

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

void CCircle::GetNormal(const D3DXVECTOR2& pos, D3DXVECTOR2& out) const
{
	out = pos - m_circle.center;
	D3DXVec2Normalize(&out,&out);
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

void CRectangle::GetNormal(const D3DXVECTOR2& pos, D3DXVECTOR2& out) const
{
	//D3DXVECTOR2 tempPos = pos;
	//tempPos.x = m_rect.topLeft.x;
	//if(pos.x < this->m_rect.bottomRight.x)
	// todo: rename this class as a paddleCollisionRect and also
	// todo: keep this current class for basic rectangle collision
	{
		D3DXVECTOR2 middle = m_rect.Middle();
		// todo: use the && operator to fix this logic
		if(pos.x <= m_rect.topLeft.x)
		{
			middle.x += 200;
		}
		else if(pos.x >= m_rect.bottomRight.x)
		{
			middle.x -= 200;
		}
		else
		{
			middle.y = 0;
		}
		out = pos - middle;
		D3DXVec2Normalize(&out,&out);
	}
	
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
	/*return rect1.IsPointWithin(rect2.bottomRight) ||
		rect1.IsPointWithin(rect2.topLeft) ||
		rect1.IsPointWithin(D3DXVECTOR2(rect2.topLeft.x,rect2.bottomRight.y)) ||
		rect1.IsPointWithin(D3DXVECTOR2(rect2.bottomRight.x,rect2.bottomRight.y));*/

	return (rect1.topLeft.x <= rect2.bottomRight.x && rect1.bottomRight.x >= rect2.topLeft.x &&
    rect1.topLeft.y <= rect2.bottomRight.y && rect1.bottomRight.y >= rect2.topLeft.y);
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

float PongRayTrace(D3DXVECTOR2 pos, D3DXVECTOR2 dir, float fLeftBound)
{
	// Bounds
	RECT R;
	::GetWindowRect(::GetActiveWindow(),&R);

	const int Y = R.bottom - R.top;
	const int X = R.right - R.left;

	float b = 0.0f;
	float m = 0.0f;
	float x = 0.0f;

	// Loop while pos is within the window
	while(InRange(pos.x,fLeftBound,(float)X))
	{
		float c = 0.0f;
		float n = 1.0f; // normal vector

		// If the object is heading down
		if(dir.y > 0.0f)
		{
			// Set the constant to solve for
			c = (float)Y;

			// set the normal vector
			n = -n;
		}

		// calculate the slope
		m = dir.y / dir.x;

		// calculate y-intercept
		b = -m*pos.x + pos.y;

		// calculate where the object will hit
		x = ((c - b)) / m;

		// update pos
		pos = D3DXVECTOR2(x,m*x + b);

		// reflect dir
		dir = ::Reflect(-dir,D3DXVECTOR2(0.0f,n));
	}

	// y = mx + b

	// when the object out of the bound
	// we need to return the y pos when x = 50, 
	// just plug 50 into the equation above and return the result

	return m*fLeftBound + b;
	//return b;
}

float GetRandFloat(float a, float b)
{
	// todo: need to check this
	float fRand = (rand() % 10001) * 0.0001f;
	return fRand*(b - a) + a;
}

bool InRange(float value, float min, float max)
{
	return ((value >= min) && (value <= max));
}

bool Equals(float a, float b, float diff)
{
	return fabsf(a - b) < diff;
}

void RegisterScriptVecMath(asIScriptEngine* pEngine)
{
	//D3DXVECTOR2

	// todo finish registering the vector interface to script
	DBAS(pEngine->RegisterObjectType("Vector2",sizeof(D3DXVECTOR2),asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS));
	DBAS(pEngine->RegisterObjectProperty("Vector2","float x",offsetof(D3DXVECTOR2,x)));
	DBAS(pEngine->RegisterObjectProperty("Vector2","float y",offsetof(D3DXVECTOR2,x)));

	DBAS(pEngine->RegisterGlobalFunction("uint log2(uint)",asFUNCTION(LOG2),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("bool InRange(float,float,float)",asFUNCTION(InRange),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("float rand(float,float)",asFUNCTION(GetRandFloat),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("float clamp(float,float,float)",asFUNCTION(Clamp<float>),asCALL_CDECL));
}

unsigned int LOG2(unsigned int v)
{
	const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
	const unsigned int S[] = {1, 2, 4, 8, 16};
	
	unsigned int r = 0; // result of log2(v) will go here
	for (int i = 4; i >= 0; i--) // unroll for speed...
	{
		if (v & b[i])
		{
			v >>= S[i];
			r |= S[i];
		} 
	}

	return r;
}
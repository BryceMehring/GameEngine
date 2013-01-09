// Programmed by Bryce Mehring
#include "VecMath.h"
#include "asVM.h"
#include "FileManager.h"

#pragma comment(lib,"d3dx9.lib")

namespace Math
{

// ----- CCircle -----

CCircle::CCircle(const Circle& circle) : m_circle(circle)
{
}

bool CCircle::Intersects(const ICollisionPolygon& other) const
{
	// exclude collision with the same object 
	if(&other == this) { return true; }

	// Get the type of the other object
	ICollisionPolygon::Type polyType = other.GetType();
	bool bSuccess = false;

	switch(polyType)
	{
		case ICollisionPolygon::CircleType:
		{
			const CCircle& circle = static_cast<const CCircle&>(other);
			bSuccess = Math::Intersects(m_circle,circle.m_circle);
			break;
		}

		case ICollisionPolygon::RectangleType:
		{
			const CRectangle& rectangle = static_cast<const CRectangle&>(other);
			bSuccess = Math::Intersects(m_circle,rectangle.GetRect());
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

CRectangle::CRectangle(const FRECT& rect) : m_rect(rect), m_polygon(rect)
{
}

void CRectangle::Render(::IRenderer& renderer)
{
	this->m_polygon.Render(renderer);
}

bool CRectangle::Intersects(const ICollisionPolygon& other) const
{
	if(&other == this) { return true; }

	ICollisionPolygon::Type polyType = other.GetType();
	bool bSuccess = false;

	switch(polyType)
	{
		case ICollisionPolygon::CircleType:
		{
			const CCircle& circle = static_cast<const CCircle&>(other);
			bSuccess = Math::Intersects(circle.GetCircle(),m_rect);
			break;
		}

		case ICollisionPolygon::RectangleType:
		{
			const CRectangle& rectangle = static_cast<const CRectangle&>(other);
			bSuccess = Math::Intersects(m_rect,rectangle.m_rect);
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
		if(pos.x <= m_rect.bottomRight.x)
		{
			middle.x += 30;
		}
		else if(pos.x >= m_rect.topLeft.x)
		{
			middle.x -= 30;
		}
		else
		{
			middle.y = 0;
		}
		out = middle - pos;
		D3DXVec2Normalize(&out,&out);
	}
	
}

ICollisionPolygon* CreateCollionPolygon(const std::vector<D3DXVECTOR2>& poly)
{
	D3DXVECTOR2 topLeft(FLT_MAX,FLT_MAX);
	D3DXVECTOR2 bottomRight(-FLT_MAX,-FLT_MAX);

	for(auto iter = poly.begin(); iter != poly.end(); ++iter)
	{
		if(iter->x < topLeft.x)
		{
			topLeft.x = iter->x;
		}
		else if(iter->x > bottomRight.x)
		{
			bottomRight.x = iter->x;
		}

		if(iter->y < topLeft.y)
		{
			topLeft.y = iter->y;
		}
		else if(iter->y > bottomRight.y)
		{
			bottomRight.y = iter->y;
		}
	}

	return new CRectangle(Math::FRECT(topLeft,bottomRight));
}


D3DXVECTOR2 Reflect(const D3DXVECTOR2& dir, const D3DXVECTOR2& normal)
{
	return 2*normal*D3DXVec2Dot(&dir,&normal) - dir;
}

D3DXVECTOR3 Reflect(const D3DXVECTOR3& dir, const D3DXVECTOR3& normal)
{
	return 2*normal*D3DXVec3Dot(&dir,&normal) - dir;
}

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
	float closestY = Clamp(c1.center.y, R1.bottomRight.y, R1.topLeft.y);

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
	float fSum = c1.r + c2.r;
	return D3DXVec2LengthSq(&(c1.center - c2.center)) < (fSum * fSum);
}

bool Intersects(const FRECT& rect1, const FRECT& rect2)
{
	return (rect1.topLeft.x <= rect2.bottomRight.x && rect1.bottomRight.x >= rect2.topLeft.x &&
    rect1.topLeft.y >= rect2.bottomRight.y && rect1.bottomRight.y <= rect2.topLeft.y);
}

bool IsPointInPolygon(const D3DXVECTOR2* pArray, unsigned int length, POINT P)
{
	int j = length - 1;
	bool oddNodes = false;

	for(unsigned int i=0; i < length; i++)
	{
		if ((pArray[i].y< P.y && pArray[j].y>= P.y || pArray[j].y< P.y && pArray[i].y>=P.y) &&  (pArray[i].x<=P.x || pArray[j].x<=P.x))
		{
			oddNodes^=(pArray[i].x+(P.y-pArray[i].y)/(pArray[j].y-pArray[i].y)*(pArray[j].x-pArray[i].x)<P.x);
		}
		j=i;
	}

	return oddNodes; 
}

bool Sat(const std::vector<D3DXVECTOR3>& poly1, const std::vector<D3DXVECTOR3>& poly2)
{
    bool ret = true;

    //For every face in c1
    for(int i = 0; i < poly1.size() - 1; i++)
    {
        //Grab a face (face x, face y)
        float fx = poly1[i].x - poly1[(i + 1) % (poly1.size() - 1)].x;
        float fy = poly1[i].y - poly1[(i + 1) % (poly1.size() - 1)].y;

        //Create a perpendicular axis to project on (axis x, axis y)
        float ax = -fy, ay = fx;

        //Normalize the axis
        float len_v = sqrt(ax * ax + ay * ay);
        ax /= len_v;
        ay /= len_v;

        //Carve out the min and max values
        float c1_min = FLT_MAX, c1_max = -FLT_MAX;
        float c2_min = FLT_MAX, c2_max = -FLT_MAX;

        //Project every point in c1 on the axis and store min and max
        for(int j = 0; j < poly1.size() - 1; j++)
        {
            float c1_proj = (ax * (poly1[j].x) + ay * (poly1[j].y)) / (ax * ax + ay * ay);
            c1_min = min(c1_proj, c1_min);
            c1_max = max(c1_proj, c1_max);
        }

        //Project every point in c2 on the axis and store min and max
        for(int j = 0; j < poly2.size() - 1; j++)
        {
            float c2_proj = (ax * (poly2[j].x) + ay * (poly2[j].y)) / (ax * ax + ay * ay);
            c2_min = min(c2_proj, c2_min);
            c2_max = max(c2_proj, c2_max);
        }

        //Return if the projections do not overlap
        if(!(c1_max >= c2_min && c1_min <= c2_max))
            ret = false;
    }
    return ret;
}

bool IsPointInPolygon(const D3DXVECTOR3* pArray, unsigned int length, POINT P)
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
	float b = 0.0f;
	float m = 0.0f;
	float x = 0.0f;

	// Loop while pos is within the window
	while(pos.x > fLeftBound)
	{
		float c = 50.0f;
		float n = -1.0f; // normal vector

		// If the object is heading down
		if(dir.y < 0.0f)
		{
			// Set the constant to solve for
			c = -50.0f;

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
		dir = Math::Reflect(-dir,D3DXVECTOR2(0.0f,n));
	}

	// y = mx + b

	// when the object out of the bound
	// we need to return the y pos when x = 50, 
	// just plug 50 into the equation above and return the result

	return m*fLeftBound + b;
	//return b;
}

unsigned int xor128()
{
	static unsigned int x = 123456789;
	static unsigned int y = 362436069;
	static unsigned int z = 521288629;
	static unsigned int w = 88675123;
	unsigned int t;
 
	t = x ^ (x << 11);
	x = y; y = z; z = w;
	return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

float GetRandFloat(float a, float b)
{
	// todo: need to check this
	float fRand = rand() / (RAND_MAX + 1.0f);
	return fRand*(b - a) + a;
}

unsigned int GetRandInt(unsigned int a, unsigned int b)
{
	return a + (rand() % b);
}

bool InRange(float value, float min, float max)
{
	return ((value >= min) && (value <= max));
}

bool Equals(float a, float b, float diff)
{
	return fabsf(a - b) < diff;
}

// todo: write this as a static comp.
bool IsPrime(unsigned int n)
{
	// 0 - not
	// 1 - not
	//- 2 - is
	// 3 - is

	if(((n % 2 == 0) && (n != 2)) || n < 2)
	{
		return false;
	}
	else if( n < 4)
	{
		return true;
	}

	unsigned int i = 3;
	bool success = true;
	while((success = ((n % i) != 0)) && ((i * i) < n))
	{
		i += 2;
	}

	return success;
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

unsigned int LOG10(unsigned int i)
{
	static const unsigned int PowersOf10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

	int t = (LOG2(i) + 1) * 1233 >> 12; // temp
	int r = t - (i < PowersOf10[t]);
	return r;
}

unsigned int NumDigits(unsigned int uiNumber)
{
	if(uiNumber == 0)
		return 1;

	return LOG10(uiNumber) + 1;
}

// uiNumber: base 10, uiBase: base of the output 
/*unsigned int NumDigits(unsigned int uiNumber, unsigned int uiBase)
{
	return ((unsigned int)(log((float)uiNumber) / log((float)uiBase))) + 1;
}*/

unsigned int NumDigits(unsigned int uiNumber, unsigned int uiBase)
{
	if(uiNumber == 0)
		return 1;

	unsigned int uiCount = 0;
	while(uiNumber != 0)
	{
		uiNumber /= uiBase;
		uiCount++;
	}

	return uiCount;
}

std::string ConvertTo(unsigned int uiInputNumber, unsigned int uiTargetBase)
{
	std::string targetNumber;

	if(uiTargetBase > 1)
	{
		// num of characters in target base
		const unsigned int uiTotalDigits = NumDigits(uiInputNumber,uiTargetBase);
		unsigned int i = uiTotalDigits - 1;

		// resize string to final size
		targetNumber.resize(uiTotalDigits);

		do
		{
			unsigned int uiNewDigit = (uiInputNumber % uiTargetBase);

			if(uiNewDigit >= 10)
			{
				char hexChar = (uiNewDigit - 10) + 'A';
				targetNumber[i] = hexChar;
			}
			else
			{
				targetNumber[i] = '0' + uiNewDigit;
			}

			uiInputNumber /= uiTargetBase;
			i--;
		} while(uiInputNumber > 0);
	}
	
	return targetNumber;
}

void RegisterScriptVecMath(::asIScriptEngine* pEngine)
{
	//D3DXVECTOR2

	// todo: finish registering the vector interface to script
	DBAS(pEngine->RegisterObjectType("vector2",sizeof(D3DXVECTOR2),asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS));
	DBAS(pEngine->RegisterObjectProperty("vector2","float x",offsetof(D3DXVECTOR2,x)));
	DBAS(pEngine->RegisterObjectProperty("vector2","float y",offsetof(D3DXVECTOR2,x)));

	DBAS(pEngine->RegisterGlobalFunction("bool IsPrime(uint)",asFUNCTION(IsPrime),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("string ConvertTo(uint,uint)",asFUNCTION(ConvertTo),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("uint NumDigits(uint)",asFUNCTIONPR(NumDigits,(unsigned int),unsigned int),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("uint NumDigits(uint,uint)",asFUNCTIONPR(NumDigits,(unsigned int,unsigned int),unsigned int),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("uint log2(uint)",asFUNCTION(LOG2),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("bool InRange(float,float,float)",asFUNCTION(InRange),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("float rand(float,float)",asFUNCTION(GetRandFloat),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("uint rand(uint,uint)",asFUNCTION(GetRandInt),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("float clamp(float,float,float)",asFUNCTION(Clamp<float>),asCALL_CDECL));
}

} // Math
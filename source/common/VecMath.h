// Programmed by Bryce Mehring
// todo: need to insert this into every file...

#ifndef _VECMATH_
#define _VECMATH_

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>
#include "CommonExport.h"

namespace Math
{

struct AABB
{
	glm::vec2 min;
	glm::vec2 max;
};

class IPolygon
{
public:
	virtual ~IPolygon() {}
	virtual bool IsPointWithin(const glm::vec2& pos) const = 0;
};

// Basic rectangle using floats
struct FRECT : public IPolygon
{

	COMMON_API FRECT() {}

	COMMON_API FRECT(const glm::vec2& topLeft) : topLeft(topLeft) {}

	COMMON_API FRECT(const glm::vec2& topLeft, const glm::vec2& bottomRight) : topLeft(topLeft), bottomRight(bottomRight) {}

	COMMON_API FRECT(float width, float height, const glm::vec2& center) :
		topLeft(center.x - width / 2.0f,center.y + height / 2.0f),
		bottomRight(center.x + width / 2.0f,center.y - height / 2.0f) {}

	COMMON_API glm::vec2 Middle() const { return (topLeft + bottomRight) * 0.5f; }

	COMMON_API glm::mat4 Transformation() const
	{
		glm::mat4 T = glm::translate(glm::vec3(Middle(), 0.0f));
		T = glm::scale(T, glm::vec3(Width(), Height(), 1.0f));

		return T;
	}

	COMMON_API virtual bool IsPointWithin(const glm::vec2& pos) const
	{
		return (pos.x >= topLeft.x) && (pos.x <= bottomRight.x) && (pos.y >= bottomRight.y) && (pos.y <= topLeft.y);
	}

	COMMON_API float Width() const
	{
		return (bottomRight.x - topLeft.x);
	}

	COMMON_API float Height() const
	{
		return (topLeft.y - bottomRight.y);
	}

	COMMON_API void SetCenter(const glm::vec2& pos)
	{
		glm::vec2 diff = (bottomRight - topLeft)*0.5f;
		topLeft = pos - diff;
		bottomRight = pos + diff;
	}

	// todo: rename to leftTop, rightBottom;
	glm::vec2 topLeft;
	glm::vec2 bottomRight;
};

// Basic circle structure
struct Circle : public IPolygon
{
	COMMON_API Circle() {}
	COMMON_API Circle(const glm::vec2& c, float r) : center(c), r(r)
	{
	}

	virtual bool IsPointWithin(const glm::vec2& pos) const
	{
		glm::vec2 temp = pos - center;
		return (temp.x * temp.x + temp.y * temp.y) < (r*r);
	}

	void SetCenter(const glm::vec2& pos)
	{
		center = pos;
	}

	glm::vec2 center;
	float r;

};

COMMON_API bool Intersects(const std::vector<glm::vec3>& poly1, const std::vector<glm::vec3>& poly2); 
COMMON_API bool Intersects(const Circle& c1, const FRECT& R1);
COMMON_API bool Intersects(const Circle& c1, const Circle& c2);
COMMON_API bool Intersects(const FRECT& c1, const FRECT& c2);

COMMON_API bool Sat(const std::vector<glm::vec3>& poly1, const std::vector<glm::vec3>& poly2);

// ray casting algorithm
COMMON_API bool IsPointInPolygon(const glm::vec2* pArray, unsigned int length, const glm::vec2& P);

// collision interface

class ICollisionPolygon
{
public:

	enum class Type
	{
		Circle,
		Rectangle,
	};

	virtual ~ICollisionPolygon() {}

	// returns true if pOther interests this, else false
	virtual bool Intersects(const ICollisionPolygon& other) const = 0;
	virtual void GetNormal(const glm::vec2& pos, glm::vec2& out) const = 0;
	virtual void GetAABB(AABB&) const = 0;

	// returns the type of the collision object
	virtual Type GetType() const = 0;

};

// collision circle

class CCircle : public ICollisionPolygon
{
public:

	// todo: default ctor?
	// ctor
	COMMON_API CCircle() {}
	COMMON_API CCircle(const Circle& circle);

	COMMON_API virtual Type GetType() const { return ICollisionPolygon::Type::Circle; }
	COMMON_API virtual bool Intersects(const ICollisionPolygon& other) const;
	COMMON_API virtual void GetNormal(const glm::vec2& pos, glm::vec2& out) const;
	COMMON_API virtual void GetAABB(AABB&) const;
	
	// circle access
	COMMON_API const Circle& GetCircle() const { return m_circle; }
	COMMON_API Circle& GetCircle() { return m_circle; }

private:

	Circle m_circle;

};

// todo: create another CRectangle class that is renderable
// collision rectangle
class CRectangle : public ICollisionPolygon
{
public:

	// ctror
	COMMON_API CRectangle() {}
	COMMON_API CRectangle(const FRECT& rect);

	COMMON_API virtual Type GetType() const { return ICollisionPolygon::Type::Rectangle; }
	COMMON_API virtual bool Intersects(const ICollisionPolygon& other) const;
	COMMON_API virtual void GetNormal(const glm::vec2& pos, glm::vec2& out) const;
	COMMON_API virtual void GetAABB(AABB&) const;

	// rect access
	COMMON_API const FRECT& GetRect() const { return m_rect; }
	COMMON_API FRECT& GetRect() { return m_rect; }

private:

	FRECT m_rect;

};

COMMON_API void CreateCollionPolygon(const std::vector<glm::vec3>& poly, FRECT& out);

} // math

#endif // _VECMATH_

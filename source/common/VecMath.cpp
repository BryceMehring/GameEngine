// Programmed by Bryce Mehring
#include "VecMath.h"

#include <limits>
#include <glm/geometric.hpp>

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
		case ICollisionPolygon::Type::Circle:
		{
			const CCircle& circle = static_cast<const CCircle&>(other);
			bSuccess = Math::Intersects(m_circle,circle.m_circle);
			break;
		}

		case ICollisionPolygon::Type::Rectangle:
		{
			const CRectangle& rectangle = static_cast<const CRectangle&>(other);
			bSuccess = Math::Intersects(m_circle,rectangle.GetRect());
			break;
		}
		default:
			assert("Unknown polygon type selected" && false);
	}

	return bSuccess;
}

AABB CCircle::GetAABB() const
{
	return {
		glm::vec2(m_circle.center.x - m_circle.r,m_circle.center.y - m_circle.r),
		glm::vec2(m_circle.center.x + m_circle.r,m_circle.center.y + m_circle.r)
	};
}

// ----- CCircle -----

CRectangle::CRectangle(const FRECT& rect) : m_rect(rect)
{
}


bool CRectangle::Intersects(const ICollisionPolygon& other) const
{
	if(&other == this) { return true; }

	ICollisionPolygon::Type polyType = other.GetType();
	bool bSuccess = false;

	switch(polyType)
	{
		case ICollisionPolygon::Type::Circle:
		{
			const CCircle& circle = static_cast<const CCircle&>(other);
			bSuccess = Math::Intersects(circle.GetCircle(),m_rect);
			break;
		}

		case ICollisionPolygon::Type::Rectangle:
		{
			const CRectangle& rectangle = static_cast<const CRectangle&>(other);
			bSuccess = Math::Intersects(m_rect,rectangle.m_rect);
			break;
		}
	}

	return bSuccess;
}

AABB CRectangle::GetAABB() const
{
	return {
		glm::vec2(m_rect.topLeft.x,m_rect.bottomRight.y),
		glm::vec2(m_rect.bottomRight.x,m_rect.topLeft.y)
	};
}

void CreateCollionPolygon(const std::vector<glm::vec3>& poly, FRECT& out)
{
	glm::vec2 minPoint(std::numeric_limits<float>::max(),std::numeric_limits<float>::max());
	glm::vec2 maxPoint(std::numeric_limits<float>::min(),std::numeric_limits<float>::min());

	for(auto iter = poly.begin(); iter != poly.end(); ++iter)
	{
		if(iter->x < minPoint.x)
		{
			minPoint.x = iter->x;
		}
		else if(iter->x > maxPoint.x)
		{
			maxPoint.x = iter->x;
		}

		if(iter->y < minPoint.y)
		{
			minPoint.y = iter->y;
		}
		else if(iter->y > maxPoint.y)
		{
			maxPoint.y = iter->y;
		}
	}

	out.topLeft = glm::vec2(minPoint.x,maxPoint.y);
	out.bottomRight = glm::vec2(maxPoint.x,minPoint.y);
}

bool Intersects(const std::vector<glm::vec3>& poly1, const std::vector<glm::vec3>& poly2)
{
	return Sat(poly1,poly2) && Sat(poly2,poly1);
}

bool Intersects(const Circle& c1, const FRECT& R1)
{
	glm::vec2 closest = glm::vec2(glm::clamp(c1.center.x, R1.topLeft.x, R1.bottomRight.x),glm::clamp(c1.center.y, R1.bottomRight.y, R1.topLeft.y));
	glm::vec2 distance = c1.center - closest;

	// If the distance is less than the circle's radius, an intersection occurs
	return (distance.x * distance.x + distance.y * distance.y) < (c1.r * c1.r);
}

//Returns true if the circles are touching, or false if they are not
bool Intersects(const Circle& c1, const Circle& c2)
{
	float fSum = c1.r + c2.r;
	glm::vec2 diff = c1.center - c2.center;

	return (diff.x * diff.x + diff.y * diff.y) < (fSum * fSum);
}

bool Intersects(const FRECT& rect1, const FRECT& rect2)
{
	return (rect1.topLeft.x <= rect2.bottomRight.x && rect1.bottomRight.x >= rect2.topLeft.x &&
			rect1.topLeft.y >= rect2.bottomRight.y && rect1.bottomRight.y <= rect2.topLeft.y);
}

bool IsPointInPolygon(const glm::vec2* pArray, unsigned int length, const glm::vec2& P)
{
	int j = length - 1;
	bool oddNodes = false;

	for(unsigned int i=0; i < length; i++)
	{
		if (((pArray[i].y< P.y && pArray[j].y>= P.y) || (pArray[j].y< P.y && pArray[i].y>=P.y)) &&  (pArray[i].x<=P.x || pArray[j].x<=P.x))
		{
			oddNodes^=(pArray[i].x+(P.y-pArray[i].y)/(pArray[j].y-pArray[i].y)*(pArray[j].x-pArray[i].x)<P.x);
		}
		j=i;
	}

	return oddNodes;
}

bool Sat(const std::vector<glm::vec3>& poly1, const std::vector<glm::vec3>& poly2)
{
	bool ret = true;

	//For every face in c1
	for(unsigned int i = 0; i < poly1.size() - 1; i++)
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
		float c1_min = std::numeric_limits<float>::max(), c1_max = std::numeric_limits<float>::min();
		float c2_min = std::numeric_limits<float>::max(), c2_max = std::numeric_limits<float>::min();

		//Project every point in c1 on the axis and store min and max
		for(unsigned int j = 0; j < poly1.size() - 1; j++)
		{
			float c1_proj = (ax * (poly1[j].x) + ay * (poly1[j].y)) / (ax * ax + ay * ay);
			c1_min = glm::min(c1_proj, c1_min);
			c1_max = glm::max(c1_proj, c1_max);
		}

		//Project every point in c2 on the axis and store min and max
		for(unsigned int j = 0; j < poly2.size() - 1; j++)
		{
			float c2_proj = (ax * (poly2[j].x) + ay * (poly2[j].y)) / (ax * ax + ay * ay);
			c2_min = glm::min(c2_proj, c2_min);
			c2_max = glm::max(c2_proj, c2_max);
		}

		//Return if the projections do not overlap
		if(!(c1_max >= c2_min && c1_min <= c2_max))
			ret = false;
	}
	return ret;
}


} // Math

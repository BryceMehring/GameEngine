#ifndef _ISpatialObject_
#define _ISpatialObject_

#include <glm/vec3.hpp>

class ISpatialObject : public IRender
{
public:

	virtual ~ISpatialObject() {}

	virtual const glm::vec3& GetPos() const = 0;
	virtual void* QueryInterface(unsigned int) const = 0;

	virtual const class Math::ICollisionPolygon& GetCollisionPolygon() const = 0;
};

#endif // _ISpatialObject_

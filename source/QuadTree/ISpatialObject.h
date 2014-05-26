#ifndef _ISpatialObject_
#define _ISpatialObject_

#include <glm/glm.hpp>

class ISpatialObject : public IRender
{
public:

	virtual ~ISpatialObject() {}

	virtual const glm::vec3& GetPos() const = 0;
	virtual glm::vec2 GetDir() const = 0;
	virtual float GetSpeed() const = 0;

	virtual void* QueryInterface(unsigned int) const = 0;

	virtual const class Math::ICollisionPolygon& GetCollisionPolygon() const = 0;
};

#endif // _ISpatialObject_

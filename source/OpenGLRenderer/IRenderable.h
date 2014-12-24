#ifndef _IRENDERABLE_
#define _IRENDERABLE_

class IRenderable
{
public:

	virtual ~IRenderable() {}
	virtual void Render(class ApplyShader& shader, const class IResource* resource) = 0;
};

#endif // _IRENDERABLE_

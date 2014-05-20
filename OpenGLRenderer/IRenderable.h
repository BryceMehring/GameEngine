#ifndef _IRENDERABLE_
#define _IRENDERABLE_

class IRenderable
{
public:

	virtual ~IRenderable() {}
	virtual void Setup(class ApplyTexturedShader& shader, const class IResource& resource) = 0;
	virtual void Render(class ApplyTexturedShader& shader, const class IResource& resource) = 0;
};

#endif // _IRENDERABLE_

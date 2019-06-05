#ifndef _IRENDER_
#define _IRENDER_

// todo: this class needs to be renamed to IRenderable
class IRender
{
public:

	virtual ~IRender() {}

	virtual void Render(class IRenderer& renderer) = 0;
};

#endif // _IRENDER_

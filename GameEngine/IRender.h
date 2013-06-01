#ifndef _IRENDER_
#define _IRENDER_

// todo: this class needs to be renamed to IRenderable
class IRender
{
public:

	enum IRenderType
	{
		//Text,
		Picture,
		Mesh,
		Line,
		Polygon,
		// todo:add more here
	};

	virtual ~IRender() {}

	//virtual IRenderType GetRenderType() const = 0;
	virtual void Render(class IRenderer& renderer) = 0;
};

#endif // _IRENDER_

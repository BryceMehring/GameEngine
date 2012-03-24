#ifndef _IRENDER_
#define _IRENDER_

class __declspec(novtable) IRender
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
	//virtual void Begin() = 0;
	virtual void Render(class IRenderer* pRenderer) = 0;
	//virtual void End() = 0;

};

#endif // _IRENDER_
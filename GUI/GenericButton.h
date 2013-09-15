#ifndef _GENERICBUTTON_
#define _GENERICBUTTON_

#include "ButtonBase.h"
#include "Delegates.h"
#include "IKMInput.h"

namespace UI
{

template< class T >
class GenericButton : public ButtonBase
{
public:

	typedef ::Delegate<void,T> DELEGATE;

	GenericButton(const Math::FRECT& s, const glm::vec3& defaultColor,
		const glm::vec3& hoverColor, const glm::vec2& textScale, const std::string& str) : ButtonBase(s,defaultColor,hoverColor,textScale,str) {}
	GenericButton(const Math::FRECT& s, const glm::vec3& defaultColor,
		const glm::vec3& hoverColor, const glm::vec2& textScale, const std::string& str, const DELEGATE& callback) : ButtonBase(s,defaultColor,hoverColor, textScale,str), m_callback(callback) {}

	void SetCallback(const DELEGATE& callback)
	{
		m_callback = callback;
	}
	void SetArg(const T& arg)
	{
		m_type = arg;
	}
	
	virtual void Trigger()
	{
		m_callback.Call(m_type);
	}
	virtual void Update(::IKMInput& input, double)
	{
		ButtonBase::Update(input);

		if(input.MouseClick(0))
		{
			const glm::vec2& pos = input.GetTransformedMousePos();

			if(m_sprite.IsPointWithin(pos))
			{
				Trigger();
			}
		}
	}

private:

	virtual ~GenericButton() {}

	DELEGATE m_callback;
	T m_type;
};

template< class T >
class GenericButton<T&> : public ButtonBase
{
public:

	typedef ::Delegate<void,T&> DELEGATE;

	GenericButton(const Math::FRECT& s, const glm::vec3& defaultColor,
		const glm::vec3& hoverColor, const glm::vec2& textScale, const std::string& str) : ButtonBase(s,defaultColor,hoverColor,textScale,str) {}
	GenericButton(const Math::FRECT& s, const glm::vec3& defaultColor,
		const glm::vec3& hoverColor, const glm::vec2& textScale, const std::string& str, const DELEGATE& callback) : ButtonBase(s,defaultColor,hoverColor, textScale,str), m_callback(callback) {}

	void SetCallback(const DELEGATE& callback)
	{
		m_callback = callback;
	}
	void SetArg(const T& arg)
	{
		m_type = arg;
	}

	virtual void Trigger()
	{
		m_callback.Call(m_type);
	}
	virtual void Update(::IKMInput& input, double)
	{
		ButtonBase::Update(input);

		if(input.MouseRelease(0))
		{
			const glm::vec2& pos = input.GetTransformedMousePos();

			if(m_sprite.IsPointWithin(pos))
			{
				Trigger();
			}
		}
	}

private:

	virtual ~GenericButton() {}

	DELEGATE m_callback;
	T m_type;
};

template<>
class GenericButton<void> : public ButtonBase
{
public:

	typedef ::Delegate<void,void> DELEGATE;

	GenericButton(const Math::FRECT& s, const glm::vec3& defaultColor,
		const glm::vec3& hoverColor, const glm::vec2& textScale, const std::string& str) : ButtonBase(s,defaultColor,hoverColor,textScale,str) {}
	GenericButton(const Math::FRECT& s, const glm::vec3& defaultColor,
		const glm::vec3& hoverColor, const glm::vec2& textScale, const std::string& str, const DELEGATE& callback) : ButtonBase(s,defaultColor,hoverColor, textScale,str), m_callback(callback) {}

	void SetCallback(const DELEGATE& callback)
	{
		m_callback = callback;
	}

	virtual void Trigger()
	{
		m_callback.Call();
	}
	virtual void Update(::IKMInput& input, double)
	{
		ButtonBase::Update(input);

		if(input.MouseClick(0))
		{
			const glm::vec2& pos = input.GetTransformedMousePos();

			if(m_sprite.IsPointWithin(pos))
			{
				Trigger();
			}
		}
	}

private:

	virtual ~GenericButton() {}

	DELEGATE m_callback;
};


}

#endif // _GENERICBUTTON_
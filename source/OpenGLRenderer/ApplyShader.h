#ifndef _APPLYSHADER_
#define _APPLYSHADER_

class Shader;
class TexturedShader;

// Defines a simple wrapper around a Shader to bind and unbind the shader automatically within the current scope
class ApplyShader
{
public:

	// Binds pShader
	ApplyShader(Shader* pShader);
	ApplyShader(ApplyShader&& shader);

	ApplyShader(const ApplyShader&) = delete;
	ApplyShader& operator =(const ApplyShader&) = delete;

	// Unbinds the shader
	~ApplyShader();

	// Access to the shader object
	Shader* operator*() const;
	Shader* operator->() const;

protected:

	Shader* m_pShader;
};

class ApplyTexturedShader : public ApplyShader
{
public:

	ApplyTexturedShader(TexturedShader* pShader);

	// Access to the shader object
	TexturedShader* operator*() const;
	TexturedShader* operator->() const;

};

#endif // _APPLYSHADER_

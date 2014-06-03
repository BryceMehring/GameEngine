#include "ApplyShader.h"
#include "ResourceManager.h"

ApplyShader::ApplyShader(Shader *pShader) : m_pShader(pShader)
{
	assert(pShader != nullptr);
	m_pShader->Bind();
}

ApplyShader::ApplyShader(ApplyShader&& shader) : m_pShader(shader.m_pShader)
{
	shader.m_pShader = nullptr;
}

ApplyShader::~ApplyShader()
{
	if(m_pShader != nullptr)
	{
		m_pShader->UnBind();
	}
}

Shader* ApplyShader::operator *() const
{
	return m_pShader;
}

Shader* ApplyShader::operator ->() const
{
	return m_pShader;
}

ApplyShader::ShaderType ApplyShader::GetType() const
{
	if (m_pShader->QueryInterface(ResourceType::TexturedShader) != nullptr)
	{
		return ShaderType::Textured;
	}

	return ShaderType::Normal;
}

ApplyTexturedShader::ApplyTexturedShader(TexturedShader* pShader) : ApplyShader(pShader)
{
}

TexturedShader* ApplyTexturedShader::operator *() const
{
	return static_cast<TexturedShader*>(m_pShader);
}

TexturedShader* ApplyTexturedShader::operator ->() const
{
	return static_cast<TexturedShader*>(m_pShader);
}

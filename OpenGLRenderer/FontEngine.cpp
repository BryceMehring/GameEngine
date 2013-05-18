#include "FontEngine.h"
#include "GenerateBuffers.h"
#include <glm/gtx/transform.hpp>

FontEngine::FontEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam) :
m_pRm(pRm), m_iMaxLength(maxLength), m_pCamera(pCam)
{
	CreateIndexBuffer();
	CreateVertexBuffer();
}

FontEngine::~FontEngine()
{
	glDeleteBuffers(1,&m_uiIndexBuffer);
	glDeleteBuffers(1,&m_uiVertexBuffer);
}

void FontEngine::DrawString(const char* str, const char* font, const glm::vec2& pos, const glm::vec2& scale, const glm::vec4& color)
{
	if(str == nullptr)
		return;

	if(font == nullptr)
		font = "font";

	m_textSubsets.push_back(DrawTextInfo(str,font,pos,glm::vec2(5.0f,5.0f)*scale,color));
}

void FontEngine::CreateIndexBuffer()
{
    m_uiIndexBuffer = CreateQuadIndexBuffer(m_iMaxLength);
}

void FontEngine::CreateVertexBuffer()
{
	glGenBuffers(1,&m_uiVertexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, m_uiVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_iMaxLength * sizeof(FontVertex), 0, GL_DYNAMIC_DRAW);
	
}

glm::vec2 FontEngine::GetTextOffset(const glm::vec2& scale, unsigned int i, unsigned int j) const
{
	return glm::vec2(scale.x * i/2.0f,-scale.y * j);
}

void FontEngine::FillVertexBuffer()
{
	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);

	//glMapBufferRange(
	FontVertex* v = (FontVertex*)glMapBufferRange(GL_ARRAY_BUFFER , 0, m_iMaxLength * sizeof(FontVertex), GL_MAP_WRITE_BIT);

	unsigned int iCurrentVerts = 0;

	for(auto iter = m_textSubsets.begin(); iter != m_textSubsets.end(); ++iter)
	{
		const char* str = iter->text.c_str();

		unsigned int iVert = iCurrentVerts; // current vertex
		const Charset& font = static_cast<const Charset&>(m_pRm->GetResource(iter->font)); // todo: need to check if the resource is actually a font

		glm::vec3 posW(iter->pos,0.0f);
		
		// Loop over entire string or until we have filled the buffer
		while((iVert < m_iMaxLength) && *str)
		{
			char character = *str++;

			const CharDescriptor& charToRender = font.Chars[character];

			float fAdvance = (iter->scale.x * charToRender.XAdvance / (float)font.iWidth) / 2;

			if((character != '\n') && (character != '\t') && (character != ' '))
			{
				// cull characters that are off the screen
				if(posW.x >= -100.0f && posW.x <= 100.0f && posW.y <= 100.0f && posW.y >= -100.0f)
				{
					int index = iVert * 4;

					//float fAdvance = (iter->scale.x * charToRender.XAdvance / (float)font.iWidth) / 2;

					// tex coords
					glm::vec2 topLeft((charToRender.x / (float)font.iWidth),charToRender.y / (float)font.iHeight);
					glm::vec2 bottomRight(((charToRender.x+charToRender.Width) / (float)font.iWidth),(charToRender.y+charToRender.Height) / (float)font.iHeight);

					v[index].pos = glm::vec3(-fAdvance * iter->scale.x,0.5f * iter->scale.y,0.0f) + posW;
					v[index].tex = topLeft;
					//v[index].tex = glm::vec2(0.0,0.0f);

					v[index + 1].pos = glm::vec3(-fAdvance * iter->scale.x,-0.5f * iter->scale.y,0.0f) + posW;
					v[index + 1].tex = glm::vec2(topLeft.x,bottomRight.y);
					//v[index + 1].tex = glm::vec2(0.0,1.0f);

					v[index + 2].pos = glm::vec3(fAdvance * iter->scale.x,0.5f * iter->scale.y,0.0f) + posW;
					v[index + 2].tex = glm::vec2(bottomRight.x,topLeft.y);
					//v[index + 2].tex = glm::vec2(1.0,0.0f);

					v[index + 3].pos = glm::vec3(fAdvance * iter->scale.x,-0.5f * iter->scale.y,0.0f) + posW;
					v[index + 3].tex = bottomRight;
					//v[index + 3].tex = glm::vec2(1.0,1.0f);

					++iVert;
				}

				//

				 
				//posW.x += 20.0f * iter->scale.x * ((charToRender.XAdvance) / (float)font.iWidth); // (charToRender.x+charToRender.Width) / (float)font.iWidth
			}
			else if(character == '\n')
			{
				//i = -1;
				//++j;
			}
			else if(character == '\t')
			{
				//i += 10;
			}

			posW.x += fAdvance * iter->scale.x * 2.0f + 1.0f; // * 10 / 2
		}

		iter->length = iVert - iCurrentVerts;
		iCurrentVerts += iter->length;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void FontEngine::Render()
{
    // if there is nothing to draw, do nothing
    if(m_textSubsets.empty())
        return;

	// first we fill the vertex buffer with the text to render
	FillVertexBuffer();

	// get the shader to use
    Shader& theShader = static_cast<Shader&>(m_pRm->GetResource("textShader"));
    IResource& theTexture = m_pRm->GetResource("font");

	// use the shader
	glUseProgram(theShader.id);

	// set shader parameters
	glUniformMatrix4fv(theShader.uniforms["MVP"],1,false,&m_pCamera->viewProj()[0][0]);

	GLuint ColorId = theShader.uniforms["textColor"];

	GLuint TexId = theShader.uniforms["myTextureSampler"];
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, theTexture.id);
	glUniform1i(TexId,0);

	GLuint vertexPosition_modelspaceID = glGetAttribLocation(theShader.id, "vertexPosition_modelspace");
    GLuint vertexUV = glGetAttribLocation(theShader.id, "vertexUV");

	glEnableVertexAttribArray(0);
	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);
	glVertexAttribPointer(
		   vertexPosition_modelspaceID,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		   3,                  // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   sizeof(FontVertex),  // stride
		   (void*)0            // array buffer offset
		);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		   vertexUV,					// attribute 1
		   2,                  // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   sizeof(FontVertex),  // stride
		   (void*)sizeof(glm::vec3) // array buffer offset
		);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_uiIndexBuffer);	

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int uiStartingIndex = 0;
	for(unsigned int j = 0; j < m_textSubsets.size(); ++j)
	{
		//glUniform3f(ColorId,m_textSubsets[j].color.x,m_textSubsets[j].color.y,m_textSubsets[j].color.z);
		 
		 glDrawElements(
             GL_TRIANGLES,      // mode
			 (m_textSubsets[j].length) * 6,    // count
			 GL_UNSIGNED_SHORT,   // type
             (void*)(uiStartingIndex * 12)  // element array buffer offset
		 );   
		 uiStartingIndex += m_textSubsets[j].length;
	}

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	m_textSubsets.clear();
}


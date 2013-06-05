#include "FontEngine.h"
#include "GenerateBuffers.h"
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

FontEngine::FontEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam) :
	m_pRm(pRm), m_iMaxLength(2*maxLength), m_pCamera(pCam)
{
	OnReset();
}

FontEngine::~FontEngine()
{
	glDeleteBuffers(1,&m_uiIndexBuffer);
	glDeleteBuffers(1,&m_uiVertexBuffer);
}

void FontEngine::GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const
{
	if(str == nullptr)
		return;

	glm::vec2 topLeft(0.0f,0.0f);
	glm::vec2 bottomRight(topLeft);
	bottomRight.y -= 5.0f*scale.y + 5.0f;

	const Charset& font = static_cast<const Charset&>(m_pRm->GetResource("font")); // todo: need to check if the resource is actually a font

	while(*str)
	{
		// todo: need to add space and newline calculations
		//if(*str != ' ' && *str != '\n')
		{
			float fAdvance = 5.0f*scale.x * font.Chars[(*str)].Width / (float)font.iWidth;
			bottomRight.x += 5.0f*fAdvance * scale.x + 0.5f;
		}
		str++;
	}


	out = Math::FRECT(topLeft,bottomRight);
}

void FontEngine::DrawString(const char* str, const char* font, const glm::vec2& pos, const glm::vec2& scale, const glm::vec3& color, FontAlignment options)
{
	if(str == nullptr)
		return;

	if(font == nullptr)
		font = "font";

	m_textSubsets[font].push_back(DrawTextInfo(str,pos,glm::vec2(5.0f)*scale,color,options));
}

void FontEngine::OnReset()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
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

void FontEngine::FillVertexBuffer()
{
	// bind the vertex buffer that we are going to write to
	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);

	// get the vertex buffer memory to write to
	FontVertex* v = (FontVertex*)glMapBufferRange(GL_ARRAY_BUFFER , 0, m_iMaxLength * sizeof(FontVertex), GL_MAP_WRITE_BIT);

	unsigned int iCurrentVerts = 0;

	// Loop over all texture subsets and write them to the vertex buffer
	for(auto iter = m_textSubsets.begin(); iter != m_textSubsets.end(); ++iter)
	{
		// Get the current font
		const Charset& font = static_cast<const Charset&>(m_pRm->GetResource(iter->first)); // todo: need to check if the resource is actually a font

		// Loop over all fonts with the current texture
		for(auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
		{
			unsigned int iVert = iCurrentVerts; // current vertex
			const char* str = subIter->text.c_str();
			
			glm::vec3 posW(subIter->pos,0.0f); // World pos of where the text will be drawn
			
			if(subIter->options == FontAlignment::Center)
			{
				Math::FRECT drawRec;
				GetStringRec(str,subIter->scale / 5.0f,drawRec);

				posW.x -= drawRec.Width() / 2.0f;
			}
			//else if (subIter->options == FontAlignment::Left)
			{
				//posW = glm::vec3(subIter->pos,0.0f);
			}
			//else
			{
				 // right
			}

			// Loop over the entire string and write to the vertex buffer
			while((iVert < m_iMaxLength) && *str)
			{
				char character = *str++; // character to draw
				const CharDescriptor& charToRender = font.Chars[character]; // font info about the character to draw
				float fAdvance = (subIter->scale.x * charToRender.Width / (float)font.iWidth); // // How much we will advance from the current character

				if((character != '\n') && (character != '\t') && (character != ' '))
				{
					// cull characters that are off the screen
					if(posW.x >= -100.0f && posW.x <= 100.0f && posW.y <= 100.0f && posW.y >= -100.0f)
					{
						int index = iVert * 4;

						// calc tex coords
						glm::vec2 topLeft((charToRender.x / (float)font.iWidth),charToRender.y / (float)font.iHeight);
						glm::vec2 bottomRight(((charToRender.x+charToRender.Width) / (float)font.iWidth),(charToRender.y+charToRender.Height) / (float)font.iHeight);

						v[index].pos = glm::vec3(posW.x,0.5f * subIter->scale.y + posW.y,0.0f);
						v[index].tex = topLeft; // topLeft
						//v[index].tex = glm::vec2(0.0,0.0f);

						v[index + 1].pos = glm::vec3(posW.x,-0.5f * subIter->scale.y + posW.y,0.0f);
						v[index + 1].tex = glm::vec2(topLeft.x,bottomRight.y); // glm::vec2(topLeft.x,bottomRight.y) bottom left
						//v[index + 1].tex = glm::vec2(0.0,1.0f);

						v[index + 2].pos = glm::vec3(fAdvance * subIter->scale.x,0.5f * subIter->scale.y,0.0f) + posW;
						v[index + 2].tex = glm::vec2(bottomRight.x,topLeft.y) ; // glm::vec2(bottomRight.x,topLeft.y) top right
						//v[index + 2].tex = glm::vec2(1.0,0.0f);

						v[index + 3].pos = glm::vec3(fAdvance * subIter->scale.x,-0.5f * subIter->scale.y,0.0f) + posW;
						v[index + 3].tex = bottomRight   ; // bottomRight
						//v[index + 3].tex = glm::vec2(1.0,1.0f);

						++iVert;
					}
				}
				else if(character == '\n')
				{
					posW.x = subIter->pos.x;
					posW.y -= 1.2f*subIter->scale.y;
					continue;

				}
				else if(character == '\t')
				{
					//i += 10;
				}
				else if(character == ' ')
				{
					fAdvance = 0.2f;
				}

				posW.x += fAdvance * subIter->scale.x + 0.5f;
				//posW.x += 2.0f * fAdvance * subIter->scale.x; // * 10 / 2
			}

			subIter->length = iVert - iCurrentVerts;
			iCurrentVerts += subIter->length;
		}

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

	// use the shader
	glUseProgram(theShader.id);

	// set shader parameters
	glUniformMatrix4fv(theShader.uniforms["MVP"],1,false,&m_pCamera->viewProj()[0][0]);

	GLuint ColorId = theShader.uniforms["textColor"];
	GLuint TexId = theShader.uniforms["myTextureSampler"];
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
	for(auto iter = m_textSubsets.begin(); iter != m_textSubsets.end(); ++iter)
	{
		IResource& currentTexture = m_pRm->GetResource(iter->first);

		// Set the texture to use
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, currentTexture.id);
		glUniform1i(TexId,0);

		// Loop over all fonts with the current texture
		for(auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
		{
			glUniform3f(ColorId,subIter->color.x,subIter->color.y,subIter->color.z);

			glDrawElements(
						GL_TRIANGLES,      // mode
						subIter->length * 6,    // count
						GL_UNSIGNED_SHORT,   // type
						(void*)(uiStartingIndex * 12)  // element array buffer offset
						);

			uiStartingIndex += subIter->length;
		}
	}

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	m_textSubsets.clear();
}


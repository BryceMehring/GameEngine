#include "FontEngine.h"
#include "GenerateBuffers.h"
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

#include <stddef.h>

FontEngine::FontEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam) :
	m_pRm(pRm), m_pCamera(pCam), m_iMaxLength(2*maxLength)
{
	OnReset();
}

FontEngine::~FontEngine()
{
	DeleteBuffers();
}

void FontEngine::DeleteBuffers()
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
	bottomRight.y -= 2.0f*scale.y;


	const Charset* font = static_cast<const Charset*>(m_pRm->GetResource("font")); // todo: need to check if the resource is actually a font

	while(*str)
	{
		float fAdvance = 0.2f;

		// todo: need to newline calculations
		if(*str != ' ' /*&& *str != '\n'*/)
		{
			fAdvance = scale.x * font->GetCharDescriptor()[(unsigned int)(*str)].Width / (float)font->GetWidth();
		}

		bottomRight.x += fAdvance * scale.x + 0.5f;
		str++;
	}

	out = Math::FRECT(topLeft,bottomRight);

}

void FontEngine::DrawString(const char* str, const char* font, const glm::vec3& pos, const glm::vec2& scale, const glm::vec3& color, FontAlignment options)
{
	if(str == nullptr)
		return;

	if(font == nullptr)
		font = "font";

	m_textSubsets[font].push_back(DrawTextInfo(str,pos,scale,color,options));
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

void FontEngine::FillVertexBuffer(std::vector<unsigned int>& output)
{
	output.resize(m_textSubsets.size());

	// bind the vertex buffer that we are going to write to
	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);

	// get the vertex buffer memory to write to
	FontVertex* v = (FontVertex*)glMapBufferRange(GL_ARRAY_BUFFER , 0, m_iMaxLength * sizeof(FontVertex), GL_MAP_WRITE_BIT);

	unsigned int iCurrentVerts = 0;
	unsigned int i = 0;
	// Loop over all texture subsets and write them to the vertex buffer
	for(auto iter = m_textSubsets.begin(); iter != m_textSubsets.end(); ++iter)
	{
		unsigned int length = 0;

		// Loop over all fonts with the current texture
		for(auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
		{
			unsigned int iVert = iCurrentVerts; // current vertex
			const char* str = subIter->text.c_str();

			// World pos of text aligned to the left
			glm::vec3 posW(subIter->pos); // World pos of where the text will be drawn

			// If the text needs to be aligned to center or right
			if(subIter->options == FontAlignment::Center || subIter->options == FontAlignment::Right)
			{
				Math::FRECT drawRec;
				GetStringRec(str,subIter->scale,drawRec);

				float fHalfWidth = (drawRec.Width() / 2.0f);

				posW.x -= fHalfWidth;

				if (subIter->options == FontAlignment::Right)
				{
					posW.x -= fHalfWidth;
				}
			}
			else
			{
				// left, do nothing
			}

			// Get the current font
			const Charset* font = static_cast<const Charset*>(m_pRm->GetResource(iter->first)); // todo: need to check if the resource is actually a font

			// Loop over the entire string and write to the vertex buffer
			while((iVert < m_iMaxLength) && *str)
			{
				char character = *str++; // character to draw
				const CharDescriptor& charToRender = font->GetCharDescriptor()[(unsigned int)character]; // font info about the character to draw
				float fAdvance = (subIter->scale.x * charToRender.Width / (float)font->GetWidth()); // // How much we will advance from the current character

				if((character != '\n') && (character != '\t') && (character != ' '))
				{
					// cull characters that are off the screen
					if(posW.x >= -100.0f && posW.x <= 100.0f && posW.y <= 100.0f && posW.y >= -100.0f)
					{
						int index = iVert * 4;

						// calc tex coords
						glm::vec2 topLeft((charToRender.x / (float)font->GetWidth()),charToRender.y / (float)font->GetHeight());
						glm::vec2 bottomRight(((charToRender.x+charToRender.Width) / (float)font->GetWidth()),(charToRender.y+charToRender.Height) / (float)font->GetHeight());

						v[index].pos = glm::vec3(posW.x,0.5f * subIter->scale.y + posW.y,posW.z);
						v[index].uv = topLeft; // topLeft
						v[index].color = subIter->color;

						v[index + 1].pos = glm::vec3(posW.x,-0.5f * subIter->scale.y + posW.y,posW.z);
						v[index + 1].uv = glm::vec2(topLeft.x,bottomRight.y); // glm::vec2(topLeft.x,bottomRight.y) bottom left
						v[index + 1].color = subIter->color;

						v[index + 2].pos = glm::vec3(fAdvance * subIter->scale.x,0.5f * subIter->scale.y,0.0f) + posW;
						v[index + 2].uv = glm::vec2(bottomRight.x,topLeft.y) ; // glm::vec2(bottomRight.x,topLeft.y) top right
						v[index + 2].color = subIter->color;

						v[index + 3].pos = glm::vec3(fAdvance * subIter->scale.x,-0.5f * subIter->scale.y,0.0f) + posW;
						v[index + 3].uv = bottomRight   ; // bottomRight
						v[index + 3].color = subIter->color;

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
			}

			unsigned int uiCurrentLength = iVert - iCurrentVerts;
			length += uiCurrentLength;
			iCurrentVerts += uiCurrentLength;
		}

		output[i++] = (length);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void FontEngine::Render()
{
	// if there is nothing to draw, do nothing
	if(m_textSubsets.empty())
		return;

	std::vector<unsigned int> numChar;

	// first we fill the vertex buffer with the text to render
	FillVertexBuffer(numChar);

	// get the shader to use
	const TexturedShader* pShader = static_cast<TexturedShader*>(m_pRm->GetResource("textShader"));

	// use the shader
	glUseProgram(pShader->GetID());

	// set shader parameters
	glUniformMatrix4fv(pShader->GetMVP(),1,false,&m_pCamera->viewProj()[0][0]);

	GLuint vertexPosition_modelspaceID = glGetAttribLocation(pShader->GetID(), "vertexPosition_modelspace");
	GLuint vertexUV = glGetAttribLocation(pShader->GetID(), "vertexUV");
	GLuint vertexColor = glGetAttribLocation(pShader->GetID(), "vertexColor");

	// Create our vertex structure in OpenGL
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

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
				vertexColor,					// attribute 1
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(FontVertex),  // stride
				(void*)offsetof(struct FontVertex,color)  // array buffer offset
				);

	// Bind index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_uiIndexBuffer);

	// Enables one texture
	glActiveTexture(GL_TEXTURE0);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int uiStartingIndex = 0;
	unsigned int i = 0;
	for(auto iter = m_textSubsets.begin(); iter != m_textSubsets.end(); ++iter, ++i)
	{
		const IResource* pCurrentTexture = m_pRm->GetResource(iter->first);

		// Set the texture to use

		glBindTexture(GL_TEXTURE_2D, pCurrentTexture->GetID());
		glUniform1i(pShader->GetTextureSamplerID(),0);

		glDrawElements(GL_TRIANGLES, numChar[i] * 6, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(uiStartingIndex * 12));

		uiStartingIndex += numChar[i];
	}

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	m_textSubsets.clear();
}


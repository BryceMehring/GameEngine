#include "FontEngine.h"
#include <glm/gtx/transform.hpp>

FontEngine::FontEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam) :
m_pRm(pRm), m_iMaxLength(maxLength), m_iFenceIndex(0), m_pCamera(pCam)
{
	m_VertexBuffers.resize(3);
	m_fences.resize(3);

	CreateIndexBuffer();
	CreateVertexBuffer();
}

FontEngine::~FontEngine()
{
	for(unsigned int i = 0; i < m_fences.size(); ++i)
	{
		glDeleteSync( m_fences [i]);
	}

	glDeleteBuffers(1,&m_uiIndexBuffer);
	glDeleteBuffers(m_VertexBuffers.size(),&m_VertexBuffers.front());
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
	std::vector<unsigned short> indexBuffer;

	indexBuffer.resize(6*m_iMaxLength);
	for(unsigned int i = 0; i < m_iMaxLength; ++i)
	{
		// Tri 1
		indexBuffer[i*6] = 4*i + 1;
		indexBuffer[i*6 + 1] =  4*i + 2;
		indexBuffer[i*6 + 2] = 4*i;

		//Tri 2
		//
		indexBuffer[i*6 + 3] = 4*i + 2;
		indexBuffer[i*6 + 4] = 4*i + 3;
		indexBuffer[i*6 + 5] = 4*i + 1;
	}

	glGenBuffers(1,&m_uiIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned short), &indexBuffer.front(), GL_STATIC_DRAW);
	
}

void FontEngine::CreateVertexBuffer()
{
	glGenBuffers(3,&m_VertexBuffers.front());

	for(unsigned int i = 0; i < m_VertexBuffers.size(); ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffers[i]);
		glBufferData(GL_ARRAY_BUFFER, m_iMaxLength * sizeof(FontVertex), 0, GL_DYNAMIC_DRAW);
	}
}

glm::vec2 FontEngine::GetTextOffset(const glm::vec2& scale, unsigned int i, unsigned int j) const
{
	return glm::vec2(scale.x * i/2.0f,-scale.y * j);
}

void FontEngine::FillVertexBuffer()
{
	GLenum result = glClientWaitSync( m_fences [ m_iFenceIndex], 0, 1 );
	glDeleteSync( m_fences [m_iFenceIndex]);

	glBindBuffer( GL_ARRAY_BUFFER , m_VertexBuffers[m_iFenceIndex]);

	//glMapBufferRange(
	FontVertex* v = (FontVertex*)glMapBufferRange(GL_ARRAY_BUFFER , 0, m_iMaxLength * sizeof(FontVertex), GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	unsigned int iCurrentVerts = 0;

	for(auto iter = m_textSubsets.begin(); iter != m_textSubsets.end(); ++iter)
	{
		const char* str = iter->text.c_str();

		int i = 0; // x pos
		int j = 0; // y pos
		unsigned int iVert = iCurrentVerts; // current vertex

		// Loop over entire string or until we have filled the buffer
		while((iVert < m_iMaxLength) && *str)
		{
			char character = *str++;

			if((character != '\n') && (character != '\t') && (character != ' '))
			{
				// World Position
				glm::vec2 textOffset = GetTextOffset(iter->scale,i,j);
				glm::vec3 posW(iter->pos.x + textOffset.x,iter->pos.y + textOffset.y,0.0f);

				// todo: remove this, or add this functionality elsewhere
				/*if(posW.x > 40.0f)
				{
					i = -1;
					++j;
				}*/

				// cull characters that are off the screen
				if(posW.x >= -50.0f && posW.x <= 50.0f && posW.y <= 50.0f && posW.y >= -50.0f)
				{
					int index = iVert * 4;

					int x = character % 16; // Column
					int y = (character / 16); // Row

					// tex coords
					glm::vec2 topLeft((x / 16.0f),y / 16.0f);
					glm::vec2 bottomRight(((x+1) / 16.0f),(y+1) / 16.0f);

					v[index].pos = glm::vec3(-0.5f * iter->scale.x,0.5f * iter->scale.y,0.0f) + posW;
					v[index].tex = topLeft;
					//v[index].tex = glm::vec2(1.0,1.0f);

					v[index + 1].pos = glm::vec3(-0.5f * iter->scale.x,-0.5f * iter->scale.y,0.0f) + posW;
					v[index + 1].tex = glm::vec2(topLeft.x,bottomRight.y);
					//v[index + 1].tex = glm::vec2(1.0,0.0f);

					v[index + 2].pos = glm::vec3(0.5f * iter->scale.x,0.5f * iter->scale.y,0.0f) + posW;
					v[index + 2].tex = glm::vec2(bottomRight.x,topLeft.y);
					//v[index + 2].tex = glm::vec2(0.0,1.0f);

					v[index + 3].pos = glm::vec3(0.5f * iter->scale.x,-0.5f * iter->scale.y,0.0f) + posW;
					v[index + 3].tex = bottomRight;
					//v[index + 3].tex = glm::vec2(0.0,0.0f);

					++iVert;
				}
			}
			else if(character == '\n')
			{
				i = -1;
				++j;
			}
			else if(character == '\t')
			{
				i += 10;
			}

			++i;
		}

		iter->length = iVert - iCurrentVerts;
		iCurrentVerts += iter->length;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void FontEngine::Render()
{
	// first we fill the vertex buffer with the text to render
	FillVertexBuffer();

	// get the shader to use
    Shader& theShader = static_cast<Shader&>(m_pRm->GetResource("2dshader"));
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

	glEnableVertexAttribArray(0);
	glBindBuffer( GL_ARRAY_BUFFER , m_VertexBuffers[m_iFenceIndex]);
	glVertexAttribPointer(
		   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		   3,                  // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   sizeof(FontVertex),  // stride
		   (void*)0            // array buffer offset
		);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		   1,					// attribute 1
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
		glUniform3f(ColorId,m_textSubsets[j].color.x,m_textSubsets[j].color.y,m_textSubsets[j].color.z);
		 
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

	m_fences[m_iFenceIndex] = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE , 0) ;
	m_iFenceIndex = (m_iFenceIndex + 1) % m_fences.size();

	m_textSubsets.clear();
}


#include "TextBox.h"
#include <fstream>

namespace UI
{

using namespace std;

// textbox ctor
TextBox::TextBox(const glm::vec2& pos, float w, float h)
	: m_text(1), m_pos(pos)
{
}
TextBox::~TextBox()
{
}

void TextBox::SaveToFile(const std::string& file) const
{
	fstream out(file.c_str(),ios::out);

	if(out.is_open())
	{
		for(unsigned int i = 0; i < m_text.size(); ++i)
		{
			out << m_text[i].line.c_str() << endl;
		}

		out.close();
	}
}

void TextBox::Write(const string& line, const glm::vec3& color, bool c)
{
	m_text.push_back(LineData(line,color,c));
}

void TextBox::Backspace()
{
	// reference to the line
	string& text = m_text.back().line;
	size_t size = text.size();

	// if the current line has text to remove
	if(size > 0)
	{
		// remove last character
		text.resize(size - 1);
	}
	// If there is no text to remove, try to pop the line in the vector
	else if(m_text.size() > 1)
	{
		m_text.pop_back();
	}
}

void TextBox::AddKey(char Key)
{
	// Add key to the last line
	string& text = m_text.back().line;
	text += Key;
}

void TextBox::Update(IKMInput& input, double dt)
{
	if(input.KeyPress(259))
	{
		Backspace();
	}
	else if(input.KeyPress(257))
	{
		Write();
	}
	else
	{
		char inputChar;
		if(input.CharKeyDown(inputChar))
		{
			AddKey(inputChar);
		}
	}

	UpdateScrolling(input,dt);
}

void TextBox::Render(IRenderer& renderer)
{
	glm::vec2 pos = m_pos;

	// Iterate across all lines
	for(auto iter = m_text.begin(); iter != m_text.end(); ++iter)
	{
		const LineData& data = *iter;

		renderer.DrawString(("> " + data.line).c_str(),glm::vec3(pos,0.0f),glm::vec2(2.0f),data.color);

		pos.y -= 10.0f;
	}
}

void TextBox::UpdateScrolling(IKMInput& input, double dt)
{
	if(input.MouseClick(0,false))
	{
		m_pos.y = input.GetCursorPos().y;
	}
}

void TextBox::CLS()
{
	m_text.clear();
}

}

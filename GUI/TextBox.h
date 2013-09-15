#ifndef _TEXTBOX_
#define _TEXTBOX_

#include "IElement.h"
#include "IKMInput.h"
#include "IRenderer.h"

namespace UI
{

// A basic ui textbox
class TextBox : public IElement
{
public:

	TextBox(const glm::vec2& pos, float w, float h);
	virtual ~TextBox();

	// Enters a new line
	void Write(const std::string& line = "", const glm::vec3& color = glm::vec3(0.0f), bool bContinue = false);
	
	virtual void Update(IKMInput&, double dt);
	virtual void Render(IRenderer&);
	
	virtual void CLS();

	void SaveToFile(const std::string& file) const;

	// todo: need to implement
	virtual void Select() {}
	virtual void Deselect() {}
	virtual void Trigger() {}

protected:

	struct LineData
	{
		LineData() {}
		LineData(const std::string& str, const glm::vec3& color,bool c)
			: line(str), color(color), bContinue(c)
		{
		}

		std::string line;
		glm::vec3 color;
		bool bContinue;
	};

	typedef std::vector<LineData> TextDataType;
	TextDataType m_text;

	glm::vec2 m_pos;

	// ===== helper functions =====
protected:
	virtual void Backspace();
	virtual void AddKey(char Key);
private:

	// todo: passing around these interfaces seems redundant
	void UpdateScrolling(class IKMInput&, double dt);
	void UpdateCarrot(class IKMInput&);
	void UpdateInput(class IKMInput&);
	void UpdateTextInput(class IKMInput&);

};

}

#endif // _TEXTBOX_
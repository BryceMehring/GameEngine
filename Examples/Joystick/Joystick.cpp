#include "Joystick.h"
#include "Game.h"

#include <sstream>

extern "C" PLUGINDECL IPlugin* CreatePlugin()
{
	return new Joystick();
}

Joystick::Joystick() : m_posLS(-100.0f,50), m_posRS(100.0f, 0.0f)
{

}

void Joystick::Init(Game& game)
{
	IRenderer& renderer = game.GetRenderer();
	int width, height;

	// Build the world space camera
	renderer.GetDisplayMode(width, height);
	m_camera.SetLens(90.0f, (float)width, (float)height, 0.0f, 50.0f);
	m_camera.LookAt(glm::vec3(0.0f,0.0f,100.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_camera.Update();

	// Give our camera to the renderer
	renderer.SetCamera(&m_camera);
}

void Joystick::Destroy(Game& game)
{
	IRenderer& renderer = game.GetRenderer();
	renderer.SetCamera(nullptr);
}

void Joystick::Update(Game& game)
{
	IInput& input = game.GetInput();

	// Update the position of the line with the controller
	m_posLS += glm::vec2(200, -200) * input.GetJoystickAxes(JoystickAxes::LS) * (float)game.GetDt();
	m_posRS += glm::vec2(200, -200) * input.GetJoystickAxes(JoystickAxes::RS) * (float)game.GetDt();

	// Update the camera
	float trigger = input.GetJoystickAxes(JoystickAxes::LT).y;

	m_camera.LookAt(glm::vec3(0.0f, 0.0f, trigger * 50 + 100), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_camera.Update();
}

void Joystick::Draw(Game& game)
{
	IInput& input = game.GetInput();
	IRenderer& renderer = game.GetRenderer();

	// Line vertices
	glm::vec3 line[] =
	{
		glm::vec3(m_posLS.x, m_posLS.y, 0),
		glm::vec3(m_posRS.x, m_posRS.y, 0)
	};

	// Render the line in world space
	renderer.SetRenderSpace(RenderSpace::World);
	renderer.DrawLine(line, 2);

	// Render the joysticks name in screen space
	renderer.SetRenderSpace(RenderSpace::Screen);

	std::ostringstream stream;
	stream << "Joystick Name: " << input.GetJoystickName();

	int width, height;
	renderer.GetDisplayMode(width, height);
	renderer.DrawString(stream.str().c_str(), glm::vec3(width / 2.0f, height, 0.0f), 0.5f, glm::vec4(1.0f), nullptr, FontAlignment::Center);
}
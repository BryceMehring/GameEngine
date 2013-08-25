#ifndef _IGAME_
#define _IGAME_

#include <string>

class IGame
{
public:

	virtual ~IGame() {}

	virtual const std::string& GetCurrentState() const = 0;
	virtual void SetNextState(const std::string& state) = 0;
	virtual void ReloadPlugins() = 0;

	virtual class IRenderer& GetRenderer() = 0;
	virtual class IKMInput& GetInput() = 0;
	virtual class PluginManager& GetPM() = 0;
	virtual class asVM& GetAs() = 0;

	virtual double GetDt() const = 0; // time diff between frames in seconds
	virtual unsigned int GetFps() const = 0;
};

#endif // _IGAME_
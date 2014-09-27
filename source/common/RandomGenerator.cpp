#include "RandomGenerator.h"
#include <chrono>

Random& Random::Instance()
{
	static Random instance;
	return instance;
}

Random::Random() : m_generator(std::chrono::system_clock::now().time_since_epoch().count())
{
}

int Random::Generate(int max)
{
	return Generate(0, max);
}

float Random::Generate(float max)
{
	return Generate(0.0f, max);
}

int Random::Generate(int min, int max)
{
	return std::uniform_int_distribution<int>(min, max)(m_generator);
}

float Random::Generate(float min, float max)
{
	return std::uniform_real_distribution<float>(min, max)(m_generator);
}

glm::vec2 Random::GenerateVector(const glm::vec2& min, const glm::vec2& max)
{
	return glm::vec2(Generate(min.x, max.x), Generate(min.y, max.y));
}

glm::vec3 Random::GenerateVector(const glm::vec3& min, const glm::vec3& max)
{
	return glm::vec3(Generate(min.x, max.x),
					 Generate(min.y, max.y),
					 Generate(min.z, max.z));
}

#include "RandomGenerator.h"
#include <chrono>
#include <array>

Random& Random::Instance()
{
	static Random instance;
	return instance;
}

Random::Random()
{
	std::random_device r;
	std::array<int, std::mt19937::state_size> seed_data;
	std::generate(seed_data.begin(), seed_data.end(), std::ref(r));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
	m_generator.seed(seq);
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

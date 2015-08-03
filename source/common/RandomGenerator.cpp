#include "RandomGenerator.h"
#include <chrono>
#include <array>
#include <algorithm>
#include <functional>

RandomGenerator& RandomGenerator::Instance()
{
	static RandomGenerator instance;
	return instance;
}

RandomGenerator::RandomGenerator()
{
	std::random_device r;
	if (r.entropy() > 0.0)
	{
		std::array<int, std::mt19937::state_size> seed_data;
		std::generate(seed_data.begin(), seed_data.end(), std::ref(r));
		std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
		m_generator.seed(seq);
	}
	else
	{
		m_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	}
}

int RandomGenerator::Generate(int max)
{
	return Generate(0, max);
}

float RandomGenerator::Generate(float max)
{
	return Generate(0.0f, max);
}

int RandomGenerator::Generate(int min, int max)
{
	return std::uniform_int_distribution<int>(min, max)(m_generator);
}

float RandomGenerator::Generate(float min, float max)
{
	return std::uniform_real_distribution<float>(min, max)(m_generator);
}

glm::vec2 RandomGenerator::GenerateVector(const glm::vec2& min, const glm::vec2& max)
{
	return glm::vec2(Generate(min.x, max.x), Generate(min.y, max.y));
}

glm::vec3 RandomGenerator::GenerateVector(const glm::vec3& min, const glm::vec3& max)
{
	return glm::vec3(Generate(min.x, max.x),
					 Generate(min.y, max.y),
					 Generate(min.z, max.z));
}

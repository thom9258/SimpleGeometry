#pragma once

#include <ctime>

#include  <random>
#include  <iterator>

int random_int(const int min, const int max)
{
	const auto rand_max = RAND_MAX;
	return min + rand() /( rand_max/(max-min));
}

float random_float(const float min, const float max)
{
	const auto rand_max = static_cast<float>(RAND_MAX);
	return min + static_cast <float> (rand()) /( static_cast<float>(rand_max/(max-min)));
}

class RandomModelRotator 
{
public:
	explicit RandomModelRotator() 
		: RandomModelRotator(glm::translate(glm::scale(glm::mat4(1.0f),
													   glm::vec3(random_float(0.5f, 1.0f))),
											glm::vec3(random_float(-5.0f, 5.0f),
													  random_float(0.0f, 2.0f),
													  random_float(-5.0f, 5.0f))))
	{}

	explicit RandomModelRotator(const glm::mat4 model)
		: m_model(model)
	{
		recalculate();
	}
	
	RandomModelRotator& recalculate()
	{
		m_speed = random_float(40.0f, 90.0f);
		m_axis = glm::vec3(random_float(-1.0f, 1.0f),
						   random_float(-1.0f, 1.0f),
						   random_float(-1.0f, 1.0f));
		return *this;
	}

	glm::mat4 final_model_matrix(const float time) const noexcept
	{
		return glm::rotate(m_model, time * glm::radians(m_speed), m_axis); 
	}

private:
	glm::mat4 m_model = glm::mat4(1.0f);
	glm::vec3 m_axis = glm::vec3(0.0f);
	float m_speed = 0.0f;
};

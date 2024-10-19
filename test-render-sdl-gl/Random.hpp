#pragma once

#include <ctime>

float random_float(const float min, const float max)
{
	const auto rand_max = static_cast<float>(RAND_MAX);
	return min + static_cast <float> (rand()) /( static_cast<float>(rand_max/(max-min)));
}

class RandomModelRotator 
{
public:
	explicit RandomModelRotator() 
	{
		m_speed = random_float(40.0f, 90.0f);
		m_axis = glm::vec3(random_float(-1.0f, 1.0f),
						   random_float(-1.0f, 1.0f),
						   random_float(-1.0f, 1.0f));
		
		m_model = glm::mat4(1.0f);
		m_model = glm::scale(m_model, glm::vec3(random_float(0.5f, 1.0f)));
		m_model = glm::translate(m_model, glm::vec3(random_float(-1.0f, 1.0f),
													random_float(-1.0f, 1.0f),
													random_float(-1.0f, 1.0f)));
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


template <typename Mesh>
struct RandomRotatedMesh {
	Mesh mesh;
	RandomModelRotator rotator;
};

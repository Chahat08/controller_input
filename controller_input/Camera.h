#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, -5.0f);

	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.1f);
	glm::vec3 targetDirection = glm::normalize(position - target);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 rightDirection = glm::normalize(glm::cross(up, targetDirection));

	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 getLookAt();
};
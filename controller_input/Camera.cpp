#include "Camera.h"

glm::mat4 Camera::getLookAt() {
	return glm::lookAt(position, position + target, up);
}
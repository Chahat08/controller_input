#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "ControllerState.h"
#include "Camera.h"

#include "vrpn/vrpn_Analog.h"
#include "vrpn/vrpn_Button.h"

float lx = 0.0f, ly = 0.0f, rx = 0.0f, ry = 0.0f;
bool b10 = false, b11 = false, b12 = false, b13 = false;

void VRPN_CALLBACK handle_analog(void* userData, const vrpn_ANALOGCB a)

{
	/*int nbChannels = a.num_channel;
	std::cout << "Analog : ";
	for (int i = 0; i < a.num_channel; i++)
	{
		std::cout << a.channel[i] << " ";

	}

	std::cout << std::endl;*/
	lx = a.channel[0];
	ly = a.channel[1];
	rx = a.channel[2];
	ry = a.channel[3];

}

void VRPN_CALLBACK handle_button(void* userData, const vrpn_BUTTONCB b)

{

	//std::cout << "Button '" << b.button << "': " << b.state << std::endl;
	if (b.button == 10) b10 = b.state;
	if (b.button == 11) b11 = b.state;
	if (b.button == 12) b12 = b.state;
	if (b.button == 13) b13 = b.state;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

const char* vertexShaderSource =
"#version 330 core\n"
"layout(location=0) in vec3 vertexPosition;\n"
"uniform mat4 model, view, projection;"
"void main(){\n"
"	gl_Position = projection*view*model*vec4(vertexPosition, 1.0);\n"
"}\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 fragmentColor;\n"
"void main(){\n"
"	fragmentColor = vec4(0.38, 0.431, 0.922, 1.0);\n"
"}\0";

int main() {

	if (!glfwInit()) {
		std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "title", NULL, NULL);
	if (!window) {
		std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "ERROR: Failed to initialize GLAD." << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	float vertices[] = {
		0.0, 0.75, 0.0,
		0.5, -0.5, 0.0,
		-0.5, -0.5, 0.0
	};

	unsigned int VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	ControllerState controllerState(0.0,0.0,0.0,0.0);
	ControllerState* state = NULL;

	int joystickPresent = glfwJoystickPresent(GLFW_JOYSTICK_1);

	int axesCnt, buttonsCnt;
	const float* axes;
	const unsigned char* buttons;

	if (joystickPresent) {
		axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCnt);
		controllerState.setInitialAxesValues(axes[0], axes[1], axes[2], axes[3]);
		buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonsCnt);
		state = &controllerState;
	}
	else std::cout << "JOYSTICK NOT PRESENT, USING MOUSE." << std::endl;

	Camera camera;

	float rotateX = 0.0f, rotateY = 0.0f, rotateSpeed = 0.1f; // rotation parameters
	float translateX = 0.0f, translateZ = 0.0f, moveSpeed = 0.005f;
	float pitch = 0.0f, yaw = 90.0f, sensitivity = 0.5f;
	bool chagned = false;

	vrpn_Analog_Remote* vrpnAnalog = new vrpn_Analog_Remote("device0@localhost");
	vrpn_Button_Remote* vrpnButton = new vrpn_Button_Remote("device0@localhost");

	vrpnAnalog->register_change_handler(0, handle_analog);
	vrpnButton->register_change_handler(0, handle_button);

	while (!glfwWindowShouldClose(window)) {

		processInput(window);

		if (state) {
			buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonsCnt);
			
			// rotation
			//if (buttons[10] == GLFW_PRESS) // up
			//	rotateX += 1.0f;
			//if (buttons[11] == GLFW_PRESS)// right
			//	rotateY += 1.0f;
			//if (buttons[12] == GLFW_PRESS) // down
			//	rotateX -= 1.0f;
			//if (buttons[13] == GLFW_PRESS) //left
			//	rotateY -= 1.0f;

			if (b10) // up
				rotateX += 1.0f;
			if (b11)// right
				rotateY += 1.0f;
			if (b12) // down
				rotateX -= 1.0f;
			if (b13) //left
				rotateY -= 1.0f;
			

			// camera position
			axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCnt);
			if (state->leftAxesChanged(axes[0], axes[1])) {
				if (!chagned) {
					camera.position += (ly * moveSpeed) * camera.targetDirection;
					camera.position += (lx * moveSpeed) * glm::normalize(glm::cross(camera.target, camera.up));
				}
					//camera.position += (axes[1] * moveSpeed) * camera.targetDirection;
				else {
					camera.position -= (lx * moveSpeed) * glm::normalize(glm::cross(camera.target, camera.up));
					camera.position -= (ly * moveSpeed) * camera.targetDirection;
				}
				
			}
			if (state->rightAxesChanged(axes[2], axes[3])) {
				chagned = true;
				/*yaw += axes[2]*sensitivity;
				pitch += axes[3]*sensitivity;*/

				yaw += rx * sensitivity;
				pitch += ry * sensitivity;

				glm::vec3 direction;
				direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
				direction.y = sin(glm::radians(pitch));
				direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
				camera.target = glm::normalize(direction);
				camera.targetDirection = camera.target;

				camera.rightDirection = glm::normalize(glm::cross(camera.target, camera.worldUp));
				camera.up = glm::normalize(glm::cross(camera.rightDirection, camera.targetDirection));
			}

		}

		vrpnAnalog->mainloop();
		vrpnButton->mainloop();

		glClearColor(0.878, 0.341, 0.314, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glm::mat4 model(1.0f), view(1.0f), perspective(1.0f);
		model = glm::translate(model, glm::vec3(0.0));

		model = glm::rotate(model, glm::radians(rotateX*rotateSpeed), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotateY*rotateSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
		
		
		view = camera.getLookAt();//glm::lookAt(glm::vec3(0.0,0.0,3.0), glm::vec3(0.0,0.0,-1.0)+ glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0,1.0,0.0));//camera.getLookAt();

		perspective = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		unsigned int loc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(loc, 1,GL_FALSE, glm::value_ptr(model));

		loc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

		loc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(perspective));


		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	
	glDeleteProgram(shaderProgram);

	glfwTerminate();

	return 0;
}

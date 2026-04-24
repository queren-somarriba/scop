#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "scop.hpp"

namespace
{
	void InputMoveCam(GLFWwindow *window, AppState& state)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			state.camera.ProcessKeyboard(FORWARD, state.movementSpeed);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			state.camera.ProcessKeyboard(BACKWARD, state.movementSpeed);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			state.camera.ProcessKeyboard(LEFT, state.movementSpeed);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			state.camera.ProcessKeyboard(RIGHT, state.movementSpeed);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			state.camera.ProcessKeyboard(UP, state.movementSpeed);
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			state.camera.ProcessKeyboard(DOWN, state.movementSpeed);
	}

	void InputRotateModel(GLFWwindow *window, AppState& state)
	{
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !state.t_pressed)
		{
			state.showTexture = !state.showTexture;
			state.t_pressed = true;
		}
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
			state.t_pressed = false;
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !state.x_pressed)
		{
			state.angleX += 1.5708f;
			state.x_pressed = true;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
			state.x_pressed = false;
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !state.z_pressed)
		{
			state.angleZ += 1.5708f;
			state.z_pressed = true;
		}
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
			state.z_pressed = false;
	}
}

void processInput(GLFWwindow *window, AppState& state)
{
	InputMoveCam(window, state);
	InputRotateModel(window, state);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !state.space_pressed)
	{
		state.isRotatingY = !state.isRotatingY;
		state.space_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		state.space_pressed = false;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)window;
	(void)xoffset;
	AppState* state = reinterpret_cast<AppState*>(glfwGetWindowUserPointer(window));
	if (state)
		state->camera.ProcessMouseScroll(static_cast<float>(yoffset) * state->movementSpeed);
}
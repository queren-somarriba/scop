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

	void MouseRotateModel(GLFWwindow* window, AppState& state)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		if (state.mouse_pressed)
		{
			float deltaX = static_cast<float>(xpos - state.lastX);
			float deltaY = static_cast<float>(ypos - state.lastY);

			float sensitivity = 0.005f;

			state.angleY += deltaX * sensitivity;
			state.angleX += deltaY * sensitivity;
		}

		state.lastX = xpos;
		state.lastY = ypos;
	}
}

void processInput(GLFWwindow *window, AppState& state)
{
	InputMoveCam(window, state);
	MouseRotateModel(window, state);

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !state.t_pressed)
	{
		state.showTexture = !state.showTexture;
		state.t_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
		state.t_pressed = false;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		state.trunc += 0.01f * state.modelRadius;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		state.trunc -= 0.01f * state.modelRadius;
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
	(void)xoffset;
	scopData* data = reinterpret_cast<scopData*>(glfwGetWindowUserPointer(window));
	if (data && (data->noTextureNs >= 2.f || (data->noTextureNs >= 1.f && yoffset > 0.f)) &&
			(data->noTextureNs <= 99.f || (data->noTextureNs <= 100.f && yoffset < 0.f)))
		data->noTextureNs += static_cast<float>(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	(void)mods;
	scopData* data = reinterpret_cast<scopData*>(glfwGetWindowUserPointer(window));
	if (!data)
		return;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
			data->state.mouse_pressed = true;
		else if (action == GLFW_RELEASE)
			data->state.mouse_pressed = false;
	}
}
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "mat4f.hpp"
#include "texture.hpp"
#include "Renderer.hpp"
#include "camera.hpp"
#include "InputHandler.hpp"

namespace
{
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		(void)window;
		glViewport(0, 0, width, height);
	}

	void updateTextureContext(scopData& data)
	{
		mat4f view = data.state.camera.GetViewMatrix();
		mat4f projection = mat4f::perspective(data.state.camera.zoom, (float)SCR_WIDTH/SCR_HEIGHT, 0.1f,
														data.state.camera.pos.z + data.model.radius);

		mat4f model = mat4f::rotate(data.state.angleX, vect4f(1.f, 0.f, 0.f));
		model = model * mat4f::rotate(data.state.angleY, vect4f(0.f, 1.f, 0.f));

		data.shaderTexture->setFloat("Ztrunc", data.state.trunc);
		data.shaderTexture->setFloat("transition", data.state.transitionFactor);
		data.shaderTexture->setVec4("viewPos", data.state.camera.pos);
		data.shaderTexture->setMat4("model", model);
		data.shaderTexture->setMat4("invModelMat", model.inverse());
		data.shaderTexture->setMat4("view", view);
		data.shaderTexture->setMat4("projection", projection);		
	}

	void setMeshContexteAndDraw(scopData& data)
	{
		for (const MeshDraw& md : data.meshDraws)
		{
			auto it = data.model.materials.find(md.material_name);
			if (it != data.model.materials.end())
			{
				data.shaderTexture->setVec4("material.ambient", it->second.Ka);
				data.shaderTexture->setVec4("material.diffuse", it->second.Kd);
				data.shaderTexture->setVec4("material.specular", it->second.Ks);
				data.shaderTexture->setFloat("material.shininess", it->second.Ns);
			}
			else
			{
				data.shaderTexture->setVec4("material.ambient", vect4f(0.2f, 0.2f, 0.2f));
				data.shaderTexture->setVec4("material.diffuse", vect4f(0.8f, 0.8f, 0.8f));
				data.shaderTexture->setVec4("material.specular", vect4f(1.f, 1.f, 1.f));
				data.shaderTexture->setFloat("material.shininess", 32.f);

			}
			glDrawElements(GL_TRIANGLES, md.count, GL_UNSIGNED_INT, (void*)(md.offset * sizeof(GLuint)));
		}
	}

	void updateAppState(GLFWwindow* window, AppState& state, float radius)
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		state.deltaTime = currentFrame - state.lastFrame;
		state.lastFrame = currentFrame;
		state.second += state.deltaTime;
		++state.fpsCounter;
		if (state.second >= 1.f)
		{
			float fps = static_cast<float>(state.fpsCounter) / state.second;
			std::stringstream ss;
			ss << "scop - " << std::fixed << std::setprecision(0) << fps;
			ss << " fps / " << std::fixed << std::setprecision(1) << 1000.f / fps << " ms";
			glfwSetWindowTitle(window, ss.str().c_str());
			state.fpsCounter = 0;
			state.second = 0;
		}
		float target = state.showTexture ? 1.0f : 0.0f;
		state.movementSpeed = radius * state.deltaTime * 0.5f;
		if (state.isRotatingY)
			state.angleY += state.deltaTime;
		if (state.angleY >= 2.f * M_PI)
			state.angleY = 0.f;

		if (state.transitionFactor != target)
		{
			float step = 2.0f * state.deltaTime;
			if (state.transitionFactor < target)
				state.transitionFactor = std::min(state.transitionFactor + step, 1.0f);
			else
				state.transitionFactor = std::max(state.transitionFactor - step, 0.0f);
		}
	}
}

GLFWwindow* initWindow()
{
	if (!glfwInit())
	{
		std::cerr << "GLFW initialization failed" << std::endl;
		return NULL;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "scop", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return (NULL);
	}
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	return window;
}

void renderOBJ(GLFWwindow* window, scopData& data)
{
	updateAppState(window, data.state, data.model.radius);

	handleControls(window, data.state);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	updateTextureContext(data);

	data.vao.bind();
	setMeshContexteAndDraw(data);
	data.vao.unbind();

	glfwSwapBuffers(window);
	glfwPollEvents();
}
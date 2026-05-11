#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Renderer.hpp"
#include "scop.hpp"
#include "utils.hpp"
#include <iostream>
#include "GeometryProcessor.hpp"
#include <memory>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: ./scop <file.obj>" << std::endl;
		return 1;
	}

	if (!isValidFile(argv[1]))
	{
		std::cerr << "Error: Invalid file" << std::endl;
		return 1;
	}

	try
	{
		GLFWwindow* window = initWindow();
		if (!window)
			return 1;

		{
			scopData data = {};
			loadModelToGPU(data, argv[1]);
			
			glfwSetWindowUserPointer(window, &data);


			glActiveTexture(GL_TEXTURE0);
			if (data.activeMaterial && data.activeMaterial->texture)
				glBindTexture(GL_TEXTURE_2D, data.activeMaterial->texture->id);
			else
			{
				if (data.default_texture->id)
					glBindTexture(GL_TEXTURE_2D, data.default_texture->id);
			}
			while (!glfwWindowShouldClose(window))
				renderOBJ(window, data);
		}

		glfwDestroyWindow(window);
		glfwTerminate();
	}
	catch ( const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}


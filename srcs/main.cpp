#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "engine.hpp"
#include "scop.hpp"
#include "utils.hpp"
#include <iostream>
#include "data.hpp"

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
			setupData(data, argv[1]);
			
			glfwSetWindowUserPointer(window, &data);


			// glActiveTexture(GL_TEXTURE0);
			// if (data.activeMaterial && data.activeMaterial->texture)
			// {
			// 	std::cout << "mat: " << data.activeMaterial->name << std::endl;
			// 	glBindTexture(GL_TEXTURE_2D, data.activeMaterial->texture->id);
			// }
			// else
			// {
			// 	std::cout << "defmat" << std::endl;
			// 	Texture default_texture("./resources/assets/mou2.bmp");
			// 	std::cout << "id= " << default_texture.id << std::endl;
			// 	if (default_texture.id)
			// 		glBindTexture(GL_TEXTURE_2D, default_texture.id);
			// }
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


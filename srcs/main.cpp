#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "scop.hpp"

bool initGraphics()
{
	//.vertexCount = allSegments.size();
	SetupBuffers();
	Shader shaderProgram("./shaders/shader.vs", "./shaders/shader.fs");
	if (!shaderProgram)
        {
                glDeleteBuffers(1, &context.VBO);
                glfwTerminate();
                return false; 
        }

	return true;
}

void cleanScene()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
	glfwTerminate();
}

int main(int argc, char** argv)
{

	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <file.obj>\n";
		return 1;
	}

	try
	{
		ObjModel model = parseOBJ(argv[1]);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error : " << e.what() << "\n";
		return 1;
	}
	
	GLFWwindow* window = initWindow();
	if (!window)
		return 1;

	if (!initGraphics(window, model))
	{
		glfwTerminate();
		return 1;
	}


	// unsigned int VBO, VAO;
	// glGenVertexArrays(1, &VAO);
	// glGenBuffers(1, &VBO);

	// glBindVertexArray(VAO);
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// glEnableVertexAttribArray(1);

	// glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
		renderObj();
	// {
	// 	processInput(window);
	
	// 	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	// 	glClear(GL_COLOR_BUFFER_BIT);

	// 	float var = glfwGetTime();
	// 	var = sinf(var);
	// 	int vertexVarLocation = glGetUniformLocation(shaderProgram, "var");
	// 	glUseProgram(shaderProgram);
	// 	glUniform1f(vertexVarLocation, var);
		
	// 	glBindVertexArray(VAO);
	// 	glDrawArrays(GL_TRIANGLES, 0, 3);
		
	// 	glfwSwapBuffers(window);
	// 	glfwPollEvents();
	// }

	cleanScene();

	return 0;
}
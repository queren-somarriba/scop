#include <GL/glew.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback (GLFWwindow* window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "scop", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental=true;
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		std::cerr << "Error: " << glewGetErrorString(res) << std::endl;
		return NULL;
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	glClearColor(28.0 / 255.0, 28.0 / 255.0, 28.0 / 255.0, 1.0);

	return window;
}

void SetupBuffers(const ObjModel obj, GLuint& VAO, GLuint& VBO)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, allSegments.size() * sizeof(vec2), allSegments.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint CompileShaders()
{
	const char* pVSFileName = "shaders/shader.vs";
	const char* pFSFileName = "shaders/shader.fs";

	std::string vertexShaderSource = readFile(pVSFileName);
	if (vertexShaderSource.empty())
        {
                std::cerr << "Error: Unable to read " << pVSFileName << std::endl;
                return 0;
        }

	std::string fragmentShaderSource = readFile(pFSFileName);
	if (fragmentShaderSource.empty())
        {
                std::cerr << "Error: Unable to read " << pVSFileName << std::endl;
                return 0;
        }

	const char* fsh = fragmentShaderSource.c_str();
	const char* vsh = vertexShaderSource.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsh, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsh, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram); 

	return shaderProgram;
}

void renderScene(GLFWwindow* window, renderContext& context, const otData& data)
{
	int currentWidth, currentHeight;
	glfwGetFramebufferSize(window, &currentWidth, &currentHeight);
	glViewport(0, 0, currentWidth, currentHeight);

	if (context.currentDrawCount < context.vertexCount)
	{
		context.currentDrawCount += context.DrawSpeed;
		if (context.currentDrawCount > context.vertexCount)
		{
			context.currentDrawCount = context.vertexCount;
			if (context.isRecording && context.currentDrawCount >= context.vertexCount)
				closeRecorder(context);
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(context.shaderProgram);

	GLint u_resolutionLocaltion = glGetUniformLocation(context.shaderProgram, "u_resolution");
	glUniform2f(u_resolutionLocaltion, static_cast<float>(data.width), static_cast<float>(data.height));

	GLint u_angleLocation = glGetUniformLocation(context.shaderProgram, "u_angle");
	glUniform1f(u_angleLocation, context.u_angle);

	glBindVertexArray(context.VAO);

	glDrawArrays(GL_TRIANGLES, 0, context.currentDrawCount);

	if (context.isRecording && context.ffmpegPipe)
		record(context, currentWidth, currentHeight);

	glfwSwapBuffers(window);
	glfwPollEvents();
}
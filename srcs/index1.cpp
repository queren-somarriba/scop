#include <GL/glew.h>//essayer de charger les fonction a la main
#include "scop.hpp"
#include <cassert>

GLuint	VBO;
GLuint	IBO;
GLuint gWorldLocation;

std::string readFile(const char* fileName)
{
	std::fstream file(fileName, std::ios::in | std::ios::binary);
	
	if (file.fail())
		return ("");
	
	file.seekg(0, file.end);
	int len = file.tellg();
	file.seekg(0, file.beg);
	std::string outfile;
	outfile.resize(len);
	file.read(&outfile[0], len);
	return (outfile);
}

static void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;

	Scale += 0.01f;

	Matrix4f World = (Matrix4f){
		cosf(Scale),	0.0f,	-sinf(Scale),	0.0f,
		0.0f,			1.0f,	0.0f,			0.0f,
		sinf(Scale),	0.0f,	cosf(Scale),	0.0f,
		0.0f,			0.0f,	0.0f,			1.0f	
	};

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}

static void InitializeGlutCallBacks()
{
	glutDisplayFunc(RenderSceneCB);
	glutIdleFunc(RenderSceneCB);
}

static void CreateVertexBuffer()
{
	Vector3f Vertices[4];
	Vertices[0] = (Vector3f){-1.0f, -1.0f, 0.0f};
	Vertices[1] = (Vector3f){0.0f, -1.0f, 1.0f};
	Vertices[2] = (Vector3f){1.0f, -1.0f, 0.0f};
	Vertices[3] = (Vector3f){0.0f, 1.0f, 0.0f};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
	unsigned int Indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1 , 2};

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (!ShaderObj)
	{
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(1);
	}

	const GLchar* p[1];
	p[0] = pShaderText;

	GLint Lenghts[1];
	Lenghts[0] = (GLint)strlen(pShaderText);

	glShaderSource(ShaderObj, 1, p, Lenghts);

	glCompileShader(ShaderObj);
	
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}


static void CompileShaders()
{
	GLuint ShaderProgram = glCreateProgram();

	if (!ShaderProgram)
	{
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	const char* pVSFileName = "shader.vs";
	const char* pFSFileName = "shader.fs";

	std::string vs = readFile(pVSFileName);
	if (!vs.size())
		exit(1);
	
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	std::string fs = readFile(pFSFileName);
	if (!fs.size())
		exit(1);

	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(ShaderProgram);

	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (!Success)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glUseProgram(ShaderProgram);

	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	assert(gWorldLocation != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(1920 / 4, 1080 / 4);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Scop");
	
	InitializeGlutCallBacks();

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return (1);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	CreateVertexBuffer();
	CreateIndexBuffer();
	CompileShaders();

	glutMainLoop();

	return 0;
}
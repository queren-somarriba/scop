#include <GL/glew.h>//essayer de charger les fonction a la main
#include "scop.hpp"

GLuint VBO;
//GLint gTranslationLocation;
//GLint	gRotationLocation;
GLint	gScalingLocation;

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

// static void Scaling()
// {
// 	static float Scale = 1.0f;
// 	static float	Delta = 0.01f;

// 	Scale += Delta;
// 	if ((Scale >= 1.5f) || (Scale <= 0.5))
// 		Delta *= -1.0f;

// 	Matrix4f Scaling = (Matrix4f){
// 		Scale, 0.0f, 0.0f, 0.0f,
// 		0.0f, Scale , 0.0f, 0.0f,
// 		0.0f, 0.0f, Scale, 0.0f,
// 		0.0f, 0.0f, 0.0f, 1.0f
// 	};

// 	glUniformMatrix4fv(gScalingLocation, 1, GL_TRUE, &Scaling.m[0][0]);
// }

static void CombiningTransformations1()
{
   static float Scale = 1.5f;

    Matrix4f Scaling = (Matrix4f){
		Scale, 0.0f,  0.0f,  0.0f,
		0.0f,  Scale, 0.0f,  0.0f,
		0.0f,  0.0f,  Scale, 0.0f,
		0.0f,  0.0f,  0.0f,  1.0f
	};

    static float Loc = 0.0f;
    static float Delta = 0.01f;

    Loc += Delta;
    if ((Loc >= 0.5f) || (Loc <= -0.5f)) {
        Delta *= -1.0f;
    }

    Matrix4f Translation = (Matrix4f){
		1.0f, 0.0f, 0.0f, Loc,
		0.0f, 1.0f, 0.0f, 0.0,
		0.0f, 0.0f, 1.0f, 0.0,
		0.0f, 0.0f, 0.0f, 1.0f
	};

    //Matrix4f FinalTransform = Translation * Scaling;
    Matrix4f FinalTransform = Scaling * Translation;

    glUniformMatrix4fv(gScalingLocation, 1, GL_TRUE, &FinalTransform.m[0][0]);
}

// static void CombiningTransformationsExample2()
// {
//     static float Scale = 0.25f;

//     Matrix4f Scaling(Scale, 0.0f,  0.0f,  0.0f,
//                      0.0f,  Scale, 0.0f,  0.0f,
//                      0.0f,  0.0f,  Scale, 0.0f,
//                      0.0f,  0.0f,  0.0f,  1.0f);

//     static float AngleInRadians = 0.0f;
//     static float Delta = 0.01f;

//     AngleInRadians += Delta;

//     Matrix4f Rotation(cosf(AngleInRadians), -sinf(AngleInRadians), 0.0f, 0.0f,
//                       sinf(AngleInRadians), cosf(AngleInRadians),  0.0f, 0.0f,
//                       0.0,                  0.0f,                  1.0f, 0.0f,
//                       0.0f,                 0.0f,                  0.0f, 1.0f);

//     static float Loc = 0.5f;

//     Matrix4f Translation(1.0f, 0.0f, 0.0f, Loc,
//                          0.0f, 1.0f, 0.0f, 0.0,
//                          0.0f, 0.0f, 1.0f, 0.0,
//                          0.0f, 0.0f, 0.0f, 1.0f);

//     //Matrix4f FinalTransform = Translation * Rotation * Scaling;
//     Matrix4f FinalTransform = Rotation * Translation * Scaling;

//     glUniformMatrix4fv(gScalingLocation, 1, GL_TRUE, &FinalTransform.m[0][0]);
// }

static void RenderScenceCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// static float Scale = 0.0f;
	// Scale += 0.05;
	// static float AngleInRadians = 0.0f;
	// static float Delta = 0.01f;

	// Matrix4f Translation = (Matrix4f){
	// 	1.0f, 0.0f, 0.0f, sinf(Scale),
	// 	0.0f, 1.0f, 0.0f, 0.0f,
	// 	0.0f, 0.0f, 1.0f, 0.0f,
	// 	0.0f, 0.0f, 0.0f, 1.0f
	// };

	// glUniformMatrix4fv(gTranslationLocation, 1, GL_TRUE, &Translation.m[0][0]); 

	// AngleInRadians += Delta;
	// if ((AngleInRadians >= 1.5708f) || (AngleInRadians <= -1.5708f))
	// 	Delta *= -1.0f;

	// Matrix4f Rotation = (Matrix4f){
	// 	cosf(AngleInRadians),	-sinf(AngleInRadians),	0.0f,	0.0f,
	// 	sinf(AngleInRadians),	cosf(AngleInRadians),	0.0f,	0.0f,
	// 	0.0f,					0.0f,					1.0f,	0.0f,
	// 	0.0f,					0.0f,					0.0f,	1.0f
	// };

	// glUniformMatrix4fv(gRotationLocation, 1, GL_TRUE, &Rotation.m[0][0]);

	CombiningTransformations1();

	glBindBuffer(GL_ARRAY_BUFFER, VBO); 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);

	glutPostRedisplay();

	glutSwapBuffers();
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

	// gScaleLocation = glGetUniformLocation(ShaderProgram, "gScale");//after the link
	// if (gScaleLocation == -1)
	// {
	// 	fprintf(stderr, "Error getting inform location ig 'gScale'\n");
	// 	exit(1);
	// }

	// gTranslationLocation = glGetUniformLocation(ShaderProgram, "gTranslation");
	// if (gTranslationLocation == -1)
	// {
	// 	fprintf(stderr, "Error getting inform location ig 'gTranslation'\n");
	// 	exit(1);
	// }

	// gRotationLocation = glGetUniformLocation(ShaderProgram, "gRotation");
	// if (gRotationLocation == -1)
	// {
	// 	fprintf(stderr, "Error getting inform location ig 'gRotation'\n");
	// 	exit(1);
	// }

	gScalingLocation = glGetUniformLocation(ShaderProgram, "gScaling");
    if (gScalingLocation == -1)
	{
        printf("Error getting uniform location of 'gScaling'\n");
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
}

static void CreateVertexBuffer()
{
	Vector3f Vertices[3];
	Vertices[0] = (Vector3f){-1.0f, -1.0f, 0.0f};
	Vertices[1] = (Vector3f){1.0f, -1.0f, 0.0f};
	Vertices[2] = (Vector3f){0.0f, 1.0f, 0.0f};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);

	int	width = 1920 / 4;
	int	height = 1080 / 4;
	glutInitWindowSize(width, height);

	glutInitWindowPosition(200, 100);
	glutCreateWindow("Scop");
	
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return (1);
	}

	GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
	glClearColor(Red, Green, Blue, Alpha);

	CreateVertexBuffer();

	CompileShaders();

	glutDisplayFunc(RenderScenceCB);

	glutMainLoop();

	return 0;
}
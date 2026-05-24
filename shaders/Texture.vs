#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
flat out vec3 FaceColor;
out vec3 FragPosLocal;

uniform float radius;
uniform mat4 model;
uniform mat4 invModelMat;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	FragPosLocal = aPos / radius * 2.0;
	FaceColor = aColor;

	FragPos = vec3(model * vec4(aPos, 1.0));

	Normal = mat3(transpose(mat3(invModelMat))) * aNormal;

	gl_Position = projection * view * vec4(FragPos, 1.0);

	TexCoord = aTexCoord;
}
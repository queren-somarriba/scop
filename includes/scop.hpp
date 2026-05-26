#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <stdexcept>
#include "mat4f.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include <memory>


/* STRUCTURES */
struct Vec2
{
	float x, y;
};

struct Material
{
	std::string name;

	vect4f Ka{0.2f, 0.2f, 0.2f};
	vect4f Kd{0.8f, 0.8f, 0.8f};
	vect4f Ks{0.f, 0.f, 0.f};
	float Ns = 32.f;
	int illum = 2;
	std::unique_ptr<Texture>	texture;
};


struct FaceVertex
{
	int pos_idx = -1;
	int uv_idx = -1;
	int normal_idx = -1;
};

struct Face
{
	std::vector<FaceVertex> vertices;
	std::string material_name;
};

struct Mesh
{
	std::vector<Face> faces;
	std::string name;
	std::string material_name;
};

struct ObjModel
{
	std::unordered_map<std::string, Material> materials;
	std::vector<Mesh>	meshes;
	std::vector<vect4f>	positions;
	std::vector<vect4f>	normals;
	std::vector<Vec2>	uvs;
	vect4f centroid{0.0f, 0.0f, 0.0f};
	float	radius = 1.f;
	int		modelVertexCount = 0;
	bool	hasUV = 0;
};

struct MeshDraw
{
	std::string	material_name;
	int			offset;
	int			count;
};

struct AppState
{
	Camera	camera;
	double	lastX;
	double	lastY;
	float	deltaTime;
	float	lastFrame;
	float	second;
	float	transitionFactor;
	float	modelRadius;
	float	movementSpeed;
	float	angleX = 0.f;
	float	angleY = 0.f;
	float	trunc;
	size_t	fpsCounter;
	bool	isRotatingY;
	bool	showTexture;
	bool	t_pressed;
	bool	space_pressed;
	bool	mouse_pressed;
	
	AppState() : 
		camera(vect4f(0.0f, 0.0f, 3.0f)), 
		deltaTime(0.0f), lastFrame(0.0f),
		transitionFactor(1.f),
		angleX(0.f), angleY(0.f),
		trunc(1.f),
		isRotatingY(true),
		showTexture(true),
		t_pressed(false), space_pressed(false),
		mouse_pressed(false) {}
};

struct scopData
{
	ObjModel					model;
	AppState					state;
	std::vector<MeshDraw>		meshDraws;
	Material*					activeMaterial;
	VAO							vao;
	std::unique_ptr<VBO>		vbo;
	std::unique_ptr<EBO>		ebo;
	std::unique_ptr<Shader>		shaderTexture;
	std::unique_ptr<Shader>		shaderLight;
	std::unique_ptr<Texture>	default_texture;
	float						noTextureNs = 32.f;
	int							vertexCount;
};


/* FUNCTIONS */
void		parseMTL(const std::string& path,
				std::unordered_map<std::string, Material>& materials);

ObjModel	parseOBJ(const std::string& path);

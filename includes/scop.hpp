#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <stdexcept>
#include "vect4f.hpp"
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

	vect4f Ka{0.2f, 0.2f, 0.2f};	// ambient color (light reflected in shadow)
	vect4f Kd{0.8f, 0.8f, 0.8f};	// diffuse color (main surface color)
	vect4f Ks{0.f, 0.f, 0.f};	// specular color (highlight color, black = no highlight)
	vect4f Ke{0.f, 0.f, 0.f};	// emissive color (self-illumination, black = no glow)
	float Ns = 32.f;		// specular exponent (shininess, higher = sharper highlight)
	int illum = 2;			// illumination model (2 = Phong: ambient + diffuse + specular)
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
	std::string name;
	std::string material_name;
	std::vector<Face> faces;
};

struct ObjModel
{
	std::vector<vect4f>	positions;
	std::vector<Vec2>	uvs;
	std::vector<vect4f>	normals;
	std::vector<Mesh>	meshes;
	std::unordered_map<std::string, Material> materials;

	vect4f centroid{0.0f, 0.0f, 0.0f};
	float	radius = 1.f;
	int		modelVertexCount;
};

struct MeshDraw
{
	std::string	material_name;
	int			offset;
	int			count;
};

struct scopData
{
	ObjModel					model;
	std::vector<MeshDraw>		meshDraws;
	Material*					activeMaterial;
	VAO							vao;
	std::unique_ptr<VBO>		vbo;
	std::unique_ptr<EBO>		ebo;
	std::unique_ptr<Shader>		shaderTexture;
	std::unique_ptr<Shader>		shaderLight;
	std::unique_ptr<Texture>	texture;
	int							vertexCount;
};

struct AppState
{
	Camera	camera;
	float	deltaTime;
	float	lastFrame;
	float	transitionFactor;
	float	movementSpeed;
	bool	showTexture;
	bool	t_pressed;
	
	AppState() : 
		camera(vect4f(0.0f, 0.0f, 3.0f)), 
		deltaTime(0.0f), lastFrame(0.0f),
		transitionFactor(0.f),
		showTexture(false),
		t_pressed(true) {}
};


/* FUNCTIONS */
void		parseMTL(const std::string& path,
				std::unordered_map<std::string, Material>& materials);

ObjModel	parseOBJ(const std::string& path);

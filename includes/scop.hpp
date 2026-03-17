#pragma once

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <math.h>

//#include <OpenGL/freeglut.h>

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <stdexcept>
#include "vect4f.hpp"
#include "mat4f.hpp"

struct Vec2
{
	float x, y;
};

struct Material
{
	std::string name;

	vect4f Ka{0.2f, 0.2f, 0.2f};
	vect4f Kd{0.8f, 0.8f, 0.8f};
	vect4f Ks{0.0f, 0.0f, 0.0f};
	vect4f Ke{0.0f, 0.0f, 0.0f};

	float Ns = 32.0f;
	float Ni = 1.0f;
	float d= 1.0f;
	int illum = 2;

	std::string map_Ka;
	std::string map_Kd;
	std::string map_Ks;
	std::string map_Ke;
	std::string map_Ns;
	std::string map_d;
	std::string map_bump;
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
	std::vector<vect4f> positions;
	std::vector<Vec2> uvs;
	std::vector<vect4f> normals;

	std::vector<Mesh> meshes;

	std::unordered_map<std::string, Material> materials;
};

void parseMTL(const std::string& path,
	std::unordered_map<std::string, Material>& materials);

ObjModel parseOBJ(const std::string& path);
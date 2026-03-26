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

	vect4f Ka{0.2f, 0.2f, 0.2f};	// ambient color (light reflected in shadow)
	vect4f Kd{0.8f, 0.8f, 0.8f};	// diffuse color (main surface color)
	vect4f Ks{0.0f, 0.0f, 0.0f};	// specular color (highlight color, black = no highlight)
	vect4f Ke{0.0f, 0.0f, 0.0f};	// emissive color (self-illumination, black = no glow)
	float Ns = 32.0f;		// specular exponent (shininess, higher = sharper highlight)
	float Ni = 1.0f;		// index of refraction (1.0 = air/no refraction)
	float d  = 1.0f;		// dissolve / opacity (1.0 = fully opaque)
	int illum = 2;			// illumination model (2 = Phong: ambient + diffuse + specular)
					// paths to associated texture maps
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

	vect4f centroid{0.0f, 0.0f, 0.0f};
};

void parseMTL(const std::string& path,
	std::unordered_map<std::string, Material>& materials);

ObjModel parseOBJ(const std::string& path);
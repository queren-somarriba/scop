#include "scop.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

static std::string trim(const std::string& str)
{
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return {};
	size_t end = str.find_last_not_of(" \t\r\n");
	
	return str.substr(start, end - start + 1);
}

static std::string parentDir(const std::string& path)
{
	auto p = std::filesystem::path(path).parent_path();

	return p.empty() ? "./" : (p.string() + "/");
}

static FaceVertex parseFaceVertex(const std::string& token)
{
	FaceVertex fv;
	std::istringstream ss(token);
	std::string part;
	int slot = 0;
	while (std::getline(ss, part, '/'))
	{
		if (!part.empty())
		{
			int idx = std::stoi(part) - 1;
			if (slot == 0)
				fv.pos_idx= idx;
			if (slot == 1)
				fv.uv_idx = idx;
			if (slot == 2)
				fv.normal_idx = idx;
		}
		++slot;
	}

	return fv;
}

void parseMTL(const std::string& path,
	std::unordered_map<std::string, Material>& materials)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cerr << "[MTL] Can't open: " << path << "\n";
		return;
	}

	Material* current = nullptr;
	std::string line;
	const std::string dir = parentDir(path);

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
			continue;

		std::istringstream ss(line);
		std::string keyword;
		ss >> keyword;

		if (keyword == "newmtl")
		{
			std::string name;
			std::getline(ss, name);
			name = trim(name);
			materials[name].name = name;
			current = &materials[name];
		}
		else if (current && keyword == "Ka")
			ss >> current->Ka.x >> current->Ka.y >> current->Ka.z;
		else if (current && keyword == "Kd")
			ss >> current->Kd.x >> current->Kd.y >> current->Kd.z;
		else if (current && keyword == "Ks")
			ss >> current->Ks.x >> current->Ks.y >> current->Ks.z;
		else if (current && keyword == "Ke")
			ss >> current->Ke.x >> current->Ke.y >> current->Ke.z;
		else if (current && keyword == "Ns")
			ss >> current->Ns;
		else if (current && keyword == "Ni")
			ss >> current->Ni;
		else if (current && keyword == "d")
			ss >> current->d;
		else if (current && keyword == "Tr")
		{
			ss >> current->d;
			current->d = 1.0f - current->d;
		}
		else if (current && keyword == "illum")
			ss >> current->illum;
		else if (current && keyword == "map_Ka")
			ss >> current->map_Ka;
		else if (current && keyword == "map_Kd")
			ss >> current->map_Kd;
		else if (current && keyword == "map_Ks")
			ss >> current->map_Ks;
		else if (current && keyword == "map_Ke")
			ss >> current->map_Ke;
		else if (current && keyword == "map_Ns")
			ss >> current->map_Ns;
		else if (current && keyword == "map_d")
			ss >> current->map_d;
		else if (current && (keyword == "map_bump" || keyword == "bump"|| keyword == "norm"))
			ss >> current->map_bump;
	}
}


ObjModel parseOBJ(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("[OBJ] Can't open : " + path);

	ObjModel model;
	const std::string dir = parentDir(path);

	model.meshes.push_back({});
	Mesh* currentMesh = &model.meshes.back();
	std::string currentMaterial;

	std::string line;
	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
			continue;

		std::istringstream ss(line);
		std::string keyword;
		ss >> keyword;

		if (keyword == "v")
		{
			vect4f p; ss >> p.x >> p.y >> p.z;
			model.positions.push_back(p);
		}
		else if (keyword == "vt")
		{
			Vec2 uv; ss >> uv.x >> uv.y;
			model.uvs.push_back(uv);
		}
		else if (keyword == "vn")
		{
			vect4f n; ss >> n.x >> n.y >> n.z;
			model.normals.push_back(n);
		}
		else if (keyword == "o" || keyword == "g")
		{
			std::string name; std::getline(ss, name); name = trim(name);
			if (!currentMesh->faces.empty())
			{
				model.meshes.push_back({});
				currentMesh = &model.meshes.back();
			}
			currentMesh->name = name;
			currentMesh->material_name = currentMaterial;
		}
		else if (keyword == "usemtl")
		{
			std::string mat; std::getline(ss, mat); mat = trim(mat);
			currentMaterial = mat;
			if (!currentMesh->faces.empty() && currentMesh->material_name != mat)
			{
				std::string savedName = currentMesh->name;
				model.meshes.push_back({});
				currentMesh = &model.meshes.back();
				currentMesh->name = savedName;
			}
			currentMesh->material_name = mat;
		}
		else if (keyword == "mtllib")
		{
			std::string mtlFile; std::getline(ss, mtlFile); mtlFile = trim(mtlFile);
			parseMTL(dir + mtlFile, model.materials);
		}
		else if (keyword == "f")
		{
			Face face;
			face.material_name = currentMaterial;
			std::string token;
			while (ss >> token)
				face.vertices.push_back(parseFaceVertex(token));

			if (face.vertices.size() == 3)
				currentMesh->faces.push_back(std::move(face));
			else
			{
				for (size_t i = 1; i + 1 < face.vertices.size(); ++i)
				{
					Face tri;
					tri.material_name = face.material_name;
					tri.vertices = { face.vertices[0],
						face.vertices[i],
						face.vertices[i + 1] };
						currentMesh->faces.push_back(std::move(tri));
				}
			}
		}
	}

	model.meshes.erase(
	std::remove_if(model.meshes.begin(), model.meshes.end(),
	[](const Mesh& m){ return m.faces.empty(); }),
	model.meshes.end());

	if (!model.positions.empty())
	{
		vect4f sum{0.0f, 0.0f, 0.0f};
		for (const vect4f& p : model.positions)
		{
			sum.x += p.x;
			sum.y += p.y;
			sum.z += p.z;
		}
		float inv = 1.0f / static_cast<float>(model.positions.size());
		model.centroid.x = sum.x * inv;
		model.centroid.y = sum.y * inv;
		model.centroid.z = sum.z * inv;
	}

	return model;
}
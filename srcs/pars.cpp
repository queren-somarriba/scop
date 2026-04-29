#include "scop.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include "utils.hpp"

namespace
{
	void computeBounds(ObjModel& model)
	{
		if (model.positions.empty())
			return;

		vect4f sum{0.f, 0.f, 0.f};
		for (const auto& p : model.positions)
		{
			sum.x += p.x;
			sum.y += p.y;
			sum.z += p.z;
		}

		float inv = 1.f / static_cast<float>(model.positions.size());
		model.centroid = { sum.x * inv, sum.y * inv, sum.z * inv };

		float maxDist = 0.f;
		for (const auto& p : model.positions)
		{
			float dx = p.x - model.centroid.x;
			float dy = p.y - model.centroid.y;
			float dz = p.z - model.centroid.z;
			float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
			maxDist = std::max(maxDist, dist);
		}

		model.radius = (maxDist > 0.f) ? maxDist : 1.f;
	}

	void pushNewMesh(ObjModel& model, Mesh*& currentMesh, const std::string& name, const std::string& material)
	{
		model.meshes.push_back({});
		currentMesh = &model.meshes.back();
		currentMesh->name = name;
		currentMesh->material_name = material;
	}

	void triangulateAndAddFace(const Face& face, Mesh& mesh)
	{
		if (face.vertices.size() == 3)
		{
			mesh.faces.push_back(face);
			return;
		}

		for (size_t i = 1; i + 1 < face.vertices.size(); ++i)
		{
			Face tri;
			tri.material_name = face.material_name;
			tri.vertices = {
				face.vertices[0],
				face.vertices[i],//
				face.vertices[i + 1]
			};
			mesh.faces.push_back(std::move(tri));
		}
	}

	FaceVertex parseFaceVertex(const std::string& token, int vertexCount)
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
				if (idx < 0)
					idx = vertexCount + idx;
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

	void assignMtlValue(std::unordered_map<std::string, Material>& materials,
						std::istringstream& ss, Material*& current, std::string& keyword)
	{
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
		else if (current && keyword == "illum")
			ss >> current->illum;
		else if (current && (keyword == "map_Ka" || keyword == "map_Ks"))
		{
			std::string texture_path;
			ss >> texture_path;
			texture_path = "resources/" + texture_path;
			current->texture = std::make_unique<Texture>(texture_path.c_str());
		}
		if (!(current->texture))
			current->texture = std::make_unique<Texture>("./resources/assets/mou2.bmp");
	}

	void assignOBJValue(std::string& line, ObjModel& model, std::string& currentMaterial,
			const std::string& dir, Mesh*& currentMesh)
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
			return;

		std::istringstream ss(line);
		std::string keyword;
		ss >> keyword;

		if (keyword == "v")
		{
			vect4f p{};
			ss >> p.x >> p.y >> p.z;
			model.positions.push_back(p);
			++model.modelVertexCount;
		}
		else if (keyword == "vt")
		{
			Vec2 uv{};
			ss >> uv.x >> uv.y;
			model.uvs.push_back(uv);
		}
		else if (keyword == "vn")
		{
			vect4f n{};
			ss >> n.x >> n.y >> n.z;
			model.normals.push_back(n);
		}
		else if (keyword == "o" || keyword == "g")
		{
			std::string name;
			std::getline(ss, name);
			name = trim(name);

			if (!currentMesh->faces.empty())
				pushNewMesh(model, currentMesh, name, currentMaterial);
			else
			{
				currentMesh->name = name;
				currentMesh->material_name = currentMaterial;
			}
		}
		else if (keyword == "usemtl")
		{
			std::string mat;
			std::getline(ss, mat);
			mat = trim(mat);

			if (!currentMesh->faces.empty() && currentMesh->material_name != mat)
			{
				std::string name = currentMesh->name;
				pushNewMesh(model, currentMesh, name, mat);
			}

			currentMaterial = mat;
			currentMesh->material_name = mat;
		}
		else if (keyword == "mtllib")
		{
			std::string mtlFile;
			std::getline(ss, mtlFile);
			mtlFile = trim(mtlFile);

			parseMTL(dir + mtlFile, model.materials);
		}
		else if (keyword == "f")
		{
			Face face;
			face.material_name = currentMaterial;

			std::string token;
			while (ss >> token)
				face.vertices.push_back(parseFaceVertex(token, model.modelVertexCount));

			triangulateAndAddFace(face, *currentMesh);
		}
	}
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

		assignMtlValue(materials, ss, current, keyword);
	}
}

ObjModel parseOBJ(const std::string& path)
{
	std::ifstream file(path);
	if (!file)
		throw std::runtime_error("Error: Can't open : " + path);

	ObjModel model;
	const std::string dir = parentDir(path);

	model.meshes.emplace_back();
	Mesh* currentMesh = &model.meshes.back();
	std::string currentMaterial;

	std::string line;
	while (std::getline(file, line))
		assignOBJValue(line, model, currentMaterial, dir, currentMesh);

	model.meshes.erase(
		std::remove_if(model.meshes.begin(), model.meshes.end(),
			[](const Mesh& m) { return m.faces.empty(); }),
		model.meshes.end()
	);

	computeBounds(model);

	return model;
}
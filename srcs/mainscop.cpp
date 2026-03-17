#include "scop.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <file.obj>\n";
		return 1;
	}

	try
	{
	ObjModel model = parseOBJ(argv[1]);

	std::cout << "=== Geometry ===\n"
	<< " Positions : " << model.positions.size() << "\n"
	<< " UVs: " << model.uvs.size()<< "\n"
	<< " Normales : " << model.normals.size()<< "\n\n";

	std::cout << "=== Meshes (" << model.meshes.size() << ") ===\n";
	for (const auto& mesh : model.meshes)
	{
		std::cout << " [" << (mesh.name.empty() ? "(sans nom)" : mesh.name)
			<< "] mat=" << (mesh.material_name.empty() ? "-" : mesh.material_name)
			<< " triangles=" << mesh.faces.size() << "\n";
	}

	std::cout << "\n=== Materials (" << model.materials.size() << ") ===\n";
	for (const auto& [name, mat] : model.materials)
	{
		std::cout << " " << name << "\n"
		<< "Kd = (" << mat.Kd.x << ", " << mat.Kd.y << ", " << mat.Kd.z << ")\n"
		<< "Ks = (" << mat.Ks.x << ", " << mat.Ks.y << ", " << mat.Ks.z << ")\n"
		<< "Ns = " << mat.Ns << " d = " << mat.d << "\n";
		if (!mat.map_Kd.empty())
			std::cout << "map_Kd = " << mat.map_Kd << "\n";
		if (!mat.map_bump.empty())
			std::cout << "map_bump= " << mat.map_bump << "\n";
	}

	if (!model.meshes.empty() && !model.meshes[0].faces.empty())
	{
		const Face& f = model.meshes[0].faces[0];
		std::cout << "\n=== First triangle ===\n";
		for (const FaceVertex& fv : f.vertices)
		{
			const vect4f& p = model.positions[fv.pos_idx];
			std::cout << " pos=(" << p.x << ", " << p.y << ", " << p.z << ")";
			if (fv.uv_idx >= 0)
			{
				const Vec2& uv = model.uvs[fv.uv_idx];
				std::cout << " uv=(" << uv.x << ", " << uv.y << ")";
			}
			if (fv.normal_idx >= 0)
			{
				const vect4f& n = model.normals[fv.normal_idx];
				std::cout << " n=(" << n.x << ", " << n.y << ", " << n.z << ")";
			}
			std::cout << "\n";
			}
		}

	}
	catch (const std::exception& e)
	{
		std::cerr << "Error : " << e.what() << "\n";
		return 1;
	}
	return 0;
}
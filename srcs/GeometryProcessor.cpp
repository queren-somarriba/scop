#include "scop.hpp"
#include "GeometryProcessor.hpp"
#include <vector>
#include <map>

namespace
{
	vect4f randomColor(unsigned int idx)
	{
		float r = std::fmod(idx * 0.123f, 0.8f) + 0.2f;
		float b = std::fmod(idx * 0.789f, 0.8f) + 0.2f;
		return vect4f(r, 0.f, b, 1.f);
	}

	void computeVertexData(const ObjModel& model, std::vector<float>& vertices, std::map<VertexKey, GLuint>& tmp, std::vector<GLuint>& indices, int faceIndex,
							const FaceVertex& fv, const VertexKey key)
	{
		const int STRIDE = 11;
		vect4f color;

		GLuint idx = static_cast<GLuint>(vertices.size() / STRIDE);

		tmp[key] = idx;
		indices.push_back(idx);


		const vect4f& p = model.positions[fv.pos_idx];
		vertices.push_back(p.x - model.centroid.x);
		vertices.push_back(p.y - model.centroid.y);
		vertices.push_back(p.z - model.centroid.z);

		if (fv.normal_idx >= 0 && fv.normal_idx < (int)model.normals.size())
		{
			const vect4f& n = model.normals[fv.normal_idx];
			vertices.push_back(n.x);
			vertices.push_back(n.y);
			vertices.push_back(n.z);
		}
		else
		{
			vertices.push_back(0.f);
			vertices.push_back(1.f);
			vertices.push_back(0.f);
		}

		if (fv.uv_idx >= 0 && fv.uv_idx < (int)model.uvs.size())
		{
			vertices.push_back(model.uvs[fv.uv_idx].x);
			vertices.push_back(model.uvs[fv.uv_idx].y);
		}
		else
		{
			vertices.push_back(0.f);
			vertices.push_back(0.f);
		}
		color = randomColor(faceIndex);
		vertices.push_back(color.getX());
		vertices.push_back(color.getY());
		vertices.push_back(color.getZ());
	}

	void flattenObjModel(const ObjModel& model, std::vector<float>&  vertices,
							std::vector<GLuint>& indices, std::vector<MeshDraw>& meshDraws)
	{
		vertices.clear();
		indices.clear();
		meshDraws.clear();

		std::map<VertexKey, GLuint> tmp;
		int faceIndex = 0;

		for (const Mesh& mesh : model.meshes)
		{
			MeshDraw md;
			md.offset  = static_cast<int>(indices.size());
			md.material_name = mesh.material_name;

			for (const Face& face : mesh.faces)
			{
				for (int i = 0; i < 3; ++i)
				{
					const FaceVertex& fv = face.vertices[i];
					VertexKey key{ fv.pos_idx, fv.uv_idx, fv.normal_idx };

					auto it = tmp.find(key);
					if (it != tmp.end())
						indices.push_back(it->second);
					else
						computeVertexData(model, vertices, tmp, indices, faceIndex, fv, key);
				}
				++faceIndex;
			}

		md.count = static_cast<int>(indices.size()) - md.offset;
		if (md.count > 0)
			meshDraws.push_back(md);
		}
	}

	void activateMaterial(scopData& data)
	{
		for (const Mesh& mesh: data.model.meshes)
		{
			auto it = data.model.materials.find(mesh.material_name);
			if (it != data.model.materials.end())
			{
				data.activeMaterial = &it->second;
				break;
			}
		}

		if (!data.activeMaterial && !data.model.materials.empty())
		{
			data.activeMaterial = &data.model.materials.begin()->second;
		}
	}


	void initializeBuffers(scopData& data, std::vector<float>& vertices, std::vector<GLuint>& indices)
	{
		data.vao.bind();

		data.vbo = std::make_unique<VBO>(vertices.data(), vertices.size() * sizeof(float));
		data.ebo = std::make_unique<EBO>(indices.data(), indices.size() * sizeof(GLuint));

		data.vao.linkAttrib(*data.vbo, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
		data.vao.linkAttrib(*data.vbo, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		data.vao.linkAttrib(*data.vbo, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		data.vao.linkAttrib(*data.vbo, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

		data.vao.unbind();
		data.vbo->unbind();
	}


	void computeModelNormals(ObjModel& model)
	{
		model.normals.clear();
		model.normals.resize(model.positions.size(), vect4f(0.0f, 0.0f, 0.0f));

		for (auto& mesh : model.meshes)
		{

			for (auto& face : mesh.faces)
			{
				for (int i = 1; i < (int)face.vertices.size() - 1; i++)
				{
					FaceVertex& fv0 = face.vertices[0];
					FaceVertex& fv1 = face.vertices[i];
					FaceVertex& fv2 = face.vertices[i + 1];

					vect4f p0 = model.positions[fv0.pos_idx];
					vect4f p1 = model.positions[fv1.pos_idx];
					vect4f p2 = model.positions[fv2.pos_idx];

					vect4f edge1 = p1 - p0;
					vect4f edge2 = p2 - p0;
					vect4f faceNormal = edge1.cross(edge2);

					model.normals[fv0.pos_idx] += faceNormal;
					model.normals[fv1.pos_idx] += faceNormal;
					model.normals[fv2.pos_idx] += faceNormal;

					fv0.normal_idx = fv0.pos_idx;
					fv1.normal_idx = fv1.pos_idx;
					fv2.normal_idx = fv2.pos_idx;
				}
			}
		}

		for (auto& n : model.normals)
			n = n.normalize();
	}

	void setTextureContext(scopData& data)
	{
		vect4f diffuseColor = vect4f(0.8f, 0.8f, 0.8f);
		vect4f ambientColor = diffuseColor * vect4f(0.2f, 0.2f, 0.2f);
		vect4f lightPos = data.state.camera.pos;
		
		data.shaderTexture->use();
		data.shaderTexture->setFloat("radius", data.model.radius);
		data.shaderTexture->setVec4("light.position", lightPos);
		data.shaderTexture->setVec4("light.ambient", ambientColor);
		data.shaderTexture->setVec4("light.diffuse", diffuseColor);
		data.shaderTexture->setVec4("light.specular", vect4f(1.f, 1.f, 1.f));
		data.shaderTexture->setVec4("illum.ambient", vect4f(0.2f, 0.2f, 0.2f));
		data.shaderTexture->setVec4("illum.diffuse", vect4f(0.8f, 0.8f, 0.8f));
		data.shaderTexture->setVec4("illum.specular", vect4f(1.f, 1.f, 1.f));
		data.shaderTexture->setFloat("illum.shininess", data.noTextureNs);
		data.shaderTexture->setBool("hasUV", data.model.hasUV);
	}
}

void loadModelToGPU(scopData& data, const std::string& objPath)
{
	data.model = parseOBJ(objPath);

	if (data.model.normals.empty())
		computeModelNormals(data.model);

	float fov = 45.f * (M_PI / 180.f);
	float distance = (data.model.radius / std::tan(fov * 0.5f)) * 1.5f;
	data.state.camera.pos = vect4f(0.f, 0.f, distance);

	std::vector<float> vertices;
	std::vector<GLuint> indices;
	flattenObjModel(data.model, vertices, indices, data.meshDraws);

	data.vertexCount = static_cast<int>(indices.size());
	data.state.trunc *= data.model.radius;
	data.state.modelRadius = data.model.radius;

	activateMaterial(data);
	initializeBuffers(data, vertices, indices);

	data.default_texture = std::make_unique<Texture>("assets/models/textures/vg2.bmp");
	data.shaderTexture = std::make_unique<Shader>("./shaders/Texture.vs", "./shaders/Texture.fs");

	setTextureContext(data);
}
#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Buffer.h"
#include "Shader.h"

#include "ErrorCheck.h"
namespace Hazel {
#ifndef VERTEX
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		//glm::vec3 Tangent;
		glm::vec2 Texcoord;
	};
#endif

	struct myTexture
	{
		unsigned int slot;
		std::string type;
		aiString path;
	};


	class Mesh
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<myTexture> textures;

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<myTexture> textures);
		void Draw(const std::shared_ptr<Shader>& shader);
		void DrawInstanced(const std::shared_ptr<Shader>& shader, int amount);
		unsigned int vaID, vbID, ibID;
	private:
		void SetMesh();



	};
}
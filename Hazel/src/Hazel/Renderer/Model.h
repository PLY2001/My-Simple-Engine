#pragma once
#include "Mesh.h"
#include "stb_image.h"
#include <glad/glad.h>


namespace Hazel {
	class Model
	{
	private:
		glm::vec3 Pos;
		glm::vec3 Rotate;
		std::string directory;
		
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<myTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
		unsigned int TextureFromFile(const char* path, const std::string& directory);
		std::vector<myTexture> textures_loaded;

		


	public:
		Model(std::string path, glm::vec3 Pos);
		Model(std::string path, glm::vec3 Pos, glm::vec3 Rotate);
		Model(std::string path);
		std::string m_path;
		void Draw(const std::shared_ptr<Shader>& shader);
		void DrawInstanced(const std::shared_ptr<Shader>& shader, int amount);
		glm::mat4 mModelMatrix = glm::mat4(1.0f);
		void SetPosition();
		std::vector<Mesh> meshes;
		
	};


}
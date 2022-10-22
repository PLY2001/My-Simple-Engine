#include "hzpch.h"
#include "Skybox.h"

namespace Hazel {
	Skybox::Skybox(std::string path)
	{
		std::vector<std::string> faces
		{
			"res/textures/skybox/" + path + "/posx.jpg",
			"res/textures/skybox/" + path + "/negx.jpg",
			"res/textures/skybox/" + path + "/posy.jpg",
			"res/textures/skybox/" + path + "/negy.jpg",
			"res/textures/skybox/" + path + "/posz.jpg",
			"res/textures/skybox/" + path + "/negz.jpg"
		};
		cubemapTexture = loadCubemap(faces);

	}


	unsigned int Skybox::GenBox()
	{
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		unsigned int vaID;//VertexArray
		unsigned int vbID;

		VertexArray* va = new VertexArray(vaID);
		VertexBuffer* vb = new VertexBuffer(vbID, skyboxVertices, 108 * sizeof(float));

		VertexAttribLayout layout;//�����������Բ���ʵ��
		layout.Push<GL_FLOAT>(3);//�����һ�����Բ��֣�����Ϊfloat��ÿ����Ϊ3ά����

		va->AddBuffer(vbID, layout);//���������Բ���Ӧ���ڶ��㻺����vb�������ڶ����������va��

		va->Unbind();
		vb->Unbind();

		return vaID;
	}

	void Skybox::Draw(const std::shared_ptr<Shader>& SkyShader, unsigned int SkyboxID)
	{
		SkyShader->Bind();
		glBindVertexArray(SkyboxID);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		SkyShader->SetUniform1i("skybox", 6);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		SkyShader->Unbind();
		glBindVertexArray(0);
	}

	unsigned int Skybox::loadCubemap(std::vector<std::string> faces)
	{
		stbi_set_flip_vertically_on_load(0);//���ﲻ��ֱ��ת������ת�˷����ߵ���
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cube map texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}
}
#pragma once

#include <glad/glad.h>
#include "stb_image.h"
#include "Buffer.h"
#include "Shader.h"
namespace Hazel {
	class Skybox
	{
	private:
		unsigned int loadCubemap(std::vector<std::string> faces);
	public:
		Skybox(std::string path);
		unsigned int GenBox();
		unsigned int cubemapTexture;
		void Draw(const std::shared_ptr<Shader>& ScreenShader, unsigned int SkyboxID);
	};
}
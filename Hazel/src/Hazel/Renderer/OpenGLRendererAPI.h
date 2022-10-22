#pragma once

#include "Model.h"

namespace Hazel {

	class OpenGLRendererAPI
	{
	public:
		void static SetClearColor(const glm::vec4& color);
		void static ClearDepth();
		void static ClearColor();

		void static Draw(const std::shared_ptr<Model>& model, const std::shared_ptr<Shader>& shader);
		void static DrawInstanced(const std::shared_ptr<Model>& model, const std::shared_ptr<Shader>& shader,int ModelCount);
		void static CullFace(std::string face);
	};


}
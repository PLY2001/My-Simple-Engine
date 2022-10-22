#include "hzpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Hazel {

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::ClearDepth()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::ClearColor()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLRendererAPI::Draw(const std::shared_ptr<Model>& model, const std::shared_ptr<Shader>& shader)
	{
		model->Draw(shader);
	}

	void OpenGLRendererAPI::DrawInstanced(const std::shared_ptr<Model>& model, const std::shared_ptr<Shader>& shader, int ModelCount)
	{
		model->DrawInstanced(shader,ModelCount);
	}

	void  OpenGLRendererAPI::CullFace(std::string face)
	{
		if (face == "FRONT")
			glCullFace(GL_FRONT);
		else
			glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	}

}
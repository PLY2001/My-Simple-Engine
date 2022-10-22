#pragma once
#include <glad/glad.h>
#include <iostream>
namespace Hazel {
	static void GLClearError()
	{
		while (glGetError() != GL_NO_ERROR)
		{

		}
	}

	static void GLCheckError()
	{
		while (GLenum error = glGetError())
		{
			std::cout << "[OpenGL Error](" << error << ")" << std::endl;
		}
	}
}
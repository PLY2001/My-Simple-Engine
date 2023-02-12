#pragma once
#include <string>


namespace Hazel {
	struct ShaderSource
	{
		std::string vertexShader;
		std::string geometryShader;
		std::string fragmentShader;
	};

	class Shader
	{
	private:
		std::string RendererFilePath;
		std::unordered_map<std::string, int> RendererUniformLocation;//关联容器，是以字符串为key，整型为value的map
		int geo = 0;
	public:
		unsigned int RendererID;
		Shader(const std::string& filepath);
		~Shader();

		void Bind() const;
		void Unbind() const;
		void SetUniform1f(const std::string& name, float v0);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform1fArray(const std::string& name, float* value, int size);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform1i(const std::string& name, int value);
		void SetUniformMat4(const std::string& name, glm::mat4& value);
		void SetUniformMat4Vector(const std::string& name, std::vector<glm::mat4>& value);

	private:
		int GetUniformLocation(const std::string& name);
		ShaderSource ParseSahder(const std::string& filepath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader);
	};
}
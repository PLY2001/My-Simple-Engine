#pragma once

#include "Shader.h"
#include <glad/glad.h>
//#include "Model.h"
//#include "Objects/Objects.h"

namespace Hazel {

	///////////////////////////////////////////////////VertexAttribLayout//////////////////////////////////////////////////////
	struct VertexAttribElement
	{
		unsigned int type;
		unsigned int count;
		unsigned char normalized;

		static unsigned int GetSizeOfType(unsigned int type)
		{
			switch (type)
			{
			case GL_FLOAT:return 4;
			case GL_UNSIGNED_INT: return 4;
			case GL_UNSIGNED_BYTE:return 1;
			}
		}
	};

	class VertexAttribLayout
	{
	private:
		std::vector<VertexAttribElement> RendererElements;
		unsigned int RendererStride;
	public:
		VertexAttribLayout() :RendererStride(0) {};

		template<unsigned int T>
		void Push(unsigned int count)
		{
			RendererElements.push_back({ T,count,GL_FALSE });
			RendererStride += VertexAttribElement::GetSizeOfType(T) * count;
		}

		inline const std::vector<VertexAttribElement> GetElements() const { return RendererElements; }
		inline unsigned int GetStride() const { return RendererStride; }
	};
	

	///////////////////////////////////////////////////InstanceBuffer//////////////////////////////////////////////////////
	class InstanceBuffer
	{
	private:
		unsigned int RendererID;
	public:
		InstanceBuffer(unsigned int size, const glm::mat4* data);
		void AddInstanceBuffermat4(unsigned int vaID, unsigned int AttribIndex);
		void SetDatamat4(unsigned int size, const glm::mat4* data);
	};


	///////////////////////////////////////////////////UniformBuffer//////////////////////////////////////////////////////
	class UniformBuffer
	{
	private:
		unsigned int RendererID;
		unsigned int BindPoint;
	public:
		UniformBuffer(unsigned int size, unsigned int BindPoint);
		void Bind(std::vector<int>& shaderIDs, const char* name);
		void SetDatamat4(unsigned int offset, unsigned int size, const glm::mat4* data);
	};

	///////////////////////////////////////////////////VertexArray//////////////////////////////////////////////////////
	class VertexArray
	{
	private:
		unsigned int& RendererID;
	public:
		VertexArray(unsigned int& RendererID);
		~VertexArray();

		void AddBuffer(const unsigned int& vbID, const VertexAttribLayout& layout);

		void Bind() const;
		void Unbind() const;
	};





	///////////////////////////////////////////////////VertexBuffer//////////////////////////////////////////////////////
#define VERTEX
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		//glm::vec3 Tangent;
		glm::vec2 Texcoord;
	};

	struct SimpleVertex
	{
		glm::vec3 Position;
		glm::vec2 Texcoord;
	};

	struct FullVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec2 Texcoord;
	};


	class VertexBuffer
	{
	private:
		unsigned int RendererID;
	public:
		VertexBuffer(unsigned int& RendererID, const Vertex* data, unsigned int size);
		VertexBuffer(unsigned int& RendererID, const float* data, unsigned int size);
		VertexBuffer(unsigned int& RendererID, const glm::vec3* data, unsigned int size);
		VertexBuffer(unsigned int& RendererID, const SimpleVertex* data, unsigned int size);
		~VertexBuffer();
		void Bind() const;
		void Unbind() const;

	};

	/////////////////////////////////////////////////////FrameBuffer////////////////////////////////////////////////////
	class FrameBuffer
	{
	private:
		unsigned int fbID;//FrameBuffer
		unsigned int tbID;//TextureBuffer
		unsigned int rbID;//RenderBuffer
		unsigned int WinWidth;
		unsigned int WinHeight;
		unsigned int mSamples;
		std::unique_ptr<VertexArray> va;
		std::unique_ptr <VertexBuffer> vb;

	public:
		FrameBuffer(unsigned int WinWidth, unsigned int WinHeight);
		void GenTexture2D();
		void GenTexture2DMultiSample(unsigned int samples);
		void GenTexture2DShadowMap();
		void GenTexture2DShadowCubeMap();
		~FrameBuffer();
		unsigned int GenQuad();
		void Bind() const;
		void Unbind() const;
		void Draw(const std::shared_ptr<Shader>& ScreenShader, unsigned int vaID);
		void GetColorAfterMSAA(unsigned int framebufferMSAA);
		void ShowColorAfterMSAA(unsigned int framebufferMSAA);
		inline unsigned int GetID() { return fbID; }
		inline unsigned int GetTexID() { return tbID; }
		void ResetWindow(unsigned int Width, unsigned int Height);
		void ResetWindowMultiSample(unsigned int Width, unsigned int Height);
		void ResetWindowCameraMap(unsigned int Width, unsigned int Height);
	};

	///////////////////////////////////////////////////IndexBuffer//////////////////////////////////////////////////////
	class IndexBuffer
	{
	private:
		unsigned int& RendererID;
		unsigned int RendererCount;
	public:
		IndexBuffer(unsigned int& RendererID, const unsigned int* data, unsigned int count);
		~IndexBuffer();
		void Bind() const;
		void Unbind() const;

		inline unsigned int GetCount() const { return RendererCount; }
	};
}
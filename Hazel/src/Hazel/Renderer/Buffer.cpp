#include "hzpch.h"
#include "Buffer.h"
#include <glad/glad.h>


namespace Hazel {

	/////////////////////////////////////////////////////FrameBuffer////////////////////////////////////////////////////
	FrameBuffer::FrameBuffer(unsigned int WinWidth, unsigned int WinHeight) :WinWidth(WinWidth), WinHeight(WinHeight)
	{
		glGenFramebuffers(1, &fbID);//创建framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbID);
	}

	void FrameBuffer::GenTexture2D()
	{
		glGenTextures(1, &tbID);//创建贴图
		glBindTexture(GL_TEXTURE_2D, tbID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//设置纹理过滤方式（必须设置）
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//设置纹理过滤方式（必须设置）
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//设置纹理环绕方式（必须设置）
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//设置纹理环绕方式（必须设置）
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WinWidth, WinHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);//设置贴图为NULL
		glBindTexture(GL_TEXTURE_2D, 0);//解绑贴图

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbID, 0);//将该贴图设置为framebuffer的颜色附件

		glGenRenderbuffers(1, &rbID);//创建renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, rbID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WinWidth, WinHeight);//将深度缓冲和模板缓冲存入
		glBindRenderbuffer(GL_RENDERBUFFER, 0);//解绑renderbuffer

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbID);//将该renderbuffer设置为framebuffer的深度-模板附件

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)//检查framebuffer是否内容完整
			std::cout << "ERROR: Frame buffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑framebuffer
	}

	void FrameBuffer::GenTexture2DMultiSample(unsigned int samples)
	{
		mSamples = samples;
		glGenTextures(1, &tbID);//创建贴图
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tbID);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//设置纹理过滤方式（必须设置）
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//设置纹理过滤方式（必须设置）
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//设置纹理环绕方式（必须设置）
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//设置纹理环绕方式（必须设置）
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, WinWidth, WinHeight, GL_TRUE);//设置贴图为NULL
		glBindTexture(GL_TEXTURE_2D, 0);//解绑贴图

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tbID, 0);//将该贴图设置为framebuffer的颜色附件

		glGenRenderbuffers(1, &rbID);//创建renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, rbID);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, WinWidth, WinHeight);//将深度缓冲和模板缓冲存入
		glBindRenderbuffer(GL_RENDERBUFFER, 0);//解绑renderbuffer

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbID);//将该renderbuffer设置为framebuffer的深度-模板附件

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)//检查framebuffer是否内容完整
			std::cout << "ERROR: Frame buffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑framebuffer
	}

	void FrameBuffer::GenTexture2DShadowMap()
	{
		glGenTextures(1, &tbID);//创建贴图
		glBindTexture(GL_TEXTURE_2D, tbID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//设置纹理过滤方式（必须设置）
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//设置纹理过滤方式（必须设置）
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//设置纹理环绕方式（必须设置）
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);//设置纹理环绕方式（必须设置）
		GLfloat borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);//设置border以外的深度为1.0

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WinWidth, WinHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//设置贴图为NULL
		glBindTexture(GL_TEXTURE_2D, 0);//解绑贴图

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tbID, 0);//将该贴图设置为framebuffer的深度附件

		glDrawBuffer(GL_NONE);//为了使framebuffer完整，显式声明没有颜色缓冲
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)//检查framebuffer是否内容完整
			std::cout << "ERROR: Frame buffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑framebuffer
	}

	void FrameBuffer::GenTexture2DShadowCubeMap()
	{
		glGenTextures(1, &tbID);//创建贴图
		glBindTexture(GL_TEXTURE_CUBE_MAP, tbID);
		for (GLuint i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, WinWidth, WinHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//设置贴图为NULL
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//设置纹理过滤方式（必须设置）
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//设置纹理过滤方式（必须设置）
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//设置纹理环绕方式（必须设置）
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);//设置纹理环绕方式（必须设置）
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);//设置纹理环绕方式（必须设置）
		GLfloat borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
		glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);//设置border以外的深度为1.0


		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);//解绑贴图

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tbID, 0);//将该贴图设置为framebuffer的深度附件

		glDrawBuffer(GL_NONE);//为了使framebuffer完整，显式声明没有颜色缓冲
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)//检查framebuffer是否内容完整
			std::cout << "ERROR: Frame buffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑framebuffer
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &fbID);
	}

	void FrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbID);
	}

	void FrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::Draw(const std::shared_ptr<Shader>& ScreenShader, unsigned int vaID)
	{

		ScreenShader->Bind();

		glBindVertexArray(vaID);

		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, tbID);
		ScreenShader->SetUniform1i("screenTexture", 4);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		ScreenShader->Unbind();
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);

	}

	void FrameBuffer::GetColorAfterMSAA(unsigned int framebufferMSAA)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferMSAA);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbID);
		glBlitFramebuffer(0, 0, WinWidth, WinHeight, 0, 0, WinWidth, WinHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::ShowColorAfterMSAA(unsigned int framebufferMSAA)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferMSAA);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, WinWidth, WinHeight, 0, 0, WinWidth, WinHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::ResetWindow(unsigned int Width, unsigned int Height)
	{
		WinWidth = Width;
		WinHeight = Height;
		glBindTexture(GL_TEXTURE_2D, tbID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WinWidth, WinHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);//设置贴图为NULL
		glBindTexture(GL_TEXTURE_2D, 0);//解绑贴图

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbID, 0);//将该贴图设置为framebuffer的颜色附件

		glBindRenderbuffer(GL_RENDERBUFFER, rbID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WinWidth, WinHeight);//将深度缓冲和模板缓冲存入
		glBindRenderbuffer(GL_RENDERBUFFER, 0);//解绑renderbuffer

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbID);//将该renderbuffer设置为framebuffer的深度-模板附件

		glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑framebuffer
	}
	void FrameBuffer::ResetWindowMultiSample(unsigned int Width, unsigned int Height)
	{
		WinWidth = Width;
		WinHeight = Height;

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tbID);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mSamples, GL_RGB, WinWidth, WinHeight, GL_TRUE);//设置贴图为NULL
		glBindTexture(GL_TEXTURE_2D, 0);//解绑贴图

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tbID, 0);//将该贴图设置为framebuffer的颜色附件

		glBindRenderbuffer(GL_RENDERBUFFER, rbID);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_DEPTH24_STENCIL8, WinWidth, WinHeight);//将深度缓冲和模板缓冲存入
		glBindRenderbuffer(GL_RENDERBUFFER, 0);//解绑renderbuffer

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbID);//将该renderbuffer设置为framebuffer的深度-模板附件

		glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑framebuffer
	}


	void FrameBuffer::ResetWindowCameraMap(unsigned int Width, unsigned int Height)
	{
		WinWidth = Width;
		WinHeight = Height;
		glBindTexture(GL_TEXTURE_2D, tbID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WinWidth, WinHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//设置贴图为NULL
		glBindTexture(GL_TEXTURE_2D, 0);//解绑贴图

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tbID, 0);//将该贴图设置为framebuffer的颜色附件

		//glDrawBuffer(GL_NONE);//为了使framebuffer完整，显式声明没有颜色缓冲
		//glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑framebuffer





	}

	unsigned int FrameBuffer::GenQuad()
	{
		std::vector<SimpleVertex> quadVertices = {
			// positions // texCoords
			{glm::vec3(-1.0f,  1.0f,  0.0f),glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-1.0f, -1.0f,  0.0f),glm::vec2(0.0f, 0.0f)},
			{glm::vec3(1.0f, -1.0f,  0.0f),glm::vec2(1.0f, 0.0f)},

			{glm::vec3(-1.0f,  1.0f,  0.0f),glm::vec2(0.0f, 1.0f)},
			{glm::vec3(1.0f, -1.0f,  0.0f),glm::vec2(1.0f, 0.0f)},
			 {glm::vec3(1.0f,  1.0f,  0.0f),glm::vec2(1.0f, 1.0f)}
		};
		unsigned int vaID;//VertexArray
		unsigned int vbID;

		va.reset(new VertexArray(vaID));
		vb.reset(new VertexBuffer(vbID, quadVertices.data(), (unsigned int)quadVertices.size() * sizeof(SimpleVertex)));

		VertexAttribLayout layout;//创建顶点属性布局实例
		layout.Push<GL_FLOAT>(3);//填入第一个属性布局，类型为float，每个点为3维向量
		//layout.Push<GL_FLOAT>(3);//填入第二个属性布局，类型为float，每个点为3维向量
		layout.Push<GL_FLOAT>(2);//填入第三个属性布局，类型为float，每个点为2维向量

		va->AddBuffer(vbID, layout);//将所有属性布局应用于顶点缓冲区vb，并绑定在顶点数组对象va上

		va->Unbind();
		vb->Unbind();
		return vaID;
	}


	///////////////////////////////////////////////////IndexBuffer//////////////////////////////////////////////////////
	IndexBuffer::IndexBuffer(unsigned int& RendererID, const unsigned int* data, unsigned int count) :RendererID(RendererID), RendererCount(count)
	{
		glGenBuffers(1, &RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}
	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &RendererID);
	}
	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID);
	}
	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	///////////////////////////////////////////////////InstanceBuffer//////////////////////////////////////////////////////
	InstanceBuffer::InstanceBuffer(unsigned int size, const glm::mat4* data)
	{
		glGenBuffers(1, &RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);//预设空间
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void InstanceBuffer::AddInstanceBuffermat4(unsigned int vaID, unsigned int AttribIndex)
	{
		glBindVertexArray(vaID);

		glBindBuffer(GL_ARRAY_BUFFER, RendererID); //将实例化数组缓冲区和顶点数组对象绑定在了一起
		//mat4类型，相当于连续内存中的4个vec4类型
		glEnableVertexAttribArray(AttribIndex);
		glVertexAttribPointer(AttribIndex, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);
		glEnableVertexAttribArray(AttribIndex + 1);
		glVertexAttribPointer(AttribIndex + 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(AttribIndex + 2);
		glVertexAttribPointer(AttribIndex + 2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4) * 2));
		glEnableVertexAttribArray(AttribIndex + 3);
		glVertexAttribPointer(AttribIndex + 3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4) * 3));

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glVertexAttribDivisor(AttribIndex, 1); // tell OpenGL this is an instanced vertex attribute.
		glVertexAttribDivisor(AttribIndex + 1, 1); // tell OpenGL this is an instanced vertex attribute.
		glVertexAttribDivisor(AttribIndex + 2, 1); // tell OpenGL this is an instanced vertex attribute.
		glVertexAttribDivisor(AttribIndex + 3, 1); // tell OpenGL this is an instanced vertex attribute.

		glBindVertexArray(0);

	}

	void InstanceBuffer::SetDatamat4(unsigned int size, const glm::mat4* data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	///////////////////////////////////////////////////UniformBuffer//////////////////////////////////////////////////////
	UniformBuffer::UniformBuffer(unsigned int size, unsigned int BindPoint) :BindPoint(BindPoint)
	{
		glGenBuffers(1, &RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, RendererID);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, RendererID, 0, 2 * sizeof(glm::mat4));

	}

	void UniformBuffer::Bind(std::vector<int>& shaderIDs, const char* name)
	{
		for (int shaderID : shaderIDs)
		{
			unsigned int UniformBlockID = glGetUniformBlockIndex(shaderID, name);
			glUniformBlockBinding(shaderID, UniformBlockID, BindPoint);
		}
	}

	void UniformBuffer::SetDatamat4(unsigned int offset, unsigned int size, const glm::mat4* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, RendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

	}



	///////////////////////////////////////////////////VertexArray//////////////////////////////////////////////////////
	VertexArray::VertexArray(unsigned int& RendererID) :RendererID(RendererID)
	{
		glGenVertexArrays(1, &RendererID);
		glBindVertexArray(RendererID);
	}
	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &RendererID);
	}

	void VertexArray::AddBuffer(const unsigned int& vbID, const VertexAttribLayout& layout)
	{
		Bind();
		glBindBuffer(GL_ARRAY_BUFFER, vbID);
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (int i = 0; i < elements.size(); i++)
		{
			const auto& element = elements[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
			offset += element.count * VertexAttribElement::GetSizeOfType(element.type);
		}

	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(RendererID);

	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}


	///////////////////////////////////////////////////VertexBuffer//////////////////////////////////////////////////////
	VertexBuffer::VertexBuffer(unsigned int& RendererID, const Vertex* data, unsigned int size) :RendererID(RendererID)
	{
		glGenBuffers(1, &RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
	VertexBuffer::VertexBuffer(unsigned int& RendererID, const float* data, unsigned int size) :RendererID(RendererID)
	{
		glGenBuffers(1, &RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	VertexBuffer::VertexBuffer(unsigned int& RendererID, const SimpleVertex* data, unsigned int size) :RendererID(RendererID)
	{
		glGenBuffers(1, &RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	VertexBuffer::VertexBuffer(unsigned int& RendererID, const glm::vec3* data, unsigned int size)
	{
		glGenBuffers(1, &RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &RendererID);
	}
	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);//34359....
	}
	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}



	

}
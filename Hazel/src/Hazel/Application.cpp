#include "hzpch.h"
#include "Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Input.h"



namespace Hazel
{

	

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)//����һ����ĳ�����ĺ��������ຯ����һ�������������Ϊthis

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");//��֤����
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());//�������ڣ�����Ҳ�ǵ���
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));//���ô��ڵĻص�����ΪOnEvent�����Ǵ��ں�Application��Ψһ����ͨ��

		m_ImGuiLayer = new ImGuiLayer();//����imgui��
		PushOverlay(m_ImGuiLayer);

		//����shader
		shader.reset(new Shader("res/shaders/Basic.shader"));
		OutlineShader.reset(new Shader("res/shaders/Outline.shader"));
		ScreenPostShader.reset(new Shader("res/shaders/Postprocessing.shader"));
		TShader.reset(new Shader("res/shaders/Thresholding.shader"));
		SkyShader.reset(new Shader("res/shaders/Skybox.shader"));
		ScreenBasicShader.reset(new Shader("res/shaders/ScreenBasic.shader"));
		ShadowMapShader.reset(new Shader("res/shaders/ShadowMap.shader"));
		ShadowDrawShader.reset(new Shader("res/shaders/ShadowDraw.shader"));
		GaussianShader.reset(new Shader("res/shaders/Gaussian.shader"));
		ShadowCubeMapShader.reset(new Shader("res/shaders/ShadowCubeMap.shader"));
		ShadowCubeDrawShader.reset(new Shader("res/shaders/ShadowCubeDraw.shader"));
		BricksShader.reset(new Shader("res/shaders/Bricks.shader"));
		BloomShader.reset(new Shader("res/shaders/Bloom.shader"));
		AABBShader.reset(new Shader("res/shaders/AABB.shader"));
		ArrowShader.reset(new Shader("res/shaders/Arrow.shader"));


		//����ģ��
// 		std::ifstream source("res/models/path.txt");
// 		std::string line;
// 		std::stringstream ss;
// 		while (getline(source, line))
// 		{
// 			ss << line;
// 		}

		/*IRB120*/
		IRB120Model.reset(new Model("res/models/ABB_IRB120.obj"));//��ȡģ�ͣ�Ŀ¼�ӵ�ǰ��Ŀ��Ŀ¼��ʼ���������ɵ�exe��Ŀ¼���轫noise.jpg���Ƶ�ÿһ��ģ���Աߡ�
		irb120.reset(new ABBIRB120(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.01f,0.01f,0.01f), IRB120Model));
		irb120->InitModelMatrices();
		
		

		//����ʵ��������
		
		for (int i = 0; i < IRB120Model->meshes.size();i++)//��ģ���ж������ʱ��ÿ���������Լ��Ķ����������ID��Ҫ���ʵ�������黺��������ÿ��������������ϣ��ͱ������
		{
			insbo.push_back(NULL);
			insbo.back().reset(new InstanceBuffer(100 * sizeof(glm::mat4), NULL));
			insbo.back()->AddInstanceBuffermat4(IRB120Model->meshes[i].vaID, 3);
		}
		
		/*ƽ��*/
		plane.reset(new Model("res/models/plane.obj", glm::vec3(0.0f, 0.0f, 0.0f)));
		insboplane.reset(new InstanceBuffer(sizeof(glm::mat4), &plane->mModelMatrix));//����ʵ��������
		insboplane->AddInstanceBuffermat4(plane->meshes[0].vaID, 3);
		insboplane->SetDatamat4(sizeof(glm::mat4), &plane->mModelMatrix);
		
		/*�ƶ���ͷ*/
		ArrowModel.reset(new Model("res/models/arrow.obj", glm::vec3(0.0f, 0.0f, 0.0f)));
		arrow.reset(new Arrow(irb120->GetPos(0), glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f), ArrowModel));


		//����֡����1
		framebuffer1.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer1->GenTexture2D();
		QuadID = framebuffer1->GenQuad();//���ڻ�����ͼ���ı���
		//����֡����2
		framebuffer2.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer2->GenTexture2D();
		//����֡����MSAA
		framebufferMSAA.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebufferMSAA->GenTexture2DMultiSample(4);
		//����֡����3
		framebuffer3.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer3->GenTexture2D();
		//����֡����shadowmap
		framebufferSM.reset(new FrameBuffer(ShadowMapWidth, ShadowMapHeight));
		framebufferSM->GenTexture2DShadowMap();
		//����֡����4
		framebuffer4.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer4->GenTexture2D();
		QuadID4 = framebuffer4->GenQuad();//���ڻ�����ͼ���ı���
		//����֡����5
		framebuffer5.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer5->GenTexture2D();
		//����֡����shadowcubemap
		framebufferSCM.reset(new FrameBuffer(ShadowMapWidth, ShadowMapHeight));
		framebufferSCM->GenTexture2DShadowCubeMap();
		//����֡����6
		framebuffer6.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer6->GenTexture2D();
		//����֡����7
		framebuffer7.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer7->GenTexture2D();
		QuadID7 = framebuffer7->GenQuad();//���ڻ�����ͼ���ı���

		//������պ�
		skybox.reset(new Skybox("Church"));
		SkyboxID = skybox->GenBox();//���ڻ�����պе�������

		//��Ϊ��պ���Ҫ��������Ⱥ���
		glDepthFunc(GL_LEQUAL);

		//����Uniform�������
		ubo.reset(new UniformBuffer(2 * sizeof(glm::mat4), 0));
		std::vector<int> shaderIDs;
		shaderIDs.push_back(shader->RendererID);
		shaderIDs.push_back(OutlineShader->RendererID);
		shaderIDs.push_back(ShadowDrawShader->RendererID);
		shaderIDs.push_back(ShadowCubeDrawShader->RendererID);
		shaderIDs.push_back(AABBShader->RendererID);
		shaderIDs.push_back(ArrowShader->RendererID);
		ubo->Bind(shaderIDs, "Matrices");



		//�����ƹ�
		
		PointLight.reset(new Light(glm::vec3(0.0f, 40.0f, 0.0f)));
		DirectLight.reset(new Light(glm::vec3(40.0f, 0.0f, 0.0f)));

		camera.reset(new Camera);

		glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		
	}


	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));//�����ǰ�¼�eΪWindowCloseEvent�¼����͵���OnWindowClose����
		if(mousemode == MouseMode::Disable)
		{
			dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnCameraRotate));
			dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OnCameraZoom));
		}
		else
		{
			dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnMousePos));
			dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(OnMouseReleaseEvent));
		}
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonEvent));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
		//HZ_CORE_TRACE("{0}", e);//��ʾ�¼�
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )//����������������¼�
		{
			(*--it)->OnEvent(e);
			if (e.Handled)//һ�������һ���¼����Ͳ��ٱ���
				break;
		}
	}

	void Application::Run()
	{
		while (true)
		{
			while (m_Running)
			{
				
				//glClearColor(1, 0, 1, 1);
				//glClear(GL_COLOR_BUFFER_BIT);
				
				
				

				GLClearError();//���������Ϣ


				//��������������
				camera->KeyControl(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), deltaTime);
				
				//���Դ����

				float LightSpeed = static_cast<float>(10 * deltaTime);
				if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_I) == GLFW_PRESS)
					PointLight->Pos.z -= LightSpeed;
				if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_K) == GLFW_PRESS)
					PointLight->Pos.z += LightSpeed;
				if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_J) == GLFW_PRESS)
					PointLight->Pos.x -= LightSpeed;
				if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_L) == GLFW_PRESS)
					PointLight->Pos.x += LightSpeed;
				if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_U) == GLFW_PRESS)
					PointLight->Pos.y -= LightSpeed;
				if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_O) == GLFW_PRESS)
					PointLight->Pos.y += LightSpeed;


				
				
				//�л���ʾģʽ
// 				if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_E) == GLFW_PRESS)
// 				{
// 					FrameCount++;
// 					if (FrameCount > 5)
// 					{
// 						if (graphicmode == GraphicMode::Normal)
// 							graphicmode = GraphicMode::Outline;
// 						else
// 							graphicmode = GraphicMode::Normal;
// 						FrameCount = 0;
// 					}
// 
// 				}
				//�޸ĵƹ�λ��
				if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_R) == GLFW_PRESS)
				{
					count += 0.05f;
					DirectLight->Pos = glm::vec3(40.0f * cos(count), 40.0f * sin(count), 0.0f);
				}
// 				if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_T) == GLFW_PRESS)
// 				{
// 					FrameCount++;
// 					if (FrameCount > 5)
// 					{
// 						if (lightmode == LightMode::Direct)
// 							lightmode = LightMode::Point;
// 						else
// 							lightmode = LightMode::Direct;
// 						FrameCount = 0;
// 					}
// 				}

				//��¼ÿ֡��ʱ��
				deltaTime = (float)glfwGetTime() - lastTime;
				lastTime = (float)glfwGetTime();

				
				if(AngleChanged)
				{
					irb120->ChangeAngle(index);
					
					AngleChanged = false;
				}
				
				
				ViewMatrix = camera->SetView();
				ProjectionMatrix = camera->SetProjection((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight());

				//��model������������ʵ��������
				for (int i = 0; i < insbo.size(); i++)
				{
					insbo[i]->SetDatamat4(sizeof(glm::mat4)* irb120->GetAmount(), irb120->ModelMatrices[i].data());

				}

				//��uniform�����������view��projection��������
				ubo->SetDatamat4(0, sizeof(glm::mat4), &ViewMatrix);
				ubo->SetDatamat4(sizeof(glm::mat4), sizeof(glm::mat4), &ProjectionMatrix);

				//����ֱ���VP�任����
				glm::mat4 LightProjectionMatrix = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 200.0f);
				glm::mat4 LightViewMatrix = glm::lookAt(DirectLight->Pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

				//���õ��ԴԶƽ��
				GLfloat farplane = 200.0f;

				/* Render here */



				if (lightmode == LightMode::Direct)
				{
					//ShadowMap
					glViewport(0, 0, ShadowMapWidth, ShadowMapHeight);//�޸��ӿڳߴ�
					//����Դ������������ͷ��VP�任����M����Ȼ�������ø�����ġ�

					ShadowMapShader->Bind();
					ShadowMapShader->SetUniformMat4("view", LightViewMatrix);
					ShadowMapShader->SetUniformMat4("projection", LightProjectionMatrix);

					framebufferSM->Bind();//��֡������󣬽������
					OpenGLRendererAPI::ClearDepth();//ֻ�������ȣ����������ɫ
					OpenGLRendererAPI::DrawInstanced(IRB120Model,ShadowMapShader, irb120->GetAmount());//������ҪͶ����Ӱ������
					ShadowMapShader->Unbind();
					framebufferSM->Unbind();

					glViewport(0, 0, s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight());//��ԭ�ӿڳߴ�
				}

				if (lightmode == LightMode::Point)
				{
					//���Դ������Ӱ
							//ShadowCubeMap
					glViewport(0, 0, ShadowMapWidth, ShadowMapHeight);//�޸��ӿڳߴ�
					//����VP�任����
					GLfloat aspect = (GLfloat)ShadowMapWidth / (GLfloat)ShadowMapHeight;
					GLfloat nearplane = 1.0f;

					glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearplane, farplane);//ͶӰ�任Ϊ͸�ӱ任��fov��Ϊ90��
					std::vector<glm::mat4> shadowTransforms = PointLight->GetShadowTransformsPoint(shadowProj);//�ӽǱ任Ҫ��6����Ӧ����ı任����
					//��uniform��������shader
					ShadowCubeMapShader->Bind();
					for (int i = 0; i < 6; i++)
					{
						std::stringstream sss;
						std::string index;
						sss << i;
						index = sss.str();
						ShadowCubeMapShader->SetUniformMat4(("shadowMatrices[" + index + "]").c_str(), shadowTransforms[i]);//uniformΪ����ʱ��Ҫѭ������
					}
					ShadowCubeMapShader->SetUniform3f("lightPos", PointLight->Pos.x, PointLight->Pos.y, PointLight->Pos.z);//���Դ����
					ShadowCubeMapShader->SetUniform1f("far_plane", farplane);//Զƽ�����

					framebufferSCM->Bind();//��֡������󣬽������
					OpenGLRendererAPI::ClearDepth();//ֻ�������ȣ����������ɫ

					OpenGLRendererAPI::DrawInstanced(IRB120Model,ShadowCubeMapShader, irb120->GetAmount());//������ҪͶ����Ӱ������
					ShadowCubeMapShader->Unbind();
					framebufferSCM->Unbind();

					glViewport(0, 0, s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight());//��ԭ�ӿڳߴ�
				}



				//first pass
				framebufferMSAA->Bind();//ʹ�ô�����ݵ�֡����
				OpenGLRendererAPI::SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				OpenGLRendererAPI::ClearColor();
				OpenGLRendererAPI::ClearDepth();
				//glEnable(GL_DEPTH_TEST);//zbuffer		

				if (graphicmode == GraphicMode::Normal)
				{
					//render stuff
					//���
					/*
					renderer.CullFace("FRONT");
					modelmesh.DrawInstanced(OutlineShader,ModelCount);
					*/
				}


				//second pass
				//render stuff

				///////////////////////////////////////
				
				float XMin = irb120->GetAABBMinPos(index).x;
				float XMax = irb120->GetAABBMaxPos(index).x;
				float YMin = irb120->GetAABBMinPos(index).y;
				float YMax = irb120->GetAABBMaxPos(index).y;
				float ZMin = irb120->GetAABBMinPos(index).z;
				float ZMax = irb120->GetAABBMaxPos(index).z;

				float AABBVertices[] = {
					// positions          
					XMin, YMax, ZMin,
					XMin, YMin, ZMin,
					XMax, YMin, ZMin,
					XMax, YMin, ZMin,
					XMax, YMax, ZMin,
					XMin, YMax, ZMin,

					XMin, YMin, ZMax,
					XMin, YMin, ZMin,
					XMin, YMax, ZMin,
					XMin, YMax, ZMin,
					XMin, YMax, ZMax,
					XMin, YMin, ZMax,

					XMax, YMin, ZMin,
					XMax, YMin, ZMax,
					XMax, YMax, ZMax,
					XMax, YMax, ZMax,
					XMax, YMax, ZMin,
					XMax, YMin, ZMin,

					XMin, YMin, ZMax,
					XMin, YMax, ZMax,
					XMax, YMax, ZMax,
					XMax, YMax, ZMax,
					XMax, YMin, ZMax,
					XMin, YMin, ZMax,

					XMin, YMax, ZMin,
					XMax, YMax, ZMin,
					XMax, YMax, ZMax,
					XMax, YMax, ZMax,
					XMin, YMax, ZMax,
					XMin, YMax, ZMin,

					XMin, YMin, ZMin,
					XMin, YMin, ZMax,
					XMax, YMin, ZMin,
					XMax, YMin, ZMin,
					XMin, YMin, ZMax,
					XMax, YMin, ZMax
				};
				unsigned int vaID;//VertexArray
				unsigned int vbID;

				VertexArray va(vaID);
				VertexBuffer vb(vbID, AABBVertices, 108 * sizeof(float));

				VertexAttribLayout layout;//�����������Բ���ʵ��
				layout.Push<GL_FLOAT>(3);//�����һ�����Բ��֣�����Ϊfloat��ÿ����Ϊ3ά����

				va.AddBuffer(vbID, layout);//���������Բ���Ӧ���ڶ��㻺����vb�������ڶ����������va��

				va.Unbind();
				vb.Unbind();

				AABBShader->Bind();
				va.Bind();
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				AABBShader->Unbind();
				va.Unbind();
				/////////////////////////////////////

				//OpenGLRendererAPI::Draw(Arrow, ArrowShader);

				//OpenGLRendererAPI::CullFace("BACK");

				shader->Bind();

				//��shader���͵ƹ�λ�ú����λ��
				if (lightmode == LightMode::Direct)
					shader->SetUniform4f("u_LightPosition", 100.0f * DirectLight->Pos.x, 100.0f * DirectLight->Pos.y, 100.0f * DirectLight->Pos.z, 1.0f);
				else
					shader->SetUniform4f("u_LightPosition", PointLight->Pos.x, PointLight->Pos.y, PointLight->Pos.z, 1.0f);
				shader->SetUniform4f("u_CameraPosition", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z, 1.0f);
				//��shader������ͼ��Ԫ����
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemapTexture);
				shader->SetUniform1i("skybox", 5);

				OpenGLRendererAPI::DrawInstanced(IRB120Model,shader, irb120->GetAmount());
				if (graphicmode == GraphicMode::Normal)
				{
					OpenGLRendererAPI::Draw(plane,shader);//�������飿
				}

				shader->Unbind();


				

				if (graphicmode == GraphicMode::Outline)
				{
					//third pass
					//framebuffer stuff
					framebuffer1->GetColorAfterMSAA(framebufferMSAA->GetID());//�ӿ����֡�����ȡ��ɫ����������

					framebuffer2->Bind();
					OpenGLRendererAPI::SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					OpenGLRendererAPI::ClearColor();
					framebuffer1->Draw(TShader, QuadID);//���ƶ�ֵ��

					//forth pass
					//framebuffer stuff
					framebuffer1->Unbind();
					framebuffer2->Draw(ScreenPostShader, QuadID);//��ֵ���󣬻��Ʊ�Ե���
				}
				else
				{


					//��պ�
					//third pass
					SkyShader->Bind();

					glm::mat4 SkyboxViewMatrix = glm::mat4(glm::mat3(camera->SetView()));//��λ��ǰ�������Ǹ���С�ķ��飬ȥ��λ�ƺ󣬷����ʼ����������ϣ��������۶�����պ���
					SkyShader->SetUniformMat4("view", SkyboxViewMatrix);
					SkyShader->SetUniformMat4("projection", ProjectionMatrix);
					skybox->Draw(SkyShader, SkyboxID);

					//framebufferMSAA.ShowColorAfterMSAA(framebufferMSAA.GetID());//�ӿ����֡�����ȡ��ɫ��Ĭ��֡����0����ֱ����ʾ
					framebuffer3->GetColorAfterMSAA(framebufferMSAA->GetID());//�ӿ����֡�����ȡ��ɫ����������
					framebuffer3->Unbind();
					framebuffer3->Draw(ScreenBasicShader, QuadID);



					//forth pass
					//������Ӱ
					if (lightmode == LightMode::Direct)
					{
						ShadowDrawShader->Bind();

						glActiveTexture(GL_TEXTURE7);
						glBindTexture(GL_TEXTURE_2D, framebufferSM->GetTexID());
						ShadowDrawShader->SetUniform1i("shadowmap", 7);

						ShadowDrawShader->SetUniformMat4("view", LightViewMatrix);
						ShadowDrawShader->SetUniformMat4("projection", LightProjectionMatrix);

						framebuffer4->Bind();
						OpenGLRendererAPI::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
						OpenGLRendererAPI::ClearColor();
						OpenGLRendererAPI::ClearDepth();

						OpenGLRendererAPI::DrawInstanced(IRB120Model,ShadowDrawShader, irb120->GetAmount());
						OpenGLRendererAPI::Draw(plane,ShadowDrawShader);


						ShadowDrawShader->Unbind();
					}


					//���Դ������Ӱ
					if (lightmode == LightMode::Point)
					{
						ShadowCubeDrawShader->Bind();

						glActiveTexture(GL_TEXTURE8);
						glBindTexture(GL_TEXTURE_CUBE_MAP, framebufferSCM->GetTexID());
						ShadowCubeDrawShader->SetUniform1i("shadowcubemap", 8);

						ShadowCubeDrawShader->SetUniform3f("lightPos", PointLight->Pos.x, PointLight->Pos.y, PointLight->Pos.z);
						ShadowCubeDrawShader->SetUniform1f("far_plane", farplane);

						framebuffer4->Bind();
						OpenGLRendererAPI::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
						OpenGLRendererAPI::ClearColor();
						OpenGLRendererAPI::ClearDepth();
						
						OpenGLRendererAPI::DrawInstanced(IRB120Model, ShadowCubeDrawShader, irb120->GetAmount());
						OpenGLRendererAPI::Draw(plane, ShadowCubeDrawShader);


						ShadowCubeDrawShader->Unbind();
					}


					//bloom
// 					framebuffer6->GetColorAfterMSAA(framebufferMSAA->GetID());
// 					framebuffer7->Bind();
// 					framebuffer6->Draw(BloomShader, QuadID7);
// 
// 					framebuffer7->Unbind();
// 					glEnable(GL_BLEND);
// 					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 					framebuffer7->Draw(GaussianShader, QuadID7);
// 					glDisable(GL_BLEND);

					//fifth pass
					//����Ӱ��˹�˲�
					//framebuffer4.Unbind();

					
					framebuffer5->Bind();
					OpenGLRendererAPI::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
					OpenGLRendererAPI::ClearColor();


					framebuffer4->Draw(ScreenBasicShader, QuadID4);

					//sixth pass
					//����Ӱ������ԭͼ��
					framebuffer5->Unbind();
					//renderer.ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					framebuffer5->Draw(GaussianShader, QuadID4);
					glDisable(GL_BLEND);

					if(Choosed)
					{
						arrow->ChangePos(irb120->GetPos(index));
						glDisable(GL_DEPTH_TEST);
						ArrowShader->Bind();
						ArrowShader->SetUniformMat4("model", arrow->GetModelMatrix());
						if (ToMove)
						{
							ArrowShader->SetUniform1f("Clicked", 1.0f);
						}
						else
						{
							ArrowShader->SetUniform1f("Clicked", 0.0f);
						}
						OpenGLRendererAPI::Draw(ArrowModel, ArrowShader);
						ArrowShader->Unbind();
						glEnable(GL_DEPTH_TEST);
					}

					



				}


				


				for (Layer* layer : m_LayerStack)//�������������ʾ��ͨ��
					layer->OnUpdate();

				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)//�������������ʾImgui��
					layer->OnImGuiRender();
				m_ImGuiLayer->End();


				m_Window->OnUpdate();//SwapBuffers��PollEvent
				GLCheckError();//��ȡ������Ϣ	

				


			}
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		//glfwSetWindowShouldClose((GLFWwindow*)s_Instance->GetWindow().GetNativeWindow(), true);
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		int WinWidth = e.GetWidth();
		int WinHeight = e.GetHeight();
		glViewport(0, 0, WinWidth, WinHeight);//�ӿڱ任
		framebuffer1->ResetWindow(WinWidth, WinHeight);
		framebuffer2->ResetWindow(WinWidth, WinHeight);
		framebufferMSAA->ResetWindowMultiSample(WinWidth, WinHeight);
		framebuffer3->ResetWindow(WinWidth, WinHeight);
		framebuffer4->ResetWindow(WinWidth, WinHeight);
		framebuffer5->ResetWindow(WinWidth, WinHeight);
		framebuffer6->ResetWindow(WinWidth, WinHeight);
		framebuffer7->ResetWindow(WinWidth, WinHeight);
		return true;
	}

	bool Application::OnCameraRotate(MouseMovedEvent& e)
	{
		camera->MouseControl(e.GetX(), e.GetY());
		return true;
	}

	bool Application::OnCameraZoom(MouseScrolledEvent& e)
	{
		camera->ScrollControl(e.GetXOffset(), e.GetYOffset());
		return true;
	}



	bool Application::OnMouseButtonEvent(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == HZ_MOUSE_BUTTON_RIGHT)
		{
			if(mousemode == MouseMode::Enable)
			{
				glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				mousemode = MouseMode::Disable;
				camera->firstMouse = 1;
			}
			else
			{
				glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				mousemode = MouseMode::Enable;
			}
		}
		if (mousemode == MouseMode::Enable)
		{
			Choosed = false;
			ToMove = false;
			ClickPos = glm::vec2(MousePos.x/m_Window->GetWidth()*2.0f-1.0f,MousePos.y/m_Window->GetHeight()*2.0f-1.0f);
			//ClickPos = MousePos;
			
 			for(float ClipZ = 0.0f;ClipZ<1.0f;ClipZ+=0.0001f)
 			{
 				glm::vec4 WorldClickPos = glm::inverse(ViewMatrix)*glm::inverse(ProjectionMatrix) * glm::vec4(ClickPos, ClipZ, 1.0f);
 				WorldClickPos /= WorldClickPos.w;

				
				//���irb120��ײ
				for(int i = 0; i < irb120->GetAmount(); i++)
				{
					if (WorldClickPos.x > irb120->GetAABBMinPos(i).x&& WorldClickPos.x < irb120->GetAABBMaxPos(i).x)
					{
						if (WorldClickPos.y > irb120->GetAABBMinPos(i).y&& WorldClickPos.y < irb120->GetAABBMaxPos(i).y)
						{
							if (WorldClickPos.z > irb120->GetAABBMinPos(i).z&& WorldClickPos.z < irb120->GetAABBMaxPos(i).z)
							{
								Choosed = true;
								index = i;
								break;
							}

						}

					}
				}

				//���arrow��ײ
				for (int i = 0; i < 3; i++)
				{
					if (WorldClickPos.x > arrow->GetAABBMinPos(i).x&& WorldClickPos.x < arrow->GetAABBMaxPos(i).x)
					{
						if (WorldClickPos.y > arrow->GetAABBMinPos(i).y&& WorldClickPos.y < arrow->GetAABBMaxPos(i).y)
						{
							if (WorldClickPos.z > arrow->GetAABBMinPos(i).z&& WorldClickPos.z < arrow->GetAABBMaxPos(i).z)
							{
								ToMove = true;
								Choosed = true;
								first = true;
								axis = i;
								break;
							}

						}

					}
				}
  			
 			}

		}

		return true;
	}

	bool Application::OnMousePos(MouseMovedEvent& e)
	{
		MousePos = glm::vec2(e.GetX(), m_Window->GetHeight() - e.GetY());
		if (ToMove)
		{
			ClickPos = glm::vec2(MousePos.x / m_Window->GetWidth() * 2.0f - 1.0f, MousePos.y / m_Window->GetHeight() * 2.0f - 1.0f);

			glm::vec4 irb120ScreenPos = ProjectionMatrix * ViewMatrix * glm::vec4(irb120->GetPos(index), 1.0f);
			irb120ScreenPos /= irb120ScreenPos.w;

			glm::vec4 WorldClickPos = glm::inverse(ViewMatrix) * glm::inverse(ProjectionMatrix) * glm::vec4(ClickPos, irb120ScreenPos.z, 1.0f);
			WorldClickPos /= WorldClickPos.w;

			if (first)
			{
				LastWorldClickPos = WorldClickPos;
				first = false;
			}

			if (axis == 0)
			{
				irb120->ChangePos(index, glm::vec3(WorldClickPos.x- LastWorldClickPos.x, 0.0f, 0.0f));
			}
			if (axis == 1)
			{
				irb120->ChangePos(index, glm::vec3(0.0f, WorldClickPos.y - LastWorldClickPos.y, 0.0f));
			}
			if (axis == 2)
			{
				irb120->ChangePos(index, glm::vec3(0.0f, 0.0f, WorldClickPos.z - LastWorldClickPos.z));
			}
			LastWorldClickPos = WorldClickPos;
		}
		return true;
	}

	bool Application::OnMouseReleaseEvent(MouseButtonReleasedEvent& e)
	{
		ToMove = false;
		return true;
	}
}

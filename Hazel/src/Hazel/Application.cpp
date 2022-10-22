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

		//����ģ��
		std::ifstream source("res/models/path.txt");
		std::string line;
		std::stringstream ss;
		while (getline(source, line))
		{
			ss << line;
		}

		/*����*/
		modelmesh.reset(new Model(ss.str(), glm::vec3(0.0f, 0.0f, 0.0f)));//��ȡģ�ͣ�Ŀ¼�ӵ�ǰ��Ŀ��Ŀ¼��ʼ���������ɵ�exe��Ŀ¼���轫noise.jpg���Ƶ�ÿһ��ģ���Աߡ�
	
		//�����任����
		ModelMatrices.push_back(ModelMatrix(glm::vec3(0.0f, 0.0f, 0.0f)).Matrix);
		

		//����ʵ��������
		insbo.reset(new InstanceBuffer(100 * sizeof(glm::mat4), NULL));
		for (Mesh mesh : modelmesh->meshes)//��ģ���ж������ʱ��ÿ���������Լ��Ķ����������ID��Ҫ���ʵ�������黺��������ÿ��������������ϣ��ͱ������
		{
			insbo->AddInstanceBuffermat4(mesh.vaID, 3);
		}
		
		/*ƽ��*/
		plane.reset(new Model("res/models/plane.obj", glm::vec3(0.0f, 0.0f, 0.0f)));
		insboplane.reset(new InstanceBuffer(sizeof(glm::mat4), &plane->mModelMatrix));//����ʵ��������
		insboplane->AddInstanceBuffermat4(plane->meshes[0].vaID, 3);
		insboplane->SetDatamat4(sizeof(glm::mat4) * ModelCount, ModelMatrices.data());
		
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
		ubo->Bind(shaderIDs, "Matrices");



		//�����ƹ�
		
		PointLight.reset(new Light(glm::vec3(0.0f, 40.0f, 0.0f)));
		DirectLight.reset(new Light(glm::vec3(40.0f, 0.0f, 0.0f)));

		camera.reset(new Camera);

		glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		//gltf
		gApplication = new Sample();//��ʼ��
		gApplication->Initialize();//����gltf
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
			
		}
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyEvent));
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
				
				if (GraphicMode_Normal)
				{
					graphicmode = GraphicMode::Normal;
					GraphicMode_Outline = false;
				}
				if (GraphicMode_Outline)
				{
					graphicmode = GraphicMode::Outline;
					GraphicMode_Normal = false;
				}
				if (LightMode_Direct)
				{
					lightmode = LightMode::Direct;
					LightMode_Point = false;
				}
				if (LightMode_Point)
				{
					lightmode = LightMode::Point;
					LightMode_Direct = false;
				}
				

				GLClearError();//���������Ϣ

				//gltf
				if (gApplication != 0) {
					gApplication->Update(deltaTime);//���Ŷ���
				}

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


				//����ģ��
				
					if (int Increase = ModelCount -ModelMatrices.size()>0)
					{
						for(int i=0;i<Increase;i++)
							ModelMatrices.push_back(ModelMatrix(glm::vec3((ModelCount-1) * 10.0f, 0.0f, 0.0f)).Matrix);
					}
				
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

				//���ñ任����
				for (glm::mat4& modelmatrix : ModelMatrices)
				{
					modelmatrix = glm::rotate(modelmatrix, deltaTime * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				ViewMatrix = camera->SetView();
				ProjectionMatrix = camera->SetProjection((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight());

				//��model������������ʵ��������
				insbo->SetDatamat4(sizeof(glm::mat4) * ModelCount, ModelMatrices.data());

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
					//OpenGLRendererAPI::DrawInstanced(modelmesh,ShadowMapShader, ModelCount);//������ҪͶ����Ӱ������
					gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(),camera,DirectLight);//����ģ��

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

					//OpenGLRendererAPI::DrawInstanced(modelmesh,ShadowCubeMapShader, ModelCount);//������ҪͶ����Ӱ������
					gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(),camera,PointLight);//����ģ��

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
				OpenGLRendererAPI::CullFace("BACK");

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

				//OpenGLRendererAPI::DrawInstanced(modelmesh,shader, ModelCount);
				if (lightmode == LightMode::Direct)
				{
					gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(), camera,DirectLight);//����ģ��
				}
				else
				{
					gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(), camera, PointLight);//����ģ��
				}
				if (graphicmode == GraphicMode::Normal)
				{
					OpenGLRendererAPI::Draw(plane,shader);
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

						//OpenGLRendererAPI::DrawInstanced(modelmesh,ShadowDrawShader, ModelCount);
						gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(),camera,DirectLight);//����ģ��

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
						
						//OpenGLRendererAPI::DrawInstanced(modelmesh, ShadowCubeDrawShader, ModelCount);
						gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(),camera,PointLight);//����ģ��

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



	bool Application::OnKeyEvent(KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == HZ_KEY_M)
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
		return true;
	}

}

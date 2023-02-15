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

		m_ImGuiLayer = new ImGuiLayer();//����imgui�㣬��һ��
		PushOverlay(m_ImGuiLayer);
		m_ControlLayer = new ControlLayer();//���Ƽ�ͷ�㣬�ڶ���
		PushOverlay(m_ControlLayer);


		//����shader
		shader.reset(new Shader("res/shaders/Basic.shader"));
		OutlineShader.reset(new Shader("res/shaders/Outline.shader"));
		ScreenPostShader.reset(new Shader("res/shaders/Postprocessing.shader"));
		TShader.reset(new Shader("res/shaders/Thresholding.shader"));
		SkyShader.reset(new Shader("res/shaders/Skybox.shader"));
		ScreenBasicShader.reset(new Shader("res/shaders/ScreenBasic.shader"));
		ShadowMapShader.reset(new Shader("res/shaders/ShadowMap.shader"));
		ShadowDrawShader.reset(new Shader("res/shaders/ShadowDraw.shader"));
		//ShadowDrawPlaneShader.reset(new Shader("res/shaders/ShadowDrawPlane.shader"));
		GaussianShader.reset(new Shader("res/shaders/Gaussian.shader"));
		ShadowCubeMapShader.reset(new Shader("res/shaders/ShadowCubeMap.shader"));
		ShadowCubeDrawShader.reset(new Shader("res/shaders/ShadowCubeDraw.shader"));
		BricksShader.reset(new Shader("res/shaders/Bricks.shader"));
		BloomShader.reset(new Shader("res/shaders/Bloom.shader"));
		AABBShader.reset(new Shader("res/shaders/AABB.shader"));
		//ArrowShader.reset(new Shader("res/shaders/Arrow.shader"));
		CameraDepthMapShader.reset(new Shader("res/shaders/CameraDepthMap.shader"));
		//ShadowColorMapShader.reset(new Shader("res/shaders/ShadowColorMap.shader"));
		OriginShader.reset(new Shader("res/shaders/Origin.shader"));
		PlaneShader.reset(new Shader("res/shaders/Plane.shader"));
		LightShader.reset(new Shader("res/shaders/Light.shader"));

		//����ģ��
// 		std::ifstream source("res/models/path.txt");
// 		std::string line;
// 		std::stringstream ss;
// 		while (getline(source, line))
// 		{
// 			ss << line;
// 		}
		
		insbos.reset(new InstanceBufferObjects());
		/*IRB120*/
		IRB120Model.reset(new Model("res/models/ABB_IRB120/ABB_IRB120.obj"));//��ȡģ�ͣ�Ŀ¼�ӵ�ǰ��Ŀ��Ŀ¼��ʼ���������ɵ�exe��Ŀ¼���轫noise.jpg���Ƶ�ÿһ��ģ���Աߡ�
		Belt1Model.reset(new Model("res/models/belt1/belt1.obj"));
		Belt2Model.reset(new Model("res/models/belt2/belt2.obj"));
		BeltModel.reset(new Model("res/models/belt/belt.obj"));
		AVGModel.reset(new Model("res/models/AVGcar/AVGcar.obj"));
		BoxModel.reset(new Model("res/models/box/box.obj"));
		MachineModel.reset(new Model("res/models/machine/machine.obj"));
		StorageModel.reset(new Model("res/models/storage/storage.obj"));
		modelmap.insert(std::pair<std::string, std::shared_ptr<Model>>("irb120",IRB120Model));
		modelmap.insert(std::pair<std::string, std::shared_ptr<Model>>("belt1", Belt1Model));
		modelmap.insert(std::pair<std::string, std::shared_ptr<Model>>("belt2", Belt2Model));
		modelmap.insert(std::pair<std::string, std::shared_ptr<Model>>("belt", BeltModel));
		modelmap.insert(std::pair<std::string, std::shared_ptr<Model>>("AVG", AVGModel));
		modelmap.insert(std::pair<std::string, std::shared_ptr<Model>>("box", BoxModel));
		modelmap.insert(std::pair<std::string, std::shared_ptr<Model>>("machine", MachineModel));
		modelmap.insert(std::pair<std::string, std::shared_ptr<Model>>("storage", StorageModel));
		objects.reset(new Objects(modelmap));

		/*ƽ��*/
		plane.reset(new Model("res/models/factory1/factory1.obj", glm::vec3(0.0f, 0.0f, 0.0f)));
		insboplane.reset(new InstanceBuffer(sizeof(glm::mat4), NULL));//����ʵ��������
		for (int i = 0; i < plane->meshes.size(); i++)//��ģ���ж������ʱ��ÿ���������Լ��Ķ����������ID��Ҫ���ʵ�������黺��������ÿ��������������ϣ��ͱ������
		{
			insboplane->AddInstanceBuffermat4(plane->meshes[i].vaID, 3);
			insboplane->SetDatamat4(sizeof(glm::mat4), &plane->mModelMatrix);
			
		}
		glm::mat4 lightMM = glm::mat4(1.0f);
		/*����*/
		for (int i = 0; i < 7; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				FactoryLightPos[(i * 5 + j) * 2] = LightPosX[i];
				FactoryLightPos[(i * 5 + j) * 2 + 1] = LightPosZ[j];
				lightMM = glm::mat4(1.0f);
				lightModelMatrices[i * 5 + j] = glm::translate(lightMM, glm::vec3(LightPosX[i], LightPosY, LightPosZ[j]));
			}
		}
		light.reset(new Model("res/models/light/light.obj"));
		//insbolight.reset(new InstanceBuffer(sizeof(glm::mat4)*35, lightModelMatrices));//����ʵ��������
		//insbolight->AddInstanceBuffermat4(light->meshes[0].vaID, 3);

		/*�ƶ���ͷ*/
		//ArrowModel.reset(new Model("res/models/arrow.obj", glm::vec3(0.0f, 0.0f, 0.0f)));
		//arrow.reset(new Arrow(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f), ArrowModel));

		//����֡����1
		framebuffer1.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer1->GenTexture2D();
		QuadID = framebuffer1->GenQuad();//���ڻ�����ͼ���ı���
		//����֡����2
		framebuffer2.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer2->GenTexture2D();
		//����֡����MSAA
		framebufferMSAA.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebufferMSAA->GenTexture2DMultiSample(1);
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
		//framebuffer6.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		//framebuffer6->GenTexture2D();
		//����֡����7
		//framebuffer7.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		//framebuffer7->GenTexture2D();
		//QuadID7 = framebuffer7->GenQuad();//���ڻ�����ͼ���ı���
		framebufferCM.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebufferCM->GenTexture2DShadowMap();
		//framebufferColorSM.reset(new FrameBuffer(ShadowMapWidth, ShadowMapHeight));
		//framebufferColorSM->GenTexture2D();
		//framebufferCM1.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		//framebufferCM1->GenTexture2DShadowMap();
		framebuffer6.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer6->GenTexture2D();
		QuadID6 = framebuffer6->GenQuad();//���ڻ�����ͼ���ı���

		//������պ�
		skybox.reset(new Skybox("Factory"));
		SkyboxID = skybox->GenBox();//���ڻ�����պе�������

		//��Ϊ��պ���Ҫ��������Ⱥ���
		glDepthFunc(GL_LEQUAL);

		//����Uniform�������
		ubo.reset(new UniformBuffer(2 * sizeof(glm::mat4), 0));
		shaderIDs.push_back(shader->RendererID);
		shaderIDs.push_back(OutlineShader->RendererID);
		shaderIDs.push_back(ShadowDrawShader->RendererID);
		//shaderIDs.push_back(ShadowDrawPlaneShader->RendererID);
		shaderIDs.push_back(ShadowCubeDrawShader->RendererID);
		shaderIDs.push_back(AABBShader->RendererID);
		//shaderIDs.push_back(ArrowShader->RendererID);
		shaderIDs.push_back(CameraDepthMapShader->RendererID);
		//shaderIDs.push_back(ShadowColorMapShader->RendererID);
		shaderIDs.push_back(OriginShader->RendererID);
		shaderIDs.push_back(PlaneShader->RendererID);
		shaderIDs.push_back(LightShader->RendererID);

		ubo->Bind(shaderIDs, "Matrices");



		//�����ƹ�

		PointLight.reset(new Light(glm::vec3(0.0f, 40.0f, 0.0f)));
		DirectLight.reset(new Light(glm::vec3(273.72f, 291.68f, 0.0f)));
		

		camera.reset(new Camera);

		if (mousemode == MouseMode::Enable)
		{
			glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		aabb.reset(new AABB(AABBShader));

		//hbaotexture.reset(new Texture("res/textures/hbao.png"));
		//anim.reset(new Animation(true));

		
		origin.reset(new Origin(OriginShader));

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
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )//����������������¼�
		{
			(*--it)->OnEvent(e);
			if (e.Handled)//һ�������һ���¼����Ͳ��ٱ���
				break;
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));//�����ǰ�¼�eΪWindowCloseEvent�¼����͵���OnWindowClose����
		if (mousemode == MouseMode::Disable)
		{
			dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnCameraRotate));//��������ӽ��ƶ�
			dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OnCameraZoom));//�����������
		}
		else
		{
			dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnMousePos));//��ȡ�������
			dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(OnMouseReleaseEvent));//����ͷ�ʱ�������϶�
		}
		if(!UIClicked)
		{
			dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonEvent));//�����
		}
		
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));//���������С
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressedEvent));


		//HZ_CORE_TRACE("{0}", e);//��ʾ�¼�



	}

	void Application::Run()
	{


		while (m_Running)
		{

			//glClearColor(1, 0, 1, 1);
			//glClear(GL_COLOR_BUFFER_BIT);




			GLClearError();//���������Ϣ

// 			if (mousemode == MouseMode::Disable)
// 			{
// 				glfwSetCursorPos((GLFWwindow*)s_Instance->GetWindow().GetNativeWindow(), s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight());
// 			}
			
			//��������������
			camera->KeyControl(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), deltaTime);
			//ShadowMapWidth = (int)(8192.0f / camera->GetPosition().y);
			//ShadowMapHeight = (int)(8192.0f / camera->GetPosition().y);

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

			//ֱ������
			if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_R) == GLFW_PRESS)
			{
				count += 0.05f;
				DirectLight->Pos = glm::vec3(400.0f * cos(count), 400.0f * sin(count), 0.0f);
			}

			//��¼ÿ֡��ʱ��
			deltaTime = (float)glfwGetTime() - lastTime;
			lastTime = (float)glfwGetTime();

			//���irb120�Ƕ��Ƿ�Ҫ�޸�
			if (AngleChanged)
			{
				if (objects->GetChoosedIndex() > -1)
				{
					objects->ChangeAngle();
					AngleChanged = false;
				}
			}

			//�����VP����
			ViewMatrix = camera->SetView();
			ProjectionMatrix = camera->SetProjection((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight());

			//��model������������irb120��ʵ��������
			insbos->SetDatamat4(objects);
			
			

			//��uniform����������������view��projection��������
			ubo->SetDatamat4(0, sizeof(glm::mat4), &ViewMatrix);
			ubo->SetDatamat4(sizeof(glm::mat4), sizeof(glm::mat4), &ProjectionMatrix);

			//����ֱ���VP�任����
			glm::mat4 LightProjectionMatrix = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 2000.0f);
			glm::mat4 LightViewMatrix = glm::lookAt(DirectLight->Pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			//���õ��ԴԶƽ��
			GLfloat farplane = 200.0f;

			/* Render here */


			//��ȡֱ����ӽǵ������Ϣ������framebufferSM��
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
				for (int i = 0; i < objects->GetObjectAmount(); i++)
				{
					OpenGLRendererAPI::DrawInstanced(objects->objects[i].m_Model, ShadowMapShader, objects->GetAmount(i));//������ҪͶ����Ӱ������
				}
				
				ShadowMapShader->Unbind();
				framebufferSM->Unbind();
				/*
				ShadowColorMapShader->Bind();
				ShadowColorMapShader->SetUniformMat4("view", LightViewMatrix);
				ShadowColorMapShader->SetUniformMat4("projection", LightProjectionMatrix);
				
				glActiveTexture(GL_TEXTURE11);
				glBindTexture(GL_TEXTURE_2D, framebufferSM->GetTexID());
				ShadowColorMapShader->SetUniform1i("shadowmap", 11);
				ShadowColorMapShader->SetUniform1f("bias", bias);
				ShadowColorMapShader->SetUniform4f("u_LightPosition", 100.0f * DirectLight->Pos.x, 100.0f * DirectLight->Pos.y, 100.0f * DirectLight->Pos.z, 1.0f);

				framebufferColorSM->Bind();//��֡�������
				OpenGLRendererAPI::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
				OpenGLRendererAPI::ClearColor();
				OpenGLRendererAPI::ClearDepth();
				
				for (int i = 0; i < objects->GetObjectAmount(); i++)
				{
					OpenGLRendererAPI::DrawInstanced(objects->objects[i].m_Model, ShadowColorMapShader, objects->GetAmount(i));//������ҪͶ����Ӱ������
				}

				ShadowColorMapShader->Unbind();
				framebufferColorSM->Unbind();
				*/
				glViewport(0, 0, s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight());//��ԭ�ӿڳߴ�
			}

			//��ȡ���Դ6������������Ϣ
			if (lightmode == LightMode::Point)
			{
				//ShadowCubeMap
				glViewport(0, 0, ShadowMapWidth, ShadowMapHeight);//�޸��ӿڳߴ�
				//����VP�任����
				GLfloat aspect = (GLfloat)ShadowMapWidth / (GLfloat)ShadowMapHeight;
				GLfloat nearplane = 1.0f;

				glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearplane, farplane);//ͶӰ�任Ϊ͸�ӱ任��fov��Ϊ90��
				std::vector<glm::mat4> shadowTransforms = PointLight->GetShadowTransformsPoint(shadowProj);//�ӽǱ任Ҫ��6����Ӧ����ı任����
				//��uniform��������shader
				ShadowCubeMapShader->Bind();
				ShadowCubeMapShader->SetUniformMat4Vector("shadowMatrices", shadowTransforms);
				// 					for (int i = 0; i < 6; i++)
				// 					{
				// 						std::stringstream sss;
				// 						std::string index;
				// 						sss << i;
				// 						index = sss.str();
				// 						ShadowCubeMapShader->SetUniformMat4(("shadowMatrices[" + index + "]").c_str(), shadowTransforms[i]);//uniformΪ����ʱ��Ҫѭ������
				// 					}
				ShadowCubeMapShader->SetUniform3f("lightPos", PointLight->Pos.x, PointLight->Pos.y, PointLight->Pos.z);//���Դ����
				ShadowCubeMapShader->SetUniform1f("far_plane", farplane);//Զƽ�����

				framebufferSCM->Bind();//��֡������󣬽������
				OpenGLRendererAPI::ClearDepth();//ֻ�������ȣ����������ɫ
				for (int i = 0; i < objects->GetObjectAmount(); i++)
				{
					OpenGLRendererAPI::DrawInstanced(objects->objects[i].m_Model, ShadowCubeMapShader, objects->GetAmount(i));//������ҪͶ����Ӱ������
				}
				ShadowCubeMapShader->Unbind();
				framebufferSCM->Unbind();

				glViewport(0, 0, s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight());//��ԭ�ӿڳߴ�
			}

			

			//����������Ϣ
			CameraDepthMapShader->Bind();

			framebufferCM->Bind();//��֡������󣬽������
			OpenGLRendererAPI::ClearDepth();//ֻ�������ȣ����������ɫ
			for (int i = 0; i < objects->GetObjectAmount(); i++)
			{
				OpenGLRendererAPI::DrawInstanced(objects->objects[i].m_Model, CameraDepthMapShader, objects->GetAmount(i));//������ҪͶ����Ӱ������
			}
			OpenGLRendererAPI::Draw(plane, CameraDepthMapShader);
			framebufferCM->Unbind();

			

			CameraDepthMapShader->Unbind();

			//first pass
			framebufferMSAA->Bind();//ʹ�ô�����ݵ�֡����
			OpenGLRendererAPI::SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			OpenGLRendererAPI::ClearColor();
			OpenGLRendererAPI::ClearDepth();

			//���
// 				if (graphicmode == GraphicMode::Normal)
// 				{
// 					//���					
// 					OpenGLRendererAPI::CullFace("FRONT");
// 					OpenGLRendererAPI::DrawInstanced(IRB120Model, OutlineShader, irb120->GetAmount());
// 					
// 				}


			//second pass
			//render stuff

			//������ײ��
			if (objects->GetChoosedIndex() > -1)
			{
				OpenGLRendererAPI::CullFace("DISABLE");
				aabb->Draw(objects->GetAABBMinPos(), objects->GetAABBMaxPos());


				

			}

			

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
			shader->SetUniform1f("p", basicP);
			shader->SetUniform1f("F0", F0);
			shader->SetUniform1f("fp", fp);
			
			//������ʵ����
			for (int i = 0; i < objects->GetObjectAmount(); i++)
			{
				OpenGLRendererAPI::DrawInstanced(objects->objects[i].m_Model, shader, objects->GetAmount(i));
			}

			shader->Unbind();

			PlaneShader->Bind();

			//��shader���͵ƹ�λ�ú����λ��
			
			//PlaneShader->SetUniform4f("u_LightPosition", -0.76f, 27.64f, -0.12f, 1.0f);
			
			PlaneShader->SetUniform4f("u_CameraPosition", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z, 1.0f);
			
			PlaneShader->SetUniform1fArray("u_LightPosition", FactoryLightPos, 70);
			PlaneShader->SetUniform1f("p", planeP);


			//������ʵ����
			if (graphicmode != GraphicMode::Outline)
			{
				OpenGLRendererAPI::Draw(plane, PlaneShader);
			}

			PlaneShader->Unbind();
			



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
			if (graphicmode == GraphicMode::NoShadow)
			{
				//third pass
				//framebuffer stuff
				framebuffer1->GetColorAfterMSAA(framebufferMSAA->GetID());//�ӿ����֡�����ȡ��ɫ����������
				framebuffer1->Unbind();
				OpenGLRendererAPI::SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				OpenGLRendererAPI::ClearColor();
				framebuffer1->Draw(ScreenBasicShader, QuadID);

				
			}
			if (graphicmode == GraphicMode::Normal)
			{


				//��պ�
				//third pass
				/*
				SkyShader->Bind();

				glm::mat4 SkyboxViewMatrix = glm::mat4(glm::mat3(camera->SetView()));//��λ��ǰ�������Ǹ���С�ķ��飬ȥ��λ�ƺ󣬷����ʼ����������ϣ��������۶�����պ���
				SkyShader->SetUniformMat4("view", SkyboxViewMatrix);
				SkyShader->SetUniformMat4("projection", ProjectionMatrix);
				skybox->Draw(SkyShader, SkyboxID);
				*/
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
					/*
					glActiveTexture(GL_TEXTURE12);
					glBindTexture(GL_TEXTURE_2D, framebufferColorSM->GetTexID());
					ShadowDrawShader->SetUniform1i("shadowcolormap", 12);
					*/
					glActiveTexture(GL_TEXTURE9);
					glBindTexture(GL_TEXTURE_2D, framebufferCM->GetTexID());
					ShadowDrawShader->SetUniform1i("cameramap", 9);
					//hbaotexture->Bind(10);
					//ShadowDrawShader->SetUniform1i("hbaoPos", 10);

					ShadowDrawShader->SetUniform4f("u_LightPosition", 100.0f * DirectLight->Pos.x, 100.0f * DirectLight->Pos.y, 100.0f * DirectLight->Pos.z, 1.0f);
					ShadowDrawShader->SetUniform1f("bias", bias);
					ShadowDrawShader->SetUniform1f("radius", radius);
					ShadowDrawShader->SetUniform1f("bias1", bias1);
					ShadowDrawShader->SetUniform1f("bias3", bias3);
					//ShadowDrawShader->SetUniform1f("bias4", bias4);
					ShadowDrawShader->SetUniform1f("ShadowRoundSize", ShadowRoundSize);
					ShadowDrawShader->SetUniform1f("ShadowSoftSize", ShadowSoftSize);
					ShadowDrawShader->SetUniform1f("BiasMax", BiasMax);
					ShadowDrawShader->SetUniform1f("BiasMin", BiasMin);
					ShadowDrawShader->SetUniform1f("shadowColorDepth", shadowColorDepth);
					ShadowDrawShader->SetUniform1f("hbaoShadowColorDepth", hbaoShadowColorDepth);
					ShadowDrawShader->SetUniform4f("u_CameraPosition", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z, 1.0f);
					ShadowDrawShader->SetUniformMat4("view", LightViewMatrix);
					ShadowDrawShader->SetUniformMat4("projection", LightProjectionMatrix);

					framebuffer4->Bind();
					OpenGLRendererAPI::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
					OpenGLRendererAPI::ClearColor();
					OpenGLRendererAPI::ClearDepth();
					for (int i = 0; i < objects->GetObjectAmount(); i++)
					{
						OpenGLRendererAPI::DrawInstanced(objects->objects[i].m_Model, ShadowDrawShader, objects->GetAmount(i));
					}
					OpenGLRendererAPI::Draw(plane, ShadowDrawShader);

					/*
					ShadowDrawPlaneShader->Unbind();

					ShadowDrawPlaneShader->Bind();

					glActiveTexture(GL_TEXTURE7);
					glBindTexture(GL_TEXTURE_2D, framebufferSM->GetTexID());
					ShadowDrawPlaneShader->SetUniform1i("shadowmap", 7);
					glActiveTexture(GL_TEXTURE9);
					glBindTexture(GL_TEXTURE_2D, framebufferCM->GetTexID());
					ShadowDrawPlaneShader->SetUniform1i("cameramap", 9);

					ShadowDrawPlaneShader->SetUniform4f("u_LightPosition", 100.0f * DirectLight->Pos.x, 100.0f * DirectLight->Pos.y, 100.0f * DirectLight->Pos.z, 1.0f);
					ShadowDrawPlaneShader->SetUniform1f("bias", bias);
					ShadowDrawPlaneShader->SetUniform1f("radius", radius);
					ShadowDrawPlaneShader->SetUniform1f("bias1", bias1);
					ShadowDrawPlaneShader->SetUniform4f("u_CameraPosition", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z, 1.0f);
					ShadowDrawPlaneShader->SetUniformMat4("view", LightViewMatrix);
					ShadowDrawPlaneShader->SetUniformMat4("projection", LightProjectionMatrix);

					//framebuffer4->Bind();
					//OpenGLRendererAPI::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
					//OpenGLRendererAPI::ClearColor();
					//OpenGLRendererAPI::ClearDepth();
// 					for (int i = 0; i < objects->GetObjectAmount(); i++)
// 					{
// 						OpenGLRendererAPI::DrawInstanced(objects->m_model[i], ShadowDrawShader, objects->GetAmount(i));
// 					}
					OpenGLRendererAPI::Draw(plane, ShadowDrawPlaneShader);


					ShadowDrawPlaneShader->Unbind();
					*/
				}


				//���Դ������Ӱ
				if (lightmode == LightMode::Point)
				{
					ShadowCubeDrawShader->Bind();

					glActiveTexture(GL_TEXTURE8);
					glBindTexture(GL_TEXTURE_CUBE_MAP, framebufferSCM->GetTexID());
					ShadowCubeDrawShader->SetUniform1i("shadowcubemap", 8);
					glActiveTexture(GL_TEXTURE10);
					glBindTexture(GL_TEXTURE_2D, framebufferCM->GetTexID());
					ShadowCubeDrawShader->SetUniform1i("cameramap", 10);
					ShadowCubeDrawShader->SetUniform1f("bias2", bias2);
					ShadowCubeDrawShader->SetUniform1f("radius", radius);
					ShadowCubeDrawShader->SetUniform1f("bias1", bias1);
					ShadowCubeDrawShader->SetUniform1f("bias3", bias3);
					ShadowCubeDrawShader->SetUniform4f("u_CameraPosition", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z, 1.0f);
					ShadowCubeDrawShader->SetUniform3f("lightPos", PointLight->Pos.x, PointLight->Pos.y, PointLight->Pos.z);
					ShadowCubeDrawShader->SetUniform1f("far_plane", farplane);

					framebuffer4->Bind();
					OpenGLRendererAPI::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
					OpenGLRendererAPI::ClearColor();
					OpenGLRendererAPI::ClearDepth();
					for (int i = 0; i < objects->GetObjectAmount(); i++)
					{
						OpenGLRendererAPI::DrawInstanced(objects->objects[i].m_Model, ShadowCubeDrawShader, objects->GetAmount(i));
					}
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


				if(ShadowGaussian)
				{
					//fifth pass
					//����Ӱ��˹�˲�
					framebuffer5->Bind();
					OpenGLRendererAPI::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
					OpenGLRendererAPI::ClearColor();
					framebuffer4->Draw(ScreenBasicShader, QuadID4);

					//sixth pass
					//����Ӱ������ԭͼ��
					framebuffer5->Unbind();
					//renderer.ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
					GaussianShader->Bind();
					//float widthbias = m_Window->GetWidth() / 2400.0f;
					//float heightbias = m_Window->GetHeight() / 1600.0f;
					GaussianShader->SetUniform1f("width0", width0);//* widthbias);
					GaussianShader->SetUniform1f("height0", height0);//* heightbias);
					GaussianShader->SetUniform1f("width1", width1);// * widthbias);
					GaussianShader->SetUniform1f("height1", height1);// * heightbias);
					glActiveTexture(GL_TEXTURE11);
					glBindTexture(GL_TEXTURE_2D, framebufferCM->GetTexID());
					GaussianShader->SetUniform1i("cameramap", 11);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					framebuffer5->Draw(GaussianShader, QuadID4);
					glDisable(GL_BLEND);
				}
				else
				{
					framebuffer4->Unbind();
					//renderer.ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					framebuffer4->Draw(ScreenBasicShader, QuadID4);
					glDisable(GL_BLEND);
				}

				
				

				

				//�����ƶ���ͷ
// 					if(irb120->GetChoosedIndex()>-1||Choosed)
// 					{
// 						arrow->ChangePos(irb120->GetPos());
// 						glDisable(GL_DEPTH_TEST);
// 						ArrowShader->Bind();
// 						ArrowShader->SetUniformMat4("model", arrow->GetModelMatrix());
// 						if (ToMove)
// 						{
// 							ArrowShader->SetUniform1f("Clicked", 1.0f);
// 						}
// 						else
// 						{
// 							ArrowShader->SetUniform1f("Clicked", 0.0f);
// 						}
// 						OpenGLRendererAPI::Draw(ArrowModel, ArrowShader);
// 						ArrowShader->Unbind();
// 						glEnable(GL_DEPTH_TEST);
// 					}





			}

			framebuffer6->Bind();

			OpenGLRendererAPI::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
			OpenGLRendererAPI::ClearColor();
			OpenGLRendererAPI::ClearDepth();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			LightShader->Bind();
			LightShader->SetUniform4f("u_CameraPosition", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z, 1.0f);
			glActiveTexture(GL_TEXTURE12);
			glBindTexture(GL_TEXTURE_2D, framebufferCM->GetTexID());
			LightShader->SetUniform1i("cameramap", 12);
			LightSorted.clear();
			for (int i = 0; i < 35; i++)
			{
				float dis = glm::length(camera->GetPosition() - glm::vec3(FactoryLightPos[2 * i], 27.64f, FactoryLightPos[2 * i + 1]));
				LightSorted[dis] = lightModelMatrices[i];
				
			}
			for (std::map<float, glm::mat4>::reverse_iterator it = LightSorted.rbegin(); it != LightSorted.rend(); ++it)
			{
				LightShader->SetUniformMat4("model", it->second);
				OpenGLRendererAPI::Draw(light, LightShader);
			}
			
			LightShader->Unbind();

			framebuffer6->Unbind();
			
			framebuffer6->Draw(ScreenBasicShader, QuadID6);
			glDisable(GL_BLEND);


			if (objects->GetChoosedIndex() > -1)//����ԭ��
			{
				glDisable(GL_DEPTH_TEST);
				origin->Draw();
				glEnable(GL_DEPTH_TEST);
			}

			

			for (int i = 0; i < objects->GetObjectAmount(); i++)
			{
				for (int j = 0; j < objects->GetAmount(i); j++)
				{
					if (objects->GetAnimation(i,j).Playing)
					{
						PathPoint pathpoint = objects->GetAnimation(i, j).GetPathPoint(deltaTime);
						objects->ChangePos(pathpoint.Path_Pos,i,j);//����3s
						objects->ChangeRotate(pathpoint.Path_Rotate,i,j);//����3s
						objects->ChangeHandPos(pathpoint.Path_HandPos, i, j);//����3s
						objects->ChangeHandEular(pathpoint.Path_HandEular, i, j);//����3s
						if(objects->objects[i].m_HaveAngle)
						{
							if (objects->SolveAngle(i,j))
							{
								objects->ChangeAngle(i, j);
							}
						}
					}
				}
			}
				
			


			for (Layer* layer : m_LayerStack)//�������������ʾ��ͨ��
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			if(mousemode == MouseMode::Enable)
			{	
				for (Layer* layer : m_LayerStack)//�������������ʾImgui��
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();//SwapBuffers��PollEvent
			


			GLCheckError();//��ȡ������Ϣ	

		}

	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		glfwSetWindowShouldClose((GLFWwindow*)s_Instance->GetWindow().GetNativeWindow(), GL_TRUE);
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
		//framebuffer6->ResetWindow(WinWidth, WinHeight);
		//framebuffer7->ResetWindow(WinWidth, WinHeight);
		framebufferCM->ResetWindowCameraMap(WinWidth, WinHeight);
		//framebufferCM1->ResetWindowCameraMap(WinWidth, WinHeight);
		framebuffer6->ResetWindow(WinWidth, WinHeight);

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
		//�����Ҽ�ʱ
		if (e.GetMouseButton() == HZ_MOUSE_BUTTON_RIGHT)
		{
			if (mousemode == MouseMode::Enable)//�ر�ָ��
			{
				glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				mousemode = MouseMode::Disable;
				camera->firstMouse = 1;
			}
			else//����ָ��
			{
				glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				mousemode = MouseMode::Enable;
			}
		}

		//��ָ�뿪��ʱ
		if (mousemode == MouseMode::Enable && e.GetMouseButton() == HZ_MOUSE_BUTTON_LEFT)
		{
			
			ClickPos = glm::vec2(MousePos.x / m_Window->GetWidth() * 2.0f - 1.0f, MousePos.y / m_Window->GetHeight() * 2.0f - 1.0f);

			if(!ModularCopy)
			{
				for (float ClipZ = 1.0f; ClipZ > 0.0f; ClipZ -= 0.0001f)//OpenGL������У�z��ָ���Լ���zԽ��Խ��
				{
					glm::vec4 WorldClickPos = glm::inverse(ViewMatrix) * glm::inverse(ProjectionMatrix) * glm::vec4(ClickPos, ClipZ, 1.0f);
					WorldClickPos /= WorldClickPos.w;


					//���irb120��ײ
					if (!ToMove)
					{
						for (int j = 0; j < objects->GetObjectAmount(); j++)
						{
							for (int i = 0; i < objects->GetAmount(j); i++)
							{
								if (objects->CheckCollision(j, i, WorldClickPos))
								{
									objects->SetChoosedIndex(j, i);
									//Choosed = true;
									break;
								}
							}
						}


					}


				}
			}
			else
			{
				for (float ClipZ = 1.0f; ClipZ > 0.0f; ClipZ -= 0.0001f)//OpenGL������У�z��ָ���Լ���zԽ��Խ��
				{
					glm::vec4 WorldClickPos = glm::inverse(ViewMatrix) * glm::inverse(ProjectionMatrix) * glm::vec4(ClickPos, ClipZ, 1.0f);
					WorldClickPos /= WorldClickPos.w;

					if (abs(WorldClickPos.y) < 0.1f)
					{
						glm::vec3 ObjectMinPos = objects->GetAABBMinPos();
						glm::vec3 ObjectMaxPos = objects->GetAABBMaxPos();
						if (WorldClickPos.x > ObjectMaxPos.x&& WorldClickPos.z < ObjectMaxPos.z && WorldClickPos.z > ObjectMinPos.z)
						{
							objects->AddAmount(glm::vec3(ObjectMaxPos.x, 0.0f, (ObjectMaxPos.z + ObjectMinPos.z) / 2.0f),glm::vec3(0.0f,0.0f,0.0f));
							ModularCopy = false;
						}
						if (WorldClickPos.x > ObjectMinPos.x&& WorldClickPos.x < ObjectMaxPos.x && WorldClickPos.z > ObjectMaxPos.z)
						{
							objects->AddAmount(glm::vec3((ObjectMaxPos.x + ObjectMinPos.x) / 2.0f, 0.0f, ObjectMaxPos.z), glm::vec3(0.0f, 3.0f * PI / 2.0f, 0.0f));
							ModularCopy = false;
						}
						if (WorldClickPos.x < ObjectMinPos.x&& WorldClickPos.z < ObjectMaxPos.x && WorldClickPos.z > ObjectMinPos.z)
						{
							objects->AddAmount(glm::vec3(ObjectMinPos.x, 0.0f, (ObjectMaxPos.z + ObjectMinPos.z) / 2.0f), glm::vec3(0.0f, PI, 0.0f));
							ModularCopy = false;
						}
						if (WorldClickPos.x > ObjectMinPos.x&& WorldClickPos.x < ObjectMaxPos.x && WorldClickPos.z < ObjectMinPos.z)
						{
							objects->AddAmount(glm::vec3((ObjectMaxPos.x + ObjectMinPos.x) / 2.0f, 0.0f, ObjectMinPos.z), glm::vec3(0.0f, PI / 2.0f, 0.0f));
							ModularCopy = false;
						}
					}
					


				}
			}

			return true;
		}
	}

	bool Application::OnMousePos(MouseMovedEvent& e)
	{
		
		MousePos = glm::vec2(e.GetX(), m_Window->GetHeight() - e.GetY());
		if (ToMove)
		{
			ClickPos = glm::vec2(MousePos.x / m_Window->GetWidth() * 2.0f - 1.0f, MousePos.y / m_Window->GetHeight() * 2.0f - 1.0f);

			glm::vec4 irb120ScreenPos = ProjectionMatrix * ViewMatrix * glm::vec4(objects->GetPos(), 1.0f);
			irb120ScreenPos /= irb120ScreenPos.w;

			glm::vec4 WorldClickPos = glm::inverse(ViewMatrix) * glm::inverse(ProjectionMatrix) * glm::vec4(ClickPos, irb120ScreenPos.z, 1.0f);
			WorldClickPos /= WorldClickPos.w;




			if (first)
			{
				LastWorldClickPos = WorldClickPos;
				//LastArrowPos = ArrowPos;
				first = false;
			}
			if (m_ControlLayer->GetArrowMode() == ControlLayer::ArrowMode::Trans)
			{
				if (axis == 0)
				{
					objects->ChangePos(glm::vec3(WorldClickPos.x - LastWorldClickPos.x, 0.0f, 0.0f));
					//m_ControlLayer->arrow->ChangePos(glm::vec3(ArrowPos.x-LastArrowPos.x,0.0f,0.0f));
					//m_ControlLayer->rotateArrow->ChangePos(glm::vec3(ArrowPos.x - LastArrowPos.x, 0.0f, 0.0f));

				}
				if (axis == 1)
				{
					objects->ChangePos(glm::vec3(0.0f, WorldClickPos.y - LastWorldClickPos.y, 0.0f));
					//m_ControlLayer->arrow->ChangePos(glm::vec3(0.0f,ArrowPos.y - LastArrowPos.y, 0.0f));
					//m_ControlLayer->rotateArrow->ChangePos(glm::vec3(ArrowPos.x - LastArrowPos.x, 0.0f, 0.0f));

				}
				if (axis == 2)
				{
					objects->ChangePos(glm::vec3(0.0f, 0.0f, WorldClickPos.z - LastWorldClickPos.z));
					//m_ControlLayer->arrow->ChangePos(glm::vec3(0.0f, 0.0f, ArrowPos.z - LastArrowPos.z ));
					//m_ControlLayer->rotateArrow->ChangePos(glm::vec3(ArrowPos.x - LastArrowPos.x, 0.0f, 0.0f));

				}
			}
			if (m_ControlLayer->GetArrowMode() == ControlLayer::ArrowMode::Rotat)
			{
				if (axis == 0)//y
				{
					glm::vec2 WorldClickVec = glm::normalize(glm::vec2(WorldClickPos.x - objects->GetPos().x, WorldClickPos.z - objects->GetPos().z));
					glm::vec2 LastWorldClickVec = glm::normalize(glm::vec2(LastWorldClickPos.x - objects->GetPos().x, LastWorldClickPos.z - objects->GetPos().z));
					float RotateAngle = -acos(glm::dot(WorldClickVec, LastWorldClickVec)) * (glm::cross(glm::vec3(WorldClickVec.x, 0.0f, WorldClickVec.y), glm::vec3(LastWorldClickVec.x, 0.0f, LastWorldClickVec.y)).y > 0 ? 1.0f : -1.0f);
					if (RotateAngle < 2 * PI && RotateAngle>-2 * PI)
					{
						objects->ChangeRotate(glm::vec3(0.0f, RotateAngle, 0.0f));

					}
					
				}
				if (axis == 1)//x
				{
				 	glm::vec2 WorldClickVec = glm::normalize(glm::vec2(WorldClickPos.y - objects->GetPos().y, WorldClickPos.z - objects->GetPos().z));
				 	glm::vec2 LastWorldClickVec = glm::normalize(glm::vec2(LastWorldClickPos.y - objects->GetPos().y, LastWorldClickPos.z - objects->GetPos().z));
				 	float RotateAngle = acos(glm::dot(WorldClickVec, LastWorldClickVec)) * (glm::cross(glm::vec3(WorldClickVec.x, 0.0f, WorldClickVec.y), glm::vec3(LastWorldClickVec.x, 0.0f, LastWorldClickVec.y)).y > 0 ? 1.0f : -1.0f);
					if (RotateAngle < 2 * PI && RotateAngle>-2 * PI)
					{
						objects->ChangeRotate(glm::vec3(RotateAngle, 0.0f, 0.0f));
					}
				 	//m_ControlLayer->rotateArrow->ChangeRotate(glm::vec3(RotateAngle, 0.0f, 0.0f), 0);
				 	
				}
				if (axis == 2)//z
				{
				 	glm::vec2 WorldClickVec = glm::normalize(glm::vec2(WorldClickPos.x - objects->GetPos().x, WorldClickPos.y - objects->GetPos().y));
				 	glm::vec2 LastWorldClickVec = glm::normalize(glm::vec2(LastWorldClickPos.x - objects->GetPos().x, LastWorldClickPos.y - objects->GetPos().y));
				 	float RotateAngle = acos(glm::dot(WorldClickVec, LastWorldClickVec)) * (glm::cross(glm::vec3(WorldClickVec.x, 0.0f, WorldClickVec.y), glm::vec3(LastWorldClickVec.x, 0.0f, LastWorldClickVec.y)).y > 0 ? 1.0f : -1.0f);
					if (RotateAngle < 2 * PI && RotateAngle>-2 * PI)
					{
						objects->ChangeRotate(glm::vec3(0.0f, 0.0f, RotateAngle));
					}
				 	//m_ControlLayer->rotateArrow->ChangeRotate(glm::vec3(0.0f, 0.0f, RotateAngle), 2);
				 	
				}



			}
			LastWorldClickPos = WorldClickPos;
			//LastArrowPos = ArrowPos;
		}


		return true;
	}

	bool Application::OnMouseReleaseEvent(MouseButtonReleasedEvent& e)
	{
		ToMove = false;
		return true;
	}

	bool Application::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == HZ_KEY_ESCAPE)
		{
			if (!ModularCopy)
			{
				objects->SetChoosedIndex(-1, -1);
			}
			else
			{
				ModularCopy = false;
			}
		}
		return true;
	}

}
	

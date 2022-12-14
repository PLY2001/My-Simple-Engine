#include "hzpch.h"
#include "Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Input.h"



namespace Hazel
{



#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)//定义一个绑定某函数的函数对象，类函数第一个输入参量必须为this

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");//保证单例
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());//创建窗口，窗口也是单例
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));//设置窗口的回调函数为OnEvent，这是窗口和Application的唯一交流通道

		m_ImGuiLayer = new ImGuiLayer();//创建imgui层，第一层
		PushOverlay(m_ImGuiLayer);
		m_ControlLayer = new ControlLayer();//控制箭头层，第二层
		PushOverlay(m_ControlLayer);


		//定义shader
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
		//ArrowShader.reset(new Shader("res/shaders/Arrow.shader"));


		//加载模型
// 		std::ifstream source("res/models/path.txt");
// 		std::string line;
// 		std::stringstream ss;
// 		while (getline(source, line))
// 		{
// 			ss << line;
// 		}
		objects.reset(new Objects());
		insbos.reset(new InstanceBufferObjects());
		/*IRB120*/
		IRB120Model.reset(new Model("res/models/ABB_IRB120/ABB_IRB120.obj"));//读取模型，目录从当前项目根目录开始，或者生成的exe根目录。需将noise.jpg复制到每一个模型旁边。
		//irb120.reset(new ABBIRB120(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.01f, 0.01f, 0.01f), IRB120Model));
		//////objects->AddObject(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.01f, 0.01f, 0.01f), IRB120Model, true);
		//irb120->InitModelMatrices();
		//objects->InitModelMatrices(0);
		//创建实例化数组
		//////insbos->AddObject(IRB120Model);
		//for (int i = 0; i < IRB120Model->meshes.size(); i++)//该模型有多个网格时，每个网格都有自己的顶点数组对象ID，要想把实例化数组缓冲区绑定在每个顶点数组对象上，就必须遍历
		//{
		//	insbo.push_back(NULL);
		//	insbo.back().reset(new InstanceBuffer(100 * sizeof(glm::mat4), NULL));
		//	insbo.back()->AddInstanceBuffermat4(IRB120Model->meshes[i].vaID, 3);
		//}

		BeltModel.reset(new Model("res/models/belt/belt.obj"));
		//belt.reset(new Belt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f), BeltModel));
		///objects->AddObject(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f), BeltModel, false);
		//belt->InitModelMatrices();
		//objects->InitModelMatrices(1);
		//insbos->AddObject(BeltModel);
		//for (int i = 0; i < BeltModel->meshes.size(); i++)//该模型有多个网格时，每个网格都有自己的顶点数组对象ID，要想把实例化数组缓冲区绑定在每个顶点数组对象上，就必须遍历
		//{
		//	insbobelt.push_back(NULL);
		//	insbobelt.back().reset(new InstanceBuffer(100 * sizeof(glm::mat4), NULL));
		//	insbobelt.back()->AddInstanceBuffermat4(BeltModel->meshes[i].vaID, 3);
		//}

		AVGModel.reset(new Model("res/models/AVGcar/AVGcar.obj"));
		/////objects->AddObject(glm::vec3(0, 0.8f, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f), AVGModel, false);
		//objects->InitModelMatrices(2);
		/////insbos->AddObject(AVGModel);
// 		for (int i = 0; i < AVGModel->meshes.size(); i++)//该模型有多个网格时，每个网格都有自己的顶点数组对象ID，要想把实例化数组缓冲区绑定在每个顶点数组对象上，就必须遍历
// 		{
// 			insboAVG.push_back(NULL);
// 			insboAVG.back().reset(new InstanceBuffer(100 * sizeof(glm::mat4), NULL));
// 			insboAVG.back()->AddInstanceBuffermat4(AVGModel->meshes[i].vaID, 3);
// 		}

		/*平面*/
		plane.reset(new Model("res/models/plane.obj", glm::vec3(0.0f, 0.0f, 0.0f)));

 		insboplane.reset(new InstanceBuffer(sizeof(glm::mat4), &plane->mModelMatrix));//创建实例化数组
 		insboplane->AddInstanceBuffermat4(plane->meshes[0].vaID, 3);
 		insboplane->SetDatamat4(sizeof(glm::mat4), &plane->mModelMatrix);

		/*移动箭头*/
		//ArrowModel.reset(new Model("res/models/arrow.obj", glm::vec3(0.0f, 0.0f, 0.0f)));
		//arrow.reset(new Arrow(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f), ArrowModel));


		//创建帧缓冲1
		framebuffer1.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer1->GenTexture2D();
		QuadID = framebuffer1->GenQuad();//用于绘制贴图的四边形
		//创建帧缓冲2
		framebuffer2.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer2->GenTexture2D();
		//创建帧缓冲MSAA
		framebufferMSAA.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebufferMSAA->GenTexture2DMultiSample(4);
		//创建帧缓冲3
		framebuffer3.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer3->GenTexture2D();
		//创建帧缓冲shadowmap
		framebufferSM.reset(new FrameBuffer(ShadowMapWidth, ShadowMapHeight));
		framebufferSM->GenTexture2DShadowMap();
		//创建帧缓冲4
		framebuffer4.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer4->GenTexture2D();
		QuadID4 = framebuffer4->GenQuad();//用于绘制贴图的四边形
		//创建帧缓冲5
		framebuffer5.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer5->GenTexture2D();
		//创建帧缓冲shadowcubemap
		framebufferSCM.reset(new FrameBuffer(ShadowMapWidth, ShadowMapHeight));
		framebufferSCM->GenTexture2DShadowCubeMap();
		//创建帧缓冲6
		//framebuffer6.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		//framebuffer6->GenTexture2D();
		//创建帧缓冲7
		//framebuffer7.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		//framebuffer7->GenTexture2D();
		//QuadID7 = framebuffer7->GenQuad();//用于绘制贴图的四边形

		//创建天空盒
		skybox.reset(new Skybox("Factory"));
		SkyboxID = skybox->GenBox();//用于绘制天空盒的立方体

		//因为天空盒需要，设置深度函数
		glDepthFunc(GL_LEQUAL);

		//创建Uniform缓冲对象
		ubo.reset(new UniformBuffer(2 * sizeof(glm::mat4), 0));
		shaderIDs.push_back(shader->RendererID);
		shaderIDs.push_back(OutlineShader->RendererID);
		shaderIDs.push_back(ShadowDrawShader->RendererID);
		shaderIDs.push_back(ShadowCubeDrawShader->RendererID);
		shaderIDs.push_back(AABBShader->RendererID);
		//shaderIDs.push_back(ArrowShader->RendererID);
		ubo->Bind(shaderIDs, "Matrices");



		//创建灯光

		PointLight.reset(new Light(glm::vec3(0.0f, 40.0f, 0.0f)));
		DirectLight.reset(new Light(glm::vec3(27.372f, 29.168f, 0.0f)));

		camera.reset(new Camera);

		glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		aabb.reset(new AABB(AABBShader));
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
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )//反向遍历层来处理事件
		{
			(*--it)->OnEvent(e);
			if (e.Handled)//一旦处理好一个事件，就不再遍历
				break;
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));//如果当前事件e为WindowCloseEvent事件，就调用OnWindowClose函数
		if (mousemode == MouseMode::Disable)
		{
			dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnCameraRotate));//控制相机视角移动
			dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OnCameraZoom));//控制相机缩放
		}
		else
		{
			dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnMousePos));//获取鼠标坐标
			dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(OnMouseReleaseEvent));//鼠标释放时，用于拖动
		}
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonEvent));//鼠标点击
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));//窗口重设大小
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressedEvent));


		//HZ_CORE_TRACE("{0}", e);//显示事件



	}

	void Application::Run()
	{


		while (m_Running)
		{

			//glClearColor(1, 0, 1, 1);
			//glClear(GL_COLOR_BUFFER_BIT);




			GLClearError();//清除错误信息


			//相机键盘输入控制
			camera->KeyControl(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), deltaTime);

			//点光源控制
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

			//直射光控制
			if (glfwGetKey(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_KEY_R) == GLFW_PRESS)
			{
				count += 0.05f;
				DirectLight->Pos = glm::vec3(40.0f * cos(count), 40.0f * sin(count), 0.0f);
			}

			//记录每帧的时间
			deltaTime = (float)glfwGetTime() - lastTime;
			lastTime = (float)glfwGetTime();

			//检查irb120角度是否要修改
			if (AngleChanged)
			{
				if (objects->GetChoosedIndex() > -1)
				{
					objects->ChangeAngle();
					AngleChanged = false;
				}
			}

			//相机的VP矩阵
			ViewMatrix = camera->SetView();
			ProjectionMatrix = camera->SetProjection((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight());

			//将model矩阵数组填入irb120的实例化数组
			insbos->SetDatamat4(objects);
			
			

			//向uniform缓冲对象填入相机的view、projection矩阵数据
			ubo->SetDatamat4(0, sizeof(glm::mat4), &ViewMatrix);
			ubo->SetDatamat4(sizeof(glm::mat4), sizeof(glm::mat4), &ProjectionMatrix);

			//设置直射光VP变换矩阵
			glm::mat4 LightProjectionMatrix = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 200.0f);
			glm::mat4 LightViewMatrix = glm::lookAt(DirectLight->Pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			//设置点光源远平面
			GLfloat farplane = 200.0f;

			/* Render here */


			//获取直射光视角的深度信息，存在framebufferSM中
			if (lightmode == LightMode::Direct)
			{
				//ShadowMap
				glViewport(0, 0, ShadowMapWidth, ShadowMapHeight);//修改视口尺寸
				//将光源当作正交摄像头的VP变换矩阵，M矩阵当然还是沿用各物体的。

				ShadowMapShader->Bind();
				ShadowMapShader->SetUniformMat4("view", LightViewMatrix);
				ShadowMapShader->SetUniformMat4("projection", LightProjectionMatrix);

				framebufferSM->Bind();//绑定帧缓冲对象，接收深度
				OpenGLRendererAPI::ClearDepth();//只需清除深度，不需清除颜色
				for (int i = 0; i < objects->GetObjectAmount(); i++)
				{
					OpenGLRendererAPI::DrawInstanced(objects->m_model[i], ShadowMapShader, objects->GetAmount(i));//绘制需要投射阴影的物体
				}
				
				ShadowMapShader->Unbind();
				framebufferSM->Unbind();

				glViewport(0, 0, s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight());//还原视口尺寸
			}

			//获取点光源6个方向的深度信息
			if (lightmode == LightMode::Point)
			{
				//ShadowCubeMap
				glViewport(0, 0, ShadowMapWidth, ShadowMapHeight);//修改视口尺寸
				//定义VP变换矩阵
				GLfloat aspect = (GLfloat)ShadowMapWidth / (GLfloat)ShadowMapHeight;
				GLfloat nearplane = 1.0f;

				glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearplane, farplane);//投影变换为透视变换，fov皆为90°
				std::vector<glm::mat4> shadowTransforms = PointLight->GetShadowTransformsPoint(shadowProj);//视角变换要有6个对应方向的变换矩阵
				//将uniform变量存入shader
				ShadowCubeMapShader->Bind();
				ShadowCubeMapShader->SetUniformMat4Vector("shadowMatrices", shadowTransforms);
				// 					for (int i = 0; i < 6; i++)
				// 					{
				// 						std::stringstream sss;
				// 						std::string index;
				// 						sss << i;
				// 						index = sss.str();
				// 						ShadowCubeMapShader->SetUniformMat4(("shadowMatrices[" + index + "]").c_str(), shadowTransforms[i]);//uniform为数组时，要循环存入
				// 					}
				ShadowCubeMapShader->SetUniform3f("lightPos", PointLight->Pos.x, PointLight->Pos.y, PointLight->Pos.z);//点光源坐标
				ShadowCubeMapShader->SetUniform1f("far_plane", farplane);//远平面距离

				framebufferSCM->Bind();//绑定帧缓冲对象，接收深度
				OpenGLRendererAPI::ClearDepth();//只需清除深度，不需清除颜色
				for (int i = 0; i < objects->GetObjectAmount(); i++)
				{
					OpenGLRendererAPI::DrawInstanced(objects->m_model[i], ShadowCubeMapShader, objects->GetAmount(i));//绘制需要投射阴影的物体
				}
				ShadowCubeMapShader->Unbind();
				framebufferSCM->Unbind();

				glViewport(0, 0, s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight());//还原视口尺寸
			}



			//first pass
			framebufferMSAA->Bind();//使用带抗锯齿的帧缓冲
			OpenGLRendererAPI::SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			OpenGLRendererAPI::ClearColor();
			OpenGLRendererAPI::ClearDepth();

			//描边
// 				if (graphicmode == GraphicMode::Normal)
// 				{
// 					//描边					
// 					OpenGLRendererAPI::CullFace("FRONT");
// 					OpenGLRendererAPI::DrawInstanced(IRB120Model, OutlineShader, irb120->GetAmount());
// 					
// 				}


			//second pass
			//render stuff

			//绘制碰撞盒
			if (objects->GetChoosedIndex() > -1)
			{
				OpenGLRendererAPI::CullFace("DISABLE");
				aabb->Draw(objects->GetAABBMinPos(), objects->GetAABBMaxPos());
			}




			OpenGLRendererAPI::CullFace("BACK");

			shader->Bind();

			//向shader发送灯光位置和相机位置
			if (lightmode == LightMode::Direct)
				shader->SetUniform4f("u_LightPosition", 100.0f * DirectLight->Pos.x, 100.0f * DirectLight->Pos.y, 100.0f * DirectLight->Pos.z, 1.0f);
			else
				shader->SetUniform4f("u_LightPosition", PointLight->Pos.x, PointLight->Pos.y, PointLight->Pos.z, 1.0f);
			shader->SetUniform4f("u_CameraPosition", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z, 1.0f);
			//向shader发送贴图单元索引
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemapTexture);
			shader->SetUniform1i("skybox", 5);

			//绘制真实物体
			for (int i = 0; i < objects->GetObjectAmount(); i++)
			{
				OpenGLRendererAPI::DrawInstanced(objects->m_model[i], shader, objects->GetAmount(i));
			}

			if (graphicmode == GraphicMode::Normal)
			{
				OpenGLRendererAPI::Draw(plane, shader);
			}

			shader->Unbind();




			if (graphicmode == GraphicMode::Outline)
			{
				//third pass
				//framebuffer stuff
				framebuffer1->GetColorAfterMSAA(framebufferMSAA->GetID());//从抗锯齿帧缓冲获取颜色到其他缓冲

				framebuffer2->Bind();
				OpenGLRendererAPI::SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				OpenGLRendererAPI::ClearColor();
				framebuffer1->Draw(TShader, QuadID);//绘制二值化

				//forth pass
				//framebuffer stuff
				framebuffer1->Unbind();
				framebuffer2->Draw(ScreenPostShader, QuadID);//二值化后，绘制边缘检测
			}
			else
			{


				//天空盒
				//third pass
				SkyShader->Bind();

				glm::mat4 SkyboxViewMatrix = glm::mat4(glm::mat3(camera->SetView()));//有位移前看出来是个很小的方块，去除位移后，方块就始终套在相机上，所以满眼都是天空盒了
				SkyShader->SetUniformMat4("view", SkyboxViewMatrix);
				SkyShader->SetUniformMat4("projection", ProjectionMatrix);
				skybox->Draw(SkyShader, SkyboxID);

				framebuffer3->GetColorAfterMSAA(framebufferMSAA->GetID());//从抗锯齿帧缓冲获取颜色到其他缓冲
				framebuffer3->Unbind();
				framebuffer3->Draw(ScreenBasicShader, QuadID);



				//forth pass
				//绘制阴影
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
					for (int i = 0; i < objects->GetObjectAmount(); i++)
					{
						OpenGLRendererAPI::DrawInstanced(objects->m_model[i], ShadowDrawShader, objects->GetAmount(i));
					}
					OpenGLRendererAPI::Draw(plane, ShadowDrawShader);


					ShadowDrawShader->Unbind();
				}


				//点光源万向阴影
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
					for (int i = 0; i < objects->GetObjectAmount(); i++)
					{
						OpenGLRendererAPI::DrawInstanced(objects->m_model[i], ShadowCubeDrawShader, objects->GetAmount(i));
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



				//fifth pass
				//对阴影高斯滤波
				framebuffer5->Bind();
				OpenGLRendererAPI::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
				OpenGLRendererAPI::ClearColor();
				framebuffer4->Draw(ScreenBasicShader, QuadID4);

				//sixth pass
				//将阴影叠加在原图上
				framebuffer5->Unbind();
				//renderer.ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				framebuffer5->Draw(GaussianShader, QuadID4);
				glDisable(GL_BLEND);

				//绘制移动箭头
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





			for (Layer* layer : m_LayerStack)//正向遍历层来显示普通层
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)//正向遍历层来显示Imgui层
				layer->OnImGuiRender();
			m_ImGuiLayer->End();


			m_Window->OnUpdate();//SwapBuffers和PollEvent
			GLCheckError();//获取错误信息	




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
		glViewport(0, 0, WinWidth, WinHeight);//视口变换
		framebuffer1->ResetWindow(WinWidth, WinHeight);
		framebuffer2->ResetWindow(WinWidth, WinHeight);
		framebufferMSAA->ResetWindowMultiSample(WinWidth, WinHeight);
		framebuffer3->ResetWindow(WinWidth, WinHeight);
		framebuffer4->ResetWindow(WinWidth, WinHeight);
		framebuffer5->ResetWindow(WinWidth, WinHeight);
		//framebuffer6->ResetWindow(WinWidth, WinHeight);
		//framebuffer7->ResetWindow(WinWidth, WinHeight);
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
		//按下右键时
		if (e.GetMouseButton() == HZ_MOUSE_BUTTON_RIGHT)
		{
			if (mousemode == MouseMode::Enable)//关闭指针
			{
				glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				mousemode = MouseMode::Disable;
				camera->firstMouse = 1;
			}
			else//开启指针
			{
				glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				mousemode = MouseMode::Enable;
			}
		}

		//当指针开启时
		if (mousemode == MouseMode::Enable)
		{
			//irb120->SetChoosedIndex(-1);
			//Choosed = false;
			//ToMove = false;
			ClickPos = glm::vec2(MousePos.x / m_Window->GetWidth() * 2.0f - 1.0f, MousePos.y / m_Window->GetHeight() * 2.0f - 1.0f);
			//ClickPos = MousePos;

			for (float ClipZ = 1.0f; ClipZ > 0.0f; ClipZ -= 0.0001f)//OpenGL的相机中，z轴指向自己，z越大越近
			{
				glm::vec4 WorldClickPos = glm::inverse(ViewMatrix) * glm::inverse(ProjectionMatrix) * glm::vec4(ClickPos, ClipZ, 1.0f);
				WorldClickPos /= WorldClickPos.w;


				//检测irb120碰撞
				if (!ToMove)
				{
					for (int j = 0; j < objects->GetObjectAmount(); j++)
					{
						for (int i = 0; i < objects->GetAmount(j); i++)
						{
							if (objects->CheckCollision(j,i, WorldClickPos))
							{
								objects->SetChoosedIndex(j,i);
								//Choosed = true;
								break;
							}						
						}
					}
					//检测arrow碰撞
	// 				for (int i = 0; i < 3; i++)
	// 				{
	// 					if (WorldClickPos.x > arrow->GetAABBMinPos(i).x&& WorldClickPos.x < arrow->GetAABBMaxPos(i).x)
	// 					{
	// 						if (WorldClickPos.y > arrow->GetAABBMinPos(i).y&& WorldClickPos.y < arrow->GetAABBMaxPos(i).y)
	// 						{
	// 							if (WorldClickPos.z > arrow->GetAABBMinPos(i).z&& WorldClickPos.z < arrow->GetAABBMaxPos(i).z)
	// 							{
	// 								ToMove = true;
	// 								Choosed = true;
	// 								first = true;
	// 								axis = i;
	// 								break;
	// 							}
	// 
	// 						}
	// 
	// 					}
	// 				}

				}
				// 			if (!Choosed)
				// 			{
				// 				irb120->SetChoosedIndex(-1);
				// 			}

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
					if (RotateAngle <= 180.0f && RotateAngle >= -180.0f)
					{
						objects->ChangeRotate(glm::vec3(0.0f, RotateAngle, 0.0f), 1);
						//m_ControlLayer->rotateArrow->ChangeRotate(glm::vec3(0.0f, RotateAngle, 0.0f), 1);
					}
				}
				// 				if (axis == 1)//x
				// 				{
				// 					glm::vec2 WorldClickVec = glm::normalize(glm::vec2(WorldClickPos.y - irb120->GetPos().y, WorldClickPos.z - irb120->GetPos().z));
				// 					glm::vec2 LastWorldClickVec = glm::normalize(glm::vec2(LastWorldClickPos.y - irb120->GetPos().y, LastWorldClickPos.z - irb120->GetPos().z));
				// 					float RotateAngle = acos(glm::dot(WorldClickVec, LastWorldClickVec)) * (glm::cross(glm::vec3(WorldClickVec.x, 0.0f, WorldClickVec.y), glm::vec3(LastWorldClickVec.x, 0.0f, LastWorldClickVec.y)).y > 0 ? 1.0f : -1.0f);
				// 					if (RotateAngle <= 180.0f && RotateAngle >= -180.0f)
				// 					{
				// 						irb120->ChangeRotate(glm::vec3(RotateAngle, 0.0f, 0.0f), 0);
				// 						//m_ControlLayer->rotateArrow->ChangeRotate(glm::vec3(RotateAngle, 0.0f, 0.0f), 0);
				// 					}
				// 				}
				// 				if (axis == 2)//z
				// 				{
				// 					glm::vec2 WorldClickVec = glm::normalize(glm::vec2(WorldClickPos.x - irb120->GetPos().x, WorldClickPos.y - irb120->GetPos().y));
				// 					glm::vec2 LastWorldClickVec = glm::normalize(glm::vec2(LastWorldClickPos.x - irb120->GetPos().x, LastWorldClickPos.y - irb120->GetPos().y));
				// 					float RotateAngle = acos(glm::dot(WorldClickVec, LastWorldClickVec)) * (glm::cross(glm::vec3(WorldClickVec.x, 0.0f, WorldClickVec.y), glm::vec3(LastWorldClickVec.x, 0.0f, LastWorldClickVec.y)).y > 0 ? 1.0f : -1.0f);
				// 					if (RotateAngle<=180.0f && RotateAngle >= -180.0f)
				// 					{
				// 						irb120->ChangeRotate(glm::vec3(0.0f, 0.0f, RotateAngle), 2);
				// 						//m_ControlLayer->rotateArrow->ChangeRotate(glm::vec3(0.0f, 0.0f, RotateAngle), 2);
				// 					}
				//				}



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
			objects->SetChoosedIndex(-1,-1);
		}
		return true;
	}

}
	

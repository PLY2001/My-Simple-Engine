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

		m_ImGuiLayer = new ImGuiLayer();//创建imgui层
		PushOverlay(m_ImGuiLayer);

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

		//加载模型
		std::ifstream source("res/models/path.txt");
		std::string line;
		std::stringstream ss;
		while (getline(source, line))
		{
			ss << line;
		}

		/*人物*/
		modelmesh.reset(new Model(ss.str(), glm::vec3(0.0f, 0.0f, 0.0f)));//读取模型，目录从当前项目根目录开始，或者生成的exe根目录。需将noise.jpg复制到每一个模型旁边。
	
		//创建变换矩阵
		ModelMatrices.push_back(ModelMatrix(glm::vec3(0.0f, 0.0f, 0.0f)).Matrix);
		

		//创建实例化数组
		insbo.reset(new InstanceBuffer(100 * sizeof(glm::mat4), NULL));
		for (Mesh mesh : modelmesh->meshes)//该模型有多个网格时，每个网格都有自己的顶点数组对象ID，要想把实例化数组缓冲区绑定在每个顶点数组对象上，就必须遍历
		{
			insbo->AddInstanceBuffermat4(mesh.vaID, 3);
		}
		
		/*平面*/
		plane.reset(new Model("res/models/plane.obj", glm::vec3(0.0f, 0.0f, 0.0f)));
		insboplane.reset(new InstanceBuffer(sizeof(glm::mat4), &plane->mModelMatrix));//创建实例化数组
		insboplane->AddInstanceBuffermat4(plane->meshes[0].vaID, 3);
		insboplane->SetDatamat4(sizeof(glm::mat4) * ModelCount, ModelMatrices.data());
		
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
		framebuffer6.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer6->GenTexture2D();
		//创建帧缓冲7
		framebuffer7.reset(new FrameBuffer(s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight()));
		framebuffer7->GenTexture2D();
		QuadID7 = framebuffer7->GenQuad();//用于绘制贴图的四边形

		//创建天空盒
		skybox.reset(new Skybox("Church"));
		SkyboxID = skybox->GenBox();//用于绘制天空盒的立方体

		//因为天空盒需要，设置深度函数
		glDepthFunc(GL_LEQUAL);

		//创建Uniform缓冲对象
		ubo.reset(new UniformBuffer(2 * sizeof(glm::mat4), 0));
		std::vector<int> shaderIDs;
		shaderIDs.push_back(shader->RendererID);
		shaderIDs.push_back(OutlineShader->RendererID);
		shaderIDs.push_back(ShadowDrawShader->RendererID);
		ubo->Bind(shaderIDs, "Matrices");



		//创建灯光
		
		PointLight.reset(new Light(glm::vec3(0.0f, 40.0f, 0.0f)));
		DirectLight.reset(new Light(glm::vec3(40.0f, 0.0f, 0.0f)));

		camera.reset(new Camera);

		glfwSetInputMode(static_cast<GLFWwindow*>(s_Instance->GetWindow().GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		//gltf
		gApplication = new Sample();//初始化
		gApplication->Initialize();//导入gltf
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
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));//如果当前事件e为WindowCloseEvent事件，就调用OnWindowClose函数
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
		//HZ_CORE_TRACE("{0}", e);//显示事件
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )//反向遍历层来处理事件
		{
			(*--it)->OnEvent(e);
			if (e.Handled)//一旦处理好一个事件，就不再遍历
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
				

				GLClearError();//清除错误信息

				//gltf
				if (gApplication != 0) {
					gApplication->Update(deltaTime);//播放动画
				}

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


				//增加模型
				
					if (int Increase = ModelCount -ModelMatrices.size()>0)
					{
						for(int i=0;i<Increase;i++)
							ModelMatrices.push_back(ModelMatrix(glm::vec3((ModelCount-1) * 10.0f, 0.0f, 0.0f)).Matrix);
					}
				
				//切换显示模式
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
				//修改灯光位置
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

				//记录每帧的时间
				deltaTime = (float)glfwGetTime() - lastTime;
				lastTime = (float)glfwGetTime();

				//设置变换矩阵
				for (glm::mat4& modelmatrix : ModelMatrices)
				{
					modelmatrix = glm::rotate(modelmatrix, deltaTime * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				ViewMatrix = camera->SetView();
				ProjectionMatrix = camera->SetProjection((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight());

				//将model矩阵数组填入实例化数组
				insbo->SetDatamat4(sizeof(glm::mat4) * ModelCount, ModelMatrices.data());

				//向uniform缓冲对象填入view、projection矩阵数据
				ubo->SetDatamat4(0, sizeof(glm::mat4), &ViewMatrix);
				ubo->SetDatamat4(sizeof(glm::mat4), sizeof(glm::mat4), &ProjectionMatrix);

				//设置直射光VP变换矩阵
				glm::mat4 LightProjectionMatrix = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 200.0f);
				glm::mat4 LightViewMatrix = glm::lookAt(DirectLight->Pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

				//设置点光源远平面
				GLfloat farplane = 200.0f;

				/* Render here */



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
					//OpenGLRendererAPI::DrawInstanced(modelmesh,ShadowMapShader, ModelCount);//绘制需要投射阴影的物体
					gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(),camera,DirectLight);//绘制模型

					ShadowMapShader->Unbind();
					framebufferSM->Unbind();

					glViewport(0, 0, s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight());//还原视口尺寸
				}

				if (lightmode == LightMode::Point)
				{
					//点光源万向阴影
							//ShadowCubeMap
					glViewport(0, 0, ShadowMapWidth, ShadowMapHeight);//修改视口尺寸
					//定义VP变换矩阵
					GLfloat aspect = (GLfloat)ShadowMapWidth / (GLfloat)ShadowMapHeight;
					GLfloat nearplane = 1.0f;

					glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearplane, farplane);//投影变换为透视变换，fov皆为90°
					std::vector<glm::mat4> shadowTransforms = PointLight->GetShadowTransformsPoint(shadowProj);//视角变换要有6个对应方向的变换矩阵
					//将uniform变量存入shader
					ShadowCubeMapShader->Bind();
					for (int i = 0; i < 6; i++)
					{
						std::stringstream sss;
						std::string index;
						sss << i;
						index = sss.str();
						ShadowCubeMapShader->SetUniformMat4(("shadowMatrices[" + index + "]").c_str(), shadowTransforms[i]);//uniform为数组时，要循环存入
					}
					ShadowCubeMapShader->SetUniform3f("lightPos", PointLight->Pos.x, PointLight->Pos.y, PointLight->Pos.z);//点光源坐标
					ShadowCubeMapShader->SetUniform1f("far_plane", farplane);//远平面距离

					framebufferSCM->Bind();//绑定帧缓冲对象，接收深度
					OpenGLRendererAPI::ClearDepth();//只需清除深度，不需清除颜色

					//OpenGLRendererAPI::DrawInstanced(modelmesh,ShadowCubeMapShader, ModelCount);//绘制需要投射阴影的物体
					gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(),camera,PointLight);//绘制模型

					ShadowCubeMapShader->Unbind();
					framebufferSCM->Unbind();

					glViewport(0, 0, s_Instance->GetWindow().GetWidth(), s_Instance->GetWindow().GetHeight());//还原视口尺寸
				}



				//first pass
				framebufferMSAA->Bind();//使用带抗锯齿的帧缓冲
				OpenGLRendererAPI::SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				OpenGLRendererAPI::ClearColor();
				OpenGLRendererAPI::ClearDepth();
				//glEnable(GL_DEPTH_TEST);//zbuffer		

				if (graphicmode == GraphicMode::Normal)
				{
					//render stuff
					//描边
					/*
					renderer.CullFace("FRONT");
					modelmesh.DrawInstanced(OutlineShader,ModelCount);
					*/
				}


				//second pass
				//render stuff
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

				//OpenGLRendererAPI::DrawInstanced(modelmesh,shader, ModelCount);
				if (lightmode == LightMode::Direct)
				{
					gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(), camera,DirectLight);//绘制模型
				}
				else
				{
					gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(), camera, PointLight);//绘制模型
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

					//framebufferMSAA.ShowColorAfterMSAA(framebufferMSAA.GetID());//从抗锯齿帧缓冲获取颜色到默认帧缓冲0，可直接显示
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

						//OpenGLRendererAPI::DrawInstanced(modelmesh,ShadowDrawShader, ModelCount);
						gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(),camera,DirectLight);//绘制模型

						OpenGLRendererAPI::Draw(plane,ShadowDrawShader);


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
						
						//OpenGLRendererAPI::DrawInstanced(modelmesh, ShadowCubeDrawShader, ModelCount);
						gApplication->Render((float)s_Instance->GetWindow().GetWidth() / s_Instance->GetWindow().GetHeight(),camera,PointLight);//绘制模型

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
					//framebuffer4.Unbind();

					
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
		glViewport(0, 0, WinWidth, WinHeight);//视口变换
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

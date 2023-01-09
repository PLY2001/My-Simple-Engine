#include "hzpch.h"
#include "ControlLayer.h"
#include "Hazel/Application.h"

namespace Hazel {
#define BIND_EVENT_FN(x) std::bind(&ControlLayer::x, this, std::placeholders::_1)//定义一个绑定某函数的函数对象，类函数第一个输入参量必须为this

	ControlLayer::ControlLayer():Layer("ControlLayer")
	{

	}


	void ControlLayer::OnAttach()
	{
		ArrowShader.reset(new Shader("res/shaders/Arrow.shader"));
		ArrowModel.reset(new Model("res/models/arrow.obj", glm::vec3(0.0f, 0.0f, 0.0f)));
		arrow.reset(new Arrow(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.015f, 0.015f, 0.015f), ArrowModel));
		RotateArrowModel.reset(new Model("res/models/rotateY.obj", glm::vec3(0.0f, 0.0f, 0.0f)));
		rotateArrow.reset(new Arrow(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.015f, 0.015f, 0.015f), RotateArrowModel));
		Application::Get().shaderIDs.push_back(ArrowShader->RendererID);
	}

	void ControlLayer::OnDetach()
	{
		
	}

	void ControlLayer::OnUpdate()
	{
		if (Application::Get().irb120->GetChoosedIndex() > -1 )
		{
			
			pos = Application::Get().ProjectionMatrix * Application::Get().ViewMatrix * glm::vec4(Application::Get().irb120->GetPos(), 1.0f);
			pos /= pos.w;
			Arrow_pos = glm::inverse(Application::Get().ViewMatrix) * glm::inverse(Application::Get().ProjectionMatrix) * glm::vec4(pos.x, pos.y, 0.5f, 1.0f);
			Arrow_pos /= Arrow_pos.w;

 			arrow->ChangePos(glm::vec3(Arrow_pos));
			rotateArrow->ChangePos(glm::vec3(Arrow_pos));

			
			rotateArrow->ChangeRotate(Application::Get().irb120->GetRotate());

			glDisable(GL_DEPTH_TEST);
			ArrowShader->Bind();
			if (arrowmode == ArrowMode::Trans)
			{
				ArrowShader->SetUniformMat4("model", arrow->GetModelMatrix());
			}
			if (arrowmode == ArrowMode::Rotat)
			{
				ArrowShader->SetUniformMat4("model", rotateArrow->GetModelMatrix());
			}
			if (Application::Get().ToMove)
			{
				ArrowShader->SetUniform1f("Clicked", 1.0f);
			}
			else
			{
				ArrowShader->SetUniform1f("Clicked", 0.0f);
			}

			if(arrowmode == ArrowMode::Trans)
			{	
				OpenGLRendererAPI::Draw(ArrowModel, ArrowShader);
			}
			if(arrowmode == ArrowMode::Rotat)
			{
				OpenGLRendererAPI::Draw(RotateArrowModel, ArrowShader);
			}
			ArrowShader->Unbind();
			glEnable(GL_DEPTH_TEST);
		}
	}

	void ControlLayer::OnEvent(Hazel::Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonEvent));//鼠标点击
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressedEvent));//鼠标点击
	}

	bool ControlLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		
		if (e.GetKeyCode() == HZ_KEY_1)
		{
			arrowmode = ArrowMode::None;
		}
		if (e.GetKeyCode() == HZ_KEY_2)
		{
			arrowmode = ArrowMode::Trans;
		}
		if (e.GetKeyCode() == HZ_KEY_3)
		{
			arrowmode = ArrowMode::Rotat;
		}
		return true;
	}

	bool ControlLayer::OnMouseButtonEvent(MouseButtonPressedEvent& e)
	{
		//当指针开启时
		if ((int)Application::Get().mousemode == 1)
		{
			//irb120->SetChoosedIndex(-1);
			Application::Get().Choosed = false;
			Application::Get().ToMove = false;
			Application::Get().ClickPos = glm::vec2(Application::Get().MousePos.x / Application::Get().GetWindow().GetWidth() * 2.0f - 1.0f, Application::Get().MousePos.y / Application::Get().GetWindow().GetHeight() * 2.0f - 1.0f);
			//ClickPos = MousePos;

			for (float ClipZ = 1.0f; ClipZ > 0.0f; ClipZ -= 0.0001f)
			{
				glm::vec4 WorldClickPos = glm::inverse(Application::Get().ViewMatrix) * glm::inverse(Application::Get().ProjectionMatrix) * glm::vec4(Application::Get().ClickPos, ClipZ, 1.0f);
				WorldClickPos /= WorldClickPos.w;

				//检测arrow碰撞
				if(arrowmode == ArrowMode::Trans)
				{
					for (int i = 0; i < 3; i++)
					{
						if (WorldClickPos.x > arrow->GetAABBMinPos(i).x&& WorldClickPos.x < arrow->GetAABBMaxPos(i).x)
						{
							if (WorldClickPos.y > arrow->GetAABBMinPos(i).y&& WorldClickPos.y < arrow->GetAABBMaxPos(i).y)
							{
								if (WorldClickPos.z > arrow->GetAABBMinPos(i).z&& WorldClickPos.z < arrow->GetAABBMaxPos(i).z)
								{
									Application::Get().ToMove = true;
									Application::Get().Choosed = true;
									Application::Get().first = true;
									Application::Get().axis = i;
									break;
								}

							}

						}
					}
				}
				if (arrowmode == ArrowMode::Rotat)
				{
					for (int i = 0; i < 3; i++)
					{
						if (WorldClickPos.x > rotateArrow->GetAABBMinPos(i).x&& WorldClickPos.x < rotateArrow->GetAABBMaxPos(i).x)
						{
							if (WorldClickPos.y > rotateArrow->GetAABBMinPos(i).y&& WorldClickPos.y < rotateArrow->GetAABBMaxPos(i).y)
							{
								if (WorldClickPos.z > rotateArrow->GetAABBMinPos(i).z&& WorldClickPos.z < rotateArrow->GetAABBMaxPos(i).z)
								{
									Application::Get().ToMove = true;
									Application::Get().Choosed = true;
									Application::Get().first = true;
									Application::Get().axis = i;
									break;
								}

							}

						}
					}
				}

			}

		}

		return true;
	}
	void ControlLayer::OnImGuiRender()
	{

	}
}
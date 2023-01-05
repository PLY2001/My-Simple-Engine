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
		arrow.reset(new Arrow(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f), ArrowModel));
		Application::Get().shaderIDs.push_back(ArrowShader->RendererID);
	}

	void ControlLayer::OnDetach()
	{
		
	}

	void ControlLayer::OnUpdate()
	{
		if (Application::Get().irb120->GetChoosedIndex() > -1 )
		{
			arrow->ChangePos(Application::Get().irb120->GetPos());
			glDisable(GL_DEPTH_TEST);
			ArrowShader->Bind();
			ArrowShader->SetUniformMat4("model", arrow->GetModelMatrix());
			if (Application::Get().ToMove)
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

	void ControlLayer::OnEvent(Hazel::Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonEvent));//鼠标点击
	}

	void ControlLayer::OnImGuiRender()
	{

	}

	bool ControlLayer::OnMouseButtonEvent(MouseButtonPressedEvent& e)
	{
		//当指针开启时
		if ((int)Application::Get().mousemode == 1)
		{
			//irb120->SetChoosedIndex(-1);
			//Application::Get().Choosed = false;
			Application::Get().ToMove = false;
			Application::Get().ClickPos = glm::vec2(Application::Get().MousePos.x / Application::Get().GetWindow().GetWidth() * 2.0f - 1.0f, Application::Get().MousePos.y / Application::Get().GetWindow().GetHeight() * 2.0f - 1.0f);
			//ClickPos = MousePos;

			for (float ClipZ = 1.0f; ClipZ > 0.0f; ClipZ -= 0.0001f)
			{
				glm::vec4 WorldClickPos = glm::inverse(Application::Get().ViewMatrix) * glm::inverse(Application::Get().ProjectionMatrix) * glm::vec4(Application::Get().ClickPos, ClipZ, 1.0f);
				WorldClickPos /= WorldClickPos.w;

				//检测arrow碰撞
				for (int i = 0; i < 3; i++)
				{
					if (WorldClickPos.x > arrow->GetAABBMinPos(i).x&& WorldClickPos.x < arrow->GetAABBMaxPos(i).x)
					{
						if (WorldClickPos.y > arrow->GetAABBMinPos(i).y&& WorldClickPos.y < arrow->GetAABBMaxPos(i).y)
						{
							if (WorldClickPos.z > arrow->GetAABBMinPos(i).z&& WorldClickPos.z < arrow->GetAABBMaxPos(i).z)
							{
								Application::Get().ToMove = true;
								//Application::Get().Choosed = true;
								Application::Get().first = true;
								Application::Get().axis = i;
								break;
							}

						}

					}
				}

			}

		}

		return true;
	}
}
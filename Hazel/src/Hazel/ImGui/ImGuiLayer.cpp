#include "hzpch.h"
#include "ImGuiLayer.h"

//imgui+glfw 需要引用下方3个文件
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Hazel/Application.h"



namespace Hazel {

	glm::vec3 Pos = glm::vec3(-3.74f,6.3f,0.0f);
	glm::vec3 Eular = glm::vec3(0.0f, 0.0f, 0.0f);


	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}


	void ImGuiLayer::OnAttach()//ImGui初始化
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		io.Fonts->AddFontFromFileTTF("res/fonts/msyh.ttc", 30.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());//字体、大小

		

		// Setup Dear ImGui style
		ImGui::StyleColorsLight();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 0.5f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()//ImGui销毁
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()//ImGui绘制前的准备
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()//指定完绘制内容后，开始绘制ImGui
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// 绘制界面
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnImGuiRender()//ImGui绘制内容
	{
		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);//显示imgui示例界面
		
		
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);
		
		//自定义imgui窗口
		ImGui::Begin(u8"编辑");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text(u8"切换渲染风格");           
		ImGui::RadioButton(u8"真实", (int*)&Application::Get().graphicmode , 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"描边", (int*)&Application::Get().graphicmode, 1);

		ImGui::Text(u8"切换灯光类型");
		ImGui::RadioButton(u8"直射光", (int*)&Application::Get().lightmode, 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"点光源", (int*)&Application::Get().lightmode, 1);
		if (Application::Get().objects->GetChoosedIndex() > -1)
		{
			if (ImGui::Button(u8"复制所选模型"))
				Application::Get().objects->AddAmount();
			ImGui::SameLine();
			ImGui::Text(u8"所选模型总计 %d 个", Application::Get().objects->GetMyAmount());

			if ((int)Application::Get().objects->GetControlMode() > 0)
			{
				ImGui::Text(u8"机器人控制");
				ImGui::RadioButton(u8"正向运动学", Application::Get().objects->GetControlModeAddress(), 1);
				ImGui::SameLine();
				ImGui::RadioButton(u8"逆向运动学", Application::Get().objects->GetControlModeAddress(), 2);
			}
		
			if ((int)Application::Get().objects->GetControlMode() == 1)
			{
				if (ImGui::SliderFloat("Angle1", Application::Get().objects->SetAngle(1), -165.0f, 165.0f))
				{
					Application::Get().AngleChanged = true;
				}
				if (ImGui::SliderFloat("Angle2", Application::Get().objects->SetAngle(2), -110.0f, 110.0f))
				{
					Application::Get().AngleChanged = true;
				}
				if (ImGui::SliderFloat("Angle3", Application::Get().objects->SetAngle(3), -110.0f, 70.0f))
				{
					Application::Get().AngleChanged = true;
				}
				if (ImGui::SliderFloat("Angle4", Application::Get().objects->SetAngle(4), -160.0f, 160.0f))
				{
					Application::Get().AngleChanged = true;
				}
				if (ImGui::SliderFloat("Angle5", Application::Get().objects->SetAngle(5), -120.0f, 120.0f))
				{
					Application::Get().AngleChanged = true;
				}
				if (ImGui::SliderFloat("Angle6", Application::Get().objects->SetAngle(6), -400.0f, 400.0f))
				{
					Application::Get().AngleChanged = true;
				}
			}

			if ((int)Application::Get().objects->GetControlMode() == 2)
			{
				glm::vec3 Scale = Application::Get().objects->GetScale();
				if (ImGui::SliderFloat("PosX", &Pos.x, -652.0f * Scale.x, 652.0f * Scale.x))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle(Pos / Scale, Eular);
				}
				if (ImGui::SliderFloat("PosY", &Pos.y, -184.0f * Scale.y, 1054.0f * Scale.y))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle(Pos / Scale, Eular);;
				}
				if (ImGui::SliderFloat("PosZ", &Pos.z, -652.0f * Scale.z, 652.0f * Scale.z))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle(Pos / Scale, Eular);;
				}
				if (ImGui::SliderFloat("EularX", &Eular.x, -180.0f, 180.0f))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle(Pos / Scale, Eular);;
				}
				if (ImGui::SliderFloat("EularY", &Eular.y, -180.0f, 180.0f))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle(Pos / Scale, Eular);;
				}
				if (ImGui::SliderFloat("EularZ", &Eular.z, -180.0f, 180.0f))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle(Pos / Scale, Eular);;
				}


			}
		}

		//int index = Application::Get().index;
// 		if(Application::Get().irb120->GetAmount()>1)
// 		{	
// 			int index = 1;
// 			ImGui::Text("IRB 120 AABB: XMin = %.3f , XMax = %.3f , YMin = %.3f , YMax = %.3f , ZMin = %.3f , ZMax = %.3f", Application::Get().irb120->GetAABBMinPos(index).x, Application::Get().irb120->GetAABBMaxPos(index).x, Application::Get().irb120->GetAABBMinPos(index).y, Application::Get().irb120->GetAABBMaxPos(index).y, Application::Get().irb120->GetAABBMinPos(index).z, Application::Get().irb120->GetAABBMaxPos(index).z);
// 		}
		
		ImGui::Text(u8"鼠标点击坐标 = ( %f , %f )", Application::Get().GetClickPos().x, Application::Get().GetClickPos().y);
		ImGui::Text(u8"当前选择模型索引 = %d", Application::Get().objects->GetChoosedIndex());
		if (Application::Get().objects->GetChoosedIndex() > -1)
		{
			irb120Pos = Application::Get().objects->GetPos()/10.0f;
			irb120Rotate = Application::Get().objects->GetRotate() * glm::vec3(180.0f/PI);
			if (ImGui::InputFloat3(u8"平移(m)", (float*)&irb120Pos))
			{
				Application::Get().objects->ChangePos(irb120Pos*10.0f - Application::Get().objects->GetPos());
			}
			if (ImGui::InputFloat3(u8"旋转(degree)", (float*)&irb120Rotate))
			{
				Application::Get().objects->ChangeRotate(glm::vec3(0.0f,irb120Rotate.y*PI/180.0f- Application::Get().objects->GetRotate().y,0.0f),1);
			}
			
		}
		

		//ImGui::Checkbox("ToMove",&Application::Get().ToMove);
		//ImGui::Text("LightPosition = ( %f , %f , %f )", Application::Get().DirectLight->Pos.x, Application::Get().DirectLight->Pos.y, Application::Get().DirectLight->Pos.z);
		


// 		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
// 		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
// 
// 		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
// 			counter++;
// 		ImGui::SameLine();
// 		ImGui::Text("counter = %d", counter);
// 
 		ImGui::Text(u8"每帧平均 %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


		ImGui::End();




		ImGui::Begin(u8"设备栏");
		if (ImGui::Button(u8"ABB IRB120 六轴机械臂"))
		{
			if (std::find(Application::Get().objects->ObjectsMap.begin(), Application::Get().objects->ObjectsMap.end(), "irb120") == Application::Get().objects->ObjectsMap.end())
			{
				//ObjectsMap.insert(std::pair<std::string, bool>("irb120", true));
				Application::Get().objects->AddObject("irb120", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.01f, 0.01f, 0.01f), Application::Get().IRB120Model, true);
				Application::Get().insbos->AddObject(Application::Get().IRB120Model);
			}
			else
			{
				Application::Get().objects->AddAmount("irb120");
			}
		}
		if (ImGui::Button(u8"传送带"))
		{
			if (std::find(Application::Get().objects->ObjectsMap.begin(), Application::Get().objects->ObjectsMap.end(), "belt") == Application::Get().objects->ObjectsMap.end())
			{
				//ObjectsMap.insert(std::pair<std::string, bool>("irb120", true));
				Application::Get().objects->AddObject("belt", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f), Application::Get().BeltModel, false);
				Application::Get().insbos->AddObject(Application::Get().BeltModel);
			}
			else
			{
				Application::Get().objects->AddAmount("belt");
			}
		}
		if (ImGui::Button(u8"AVG运输车"))
		{
			if (std::find(Application::Get().objects->ObjectsMap.begin(), Application::Get().objects->ObjectsMap.end(), "AVG") == Application::Get().objects->ObjectsMap.end())
			{
				//ObjectsMap.insert(std::pair<std::string, bool>("irb120", true));
				Application::Get().objects->AddObject("AVG", glm::vec3(0, 0.8f, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f), Application::Get().AVGModel, false);
				Application::Get().insbos->AddObject(Application::Get().AVGModel);
			}
			else
			{
				Application::Get().objects->AddAmount("AVG");
			}
		}
		if (ImGui::Button(u8"保存场景"))
		{
			if (Application::Get().objects->SaveScene())
			{
				Save = true;	
			}
		}
		if (Save)
		{
			ImGui::Text(u8"保存成功");
		}

		ImGui::End();

		
	}

}

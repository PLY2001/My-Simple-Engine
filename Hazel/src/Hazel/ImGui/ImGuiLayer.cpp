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

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
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


		
		//自定义imgui窗口
		ImGui::Begin("Control");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("Change Graphic Mode");           
		ImGui::RadioButton("Normal", (int*)&Application::Get().graphicmode , 0);      
		ImGui::SameLine();
		ImGui::RadioButton("Outline", (int*)&Application::Get().graphicmode, 1);

		ImGui::Text("Change Light Mode");      
		ImGui::RadioButton("Direct", (int*)&Application::Get().lightmode, 0);           
		ImGui::SameLine();
		ImGui::RadioButton("Point", (int*)&Application::Get().lightmode, 1);

		if (ImGui::Button("Increase Model Count"))                            
			Application::Get().irb120->AddAmount();
		ImGui::SameLine();
		ImGui::Text("counter = %d", Application::Get().irb120->GetAmount());

		ImGui::Text("Change Control Mode");
		ImGui::RadioButton("Angles", (int*)&Application::Get().irb120->controlmode, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Pos&Eular", (int*)&Application::Get().irb120->controlmode, 1);
		if((int)Application::Get().irb120->controlmode == 0)
		{
			if (ImGui::SliderFloat("Angle1", Application::Get().irb120->SetAngle(1), -165.0f, 165.0f))
			{
				Application::Get().AngleChanged = true;
			}
			if (ImGui::SliderFloat("Angle2", Application::Get().irb120->SetAngle(2), -110.0f, 110.0f))
			{
				Application::Get().AngleChanged = true;
			}
			if (ImGui::SliderFloat("Angle3", Application::Get().irb120->SetAngle(3), -110.0f, 70.0f))
			{
				Application::Get().AngleChanged = true;
			}
			if (ImGui::SliderFloat("Angle4", Application::Get().irb120->SetAngle(4), -160.0f, 160.0f))
			{
				Application::Get().AngleChanged = true;
			}
			if (ImGui::SliderFloat("Angle5", Application::Get().irb120->SetAngle(5), -120.0f, 120.0f))
			{
				Application::Get().AngleChanged = true;
			}
			if (ImGui::SliderFloat("Angle6", Application::Get().irb120->SetAngle(6), -400.0f, 400.0f))
			{
				Application::Get().AngleChanged = true;
			}
		}

		if ((int)Application::Get().irb120->controlmode == 1)
		{
			glm::vec3 Scale = Application::Get().irb120->GetScale();
			if (ImGui::SliderFloat("PosX", &Pos.x, -652.0f * Scale.x, 652.0f * Scale.x))
			{
				Application::Get().AngleChanged = Application::Get().irb120->SolveAngle(Pos / Scale,Eular);
			}
			if (ImGui::SliderFloat("PosY", &Pos.y, -184.0f * Scale.y, 1054.0f * Scale.y))
			{
				Application::Get().AngleChanged = Application::Get().irb120->SolveAngle(Pos / Scale, Eular);;
			}
			if (ImGui::SliderFloat("PosZ", &Pos.z, -652.0f * Scale.z, 652.0f * Scale.z))
			{
				Application::Get().AngleChanged = Application::Get().irb120->SolveAngle(Pos / Scale, Eular);;
			}
			if (ImGui::SliderFloat("EularX", &Eular.x, -180.0f, 180.0f))
			{
				Application::Get().AngleChanged = Application::Get().irb120->SolveAngle(Pos / Scale, Eular);;
			}
			if (ImGui::SliderFloat("EularY", &Eular.y, -180.0f, 180.0f))
			{
				Application::Get().AngleChanged = Application::Get().irb120->SolveAngle(Pos / Scale, Eular);;
			}
			if (ImGui::SliderFloat("EularZ", &Eular.z, -180.0f, 180.0f))
			{
				Application::Get().AngleChanged = Application::Get().irb120->SolveAngle(Pos / Scale, Eular);;
			}
			
			
		}

		//int index = Application::Get().index;
// 		if(Application::Get().irb120->GetAmount()>1)
// 		{	
// 			int index = 1;
// 			ImGui::Text("IRB 120 AABB: XMin = %.3f , XMax = %.3f , YMin = %.3f , YMax = %.3f , ZMin = %.3f , ZMax = %.3f", Application::Get().irb120->GetAABBMinPos(index).x, Application::Get().irb120->GetAABBMaxPos(index).x, Application::Get().irb120->GetAABBMinPos(index).y, Application::Get().irb120->GetAABBMaxPos(index).y, Application::Get().irb120->GetAABBMinPos(index).z, Application::Get().irb120->GetAABBMaxPos(index).z);
// 		}
		
		ImGui::Text("ClickPos = ( %f , %f )", Application::Get().GetClickPos().x, Application::Get().GetClickPos().y);
		//ImGui::Checkbox("Choosed",&Application::Get().Choosed);


// 		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
// 		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
// 
// 		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
// 			counter++;
// 		ImGui::SameLine();
// 		ImGui::Text("counter = %d", counter);
// 
 		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		
	}

}

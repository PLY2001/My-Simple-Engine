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

#define PI 3.1415926

namespace Hazel {

	glm::vec3 Pos = glm::vec3(-374.0f,630.0f,0.0f);
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

// 		ImGui::InputFloat("PosX", &Pos.x);
// 		ImGui::InputFloat("PosY", &Pos.y);
// 		ImGui::InputFloat("PosZ", &Pos.z);
// 		ImGui::InputFloat("EularX", &Eular.x);
// 		ImGui::InputFloat("EularY", &Eular.y);
// 		ImGui::InputFloat("EularZ", &Eular.z);
		if (ImGui::SliderFloat("PosX", &Pos.x, -652.0f, 652.0f))
		{
			Application::Get().AngleChanged = true;
		}
		if (ImGui::SliderFloat("PosY", &Pos.y, -184.0f, 1054.0f))
		{
			Application::Get().AngleChanged = true;
		}
		if (ImGui::SliderFloat("PosZ", &Pos.z, -652.0f, 652.0f))
		{
			Application::Get().AngleChanged = true;
		}
		if (ImGui::SliderFloat("EularX", &Eular.x, -180.0f, 180.0f))
		{
			Application::Get().AngleChanged = true;
		}
		if (ImGui::SliderFloat("EularY", &Eular.y, -180.0f, 180.0f))
		{
			Application::Get().AngleChanged = true;
		}
		if (ImGui::SliderFloat("EularZ", &Eular.z, -180.0f, 180.0f))
		{
			Application::Get().AngleChanged = true;
		}
		
		glm::mat4 backwardTranslate = glm::mat4(1);;
		backwardTranslate = glm::translate(backwardTranslate, glm::vec3(72.0f, 0.0f, 0.0f));
		

		glm::mat4 backwardEular = glm::mat4(1);
		backwardEular = glm::translate(backwardEular, Pos);
		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.x), glm::vec3(1, 0, 0));
		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.y), glm::vec3(0, 1, 0));
		backwardEular = glm::rotate(backwardEular, glm::radians(Eular.z), glm::vec3(0, 0, 1));
		backwardEular = glm::translate(backwardEular, -Pos);
		
		glm::vec4 Pw = backwardEular * backwardTranslate *  glm::vec4(Pos, 1.0f);
		
		ImGui::Text("PwX = %.3f", Pw.x);
		ImGui::Text("PwY = %.3f", Pw.y);
		ImGui::Text("PwZ = %.3f", Pw.z);

		float Angle[6] = { 0,0,0,0,0,0 };
		Angle[0] = atan2f(Pw.z, -Pw.x)/PI*180.0f;
		ImGui::Text("Angle1 = %.3f", Angle[0]);

		float x = sqrt(Pw.x * Pw.x + Pw.z * Pw.z);
		float y = Pw.y-290.0f;
		float temp = (float)acos((y * y + x * x - 169004.0f) / 167403.484f);
		Angle[2] = (temp + (float)atan2f(70.0f, 302.0f)) / PI * 180.0f - 90.0f;
		Angle[1] = atan2f((-y * 310.006f * sin(temp) + x * (270.0f + 310.006f * cos(temp))) , (x * 310.006f * sin(temp) + y * (270.0f + 310.006f * cos(temp))))*180.0f/PI;
		ImGui::Text("Angle2 = %.3f", Angle[1]);
		ImGui::Text("Angle3 = %.3f", Angle[2]);

		
		
		glm::mat4 T13 = glm::mat4(1);
		T13 = glm::translate(T13, glm::vec3(-302.0f, 630.0f, 0.0f));
		T13	= glm::rotate(T13, glm::radians(Angle[0]), glm::vec3(0.0f, 1.0f, 0.0f));
		//T13 = glm::translate(T13, glm::vec3(0.0f, 290.0f, 0.0f));
		T13 = glm::rotate(T13, glm::radians(Angle[1]), glm::vec3(0.0f, 0.0f, 1.0f));
		//T13 = glm::translate(T13, glm::vec3(0.0f, -290.0f, 0.0f));
		//T13 = glm::translate(T13, glm::vec3(0.0f, 560.0f, 0.0f));
		T13 = glm::rotate(T13, glm::radians(Angle[2]), glm::vec3(0.0f, 0.0f, 1.0f));
		//T13 = glm::translate(T13, glm::vec3(0.0f, -560.0f, 0.0f));

		glm::mat4 T16 = glm::mat4(1);
		T16 = glm::translate(T16, Pos);
		T16 = glm::rotate(T16, glm::radians(Eular.z), glm::vec3(0, 0, 1));
		T16 = glm::rotate(T16, glm::radians(Eular.y), glm::vec3(0, 1, 0));
		T16 = glm::rotate(T16, glm::radians(Eular.x), glm::vec3(1, 0, 0));

		glm::mat4 T35 = glm::inverse(T13) * T16;
		Angle[3] = atan2f(-T35[2].y,T35[2].z) / PI * 180.0f;
		Angle[4] = atan2f(-T35[1].x,T35[0].x) / PI * 180.0f;
		ImGui::Text("Angle4 = %.3f", Angle[3]);
		ImGui::Text("Angle5 = %.3f", Angle[4]);
		

		if (Angle[0] < 165.0f && Angle[0] > -165.0f)
		{
			*Application::Get().irb120->SetAngle(1) = Angle[0];	
		}
		if (Angle[1] < 110.0f && Angle[1] > -110.0f)
		{
			*Application::Get().irb120->SetAngle(2) = Angle[1];
		}
		if (Angle[2] < 70.0f && Angle[2] > -110.0f)
		{
			*Application::Get().irb120->SetAngle(3) = Angle[2];
		}
		if (Angle[3] < 160.0f && Angle[3] > -160.0f)
		{
			*Application::Get().irb120->SetAngle(4) = Angle[3];
		}
		if (Angle[4] < 120.0f && Angle[4] > -120.0f)
		{
			*Application::Get().irb120->SetAngle(5) = Angle[4];
		}

		if (ImGui::Button("Solve"))
		{
			Application::Get().AngleChanged = true;
		}

		

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

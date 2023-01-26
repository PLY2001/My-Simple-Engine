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

	//glm::vec3 Pos = glm::vec3(-5.18f,6.3f,0.0f);
	//glm::vec3 Eular = glm::vec3(0.0f, 0.0f, 0.0f);


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
		
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu(u8"  文件  "))
			{
				if (ImGui::MenuItem(u8"读取场景"))
				{

					Application::Get().objects.reset(new Objects(Application::Get().modelmap));
					Application::Get().insbos.reset(new InstanceBufferObjects());
					
					if (Application::Get().objects->LoadScene())
					{
						
						Application::Get().insbos->AddObjects(Application::Get().objects);
						Loaded = true;

						

					}
				}
				


				if (ImGui::MenuItem(u8"保存场景"))
				{
					if (Application::Get().objects->SaveScene())
					{
						Save = true;
						//ImGui::OpenPopup(u8"保存提示");
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu(u8"编辑"))
			{
				if (ImGui::MenuItem(u8"清空全部"))
				{
					Application::Get().objects.reset(new Objects(Application::Get().modelmap));
					Application::Get().insbos.reset(new InstanceBufferObjects());
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (Loaded||Save)
		{
			ImGui::OpenPopup(u8"提示");
		}
		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(u8"提示", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if(Loaded)
			{
				ImGui::Text(u8"读取场景成功");
			}
			if(Save)
			{
				ImGui::Text(u8"保存场景成功");
			}
			ImGui::Separator();
			if (ImGui::Button(u8"确定", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				Loaded = false;
				Save = false;
			}
			ImGui::EndPopup();
		}
		

		
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);
		
		//自定义imgui窗口
		ImGui::Begin(u8"编辑");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text(u8"切换渲染风格");           
		ImGui::RadioButton(u8"真实", (int*)&Application::Get().graphicmode , 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"描边", (int*)&Application::Get().graphicmode, 1);
		ImGui::SameLine();
		ImGui::RadioButton(u8"无阴影", (int*)&Application::Get().graphicmode, 2);
		
		if (Application::Get().graphicmode == Application::GraphicMode::Normal)
		{
			ImGui::Checkbox(u8"阴影高斯模糊", &Application::Get().ShadowGaussian);
		}
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
			if (ImGui::Button(u8"删除所选模型"))
				Application::Get().objects->ReduceAmount();

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
				if (ImGui::SliderFloat("PosX", Application::Get().objects->SetHandPos(0), -652.0f * Scale.x, 652.0f * Scale.x))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				if (ImGui::SliderFloat("PosY", Application::Get().objects->SetHandPos(1), -184.0f * Scale.y, 1054.0f * Scale.y))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				if (ImGui::SliderFloat("PosZ", Application::Get().objects->SetHandPos(2), -652.0f * Scale.z, 652.0f * Scale.z))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				if (ImGui::SliderFloat("EularX", Application::Get().objects->SetHandEular(0), -180.0f, 180.0f))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				if (ImGui::SliderFloat("EularY", Application::Get().objects->SetHandEular(1), -180.0f, 180.0f))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				if (ImGui::SliderFloat("EularZ", Application::Get().objects->SetHandEular(2), -180.0f, 180.0f))
				{
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
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
		//ImGui::InputFloat(u8"radius", &Application::Get().radius);
		//ImGui::InputFloat(u8"bias1", &Application::Get().bias1);
		//ImGui::InputFloat(u8"bias2", &Application::Get().bias2);
		//ImGui::InputFloat(u8"bias3", &Application::Get().bias3);
		//ImGui::InputFloat(u8"bias4", &Application::Get().bias4);
		//ImGui::InputFloat(u8"width0", &Application::Get().width0);
		//ImGui::InputFloat(u8"height0", &Application::Get().height0);
		//ImGui::InputFloat(u8"width1", &Application::Get().width1);
		//ImGui::InputFloat(u8"height1", &Application::Get().height1);
		if (ImGui::Button(u8"设点"))
		{
			Application::Get().objects->GetMyAnimation().SetPathPos(irb120Pos * 10.0f);
			Application::Get().objects->GetMyAnimation().SetPathRotate(irb120Rotate * PI / 180.0f);
			Application::Get().objects->GetMyAnimation().SetPathHandPos(Application::Get().objects->GetHandPos());
			Application::Get().objects->GetMyAnimation().SetPathHandEular(Application::Get().objects->GetHandEular());
			Application::Get().objects->GetMyAnimation().SetPathTime(PathTime);
		}
		ImGui::SameLine();
		ImGui::InputFloat(u8"时间间隔", &PathTime);
		if (ImGui::Button(u8"播放"))
		{
			for (int i = 0; i < Application::Get().objects->GetObjectAmount(); i++)
			{
				for (int j = 0; j < Application::Get().objects->GetAmount(i); j++)
				{
					if(Application::Get().objects->GetAnimation(i, j).HaveAnimation)
					{
						Application::Get().objects->GetAnimation(i, j).Reset();
						Application::Get().objects->GetAnimation(i, j).Playing = true;
						Application::Get().objects->ChangePos(Application::Get().objects->GetAnimation(i, j).GetPathKeyPos(0) - Application::Get().objects->GetPos(i, j), i, j);
						Application::Get().objects->ChangeRotate(Application::Get().objects->GetAnimation(i, j).GetPathKeyRotate(0) - Application::Get().objects->GetRotate(i, j), 1,i,j);
						Application::Get().objects->ChangeHandPos(Application::Get().objects->GetAnimation(i, j).GetPathKeyHandPos(0),i,j);
						Application::Get().objects->ChangeHandEular(Application::Get().objects->GetAnimation(i, j).GetPathKeyHandEular(0),i,j);
					}
				}
			}
		}
		ImGui::End();




		ImGui::Begin(u8"设备栏");
		if (ImGui::Button(u8"ABB IRB120 六轴机械臂"))
		{
			bool finded = false;
			for (auto it = Application::Get().objects->objects.begin(); it != Application::Get().objects->objects.end(); ++it)
			{
				if ((*it).m_Name == "irb120")
				{
					Application::Get().objects->AddAmount("irb120");
					finded = true;
				}
			}
			if (!finded)
			{
				Application::Get().objects->AddObject("irb120", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.01f, 0.01f, 0.01f), true);
				Application::Get().insbos->AddObject(Application::Get().objects);
			}
			
		}
		if (ImGui::Button(u8"传送带"))
		{
			bool finded = false;
			for (auto it = Application::Get().objects->objects.begin(); it != Application::Get().objects->objects.end(); ++it)
			{
				if ((*it).m_Name == "belt")
				{
					Application::Get().objects->AddAmount("belt");
					finded = true;
				}
			}
			if (!finded)
			{
				Application::Get().objects->AddObject("belt", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f), false);
				Application::Get().insbos->AddObject(Application::Get().objects);
			}

		}
		if (ImGui::Button(u8"AGV运输车"))
		{
			bool finded = false;
			for (auto it = Application::Get().objects->objects.begin(); it != Application::Get().objects->objects.end(); ++it)
			{
				if ((*it).m_Name == "AVG")
				{
					Application::Get().objects->AddAmount("AVG");
					finded = true;
				}
			}
			if (!finded)
			{
				Application::Get().objects->AddObject("AVG", glm::vec3(0, 0.8f, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f), false);
				Application::Get().insbos->AddObject(Application::Get().objects);
			}

		}
		if (ImGui::Button(u8"塑料盒"))
		{
			bool finded = false;
			for (auto it = Application::Get().objects->objects.begin(); it != Application::Get().objects->objects.end(); ++it)
			{
				if ((*it).m_Name == "box")
				{
					Application::Get().objects->AddAmount("box");
					finded = true;
				}
			}
			if (!finded)
			{
				Application::Get().objects->AddObject("box", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.01f, 0.01f, 0.01f), false);
				Application::Get().insbos->AddObject(Application::Get().objects);
			}

		}
		if (ImGui::Button(u8"吹塑机"))
		{
			bool finded = false;
			for (auto it = Application::Get().objects->objects.begin(); it != Application::Get().objects->objects.end(); ++it)
			{
				if ((*it).m_Name == "machine")
				{
					Application::Get().objects->AddAmount("machine");
					finded = true;
				}
			}
			if (!finded)
			{
				Application::Get().objects->AddObject("machine", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.45f, 0.45f, 0.45f), false);
				Application::Get().insbos->AddObject(Application::Get().objects);
			}

		}
		
		
		ImGui::End();

		
	}

}

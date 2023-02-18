#include "hzpch.h"
#include "ImGuiLayer.h"

//imgui+glfw ��Ҫ�����·�3���ļ�
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


	void ImGuiLayer::OnAttach()//ImGui��ʼ��
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
		io.Fonts->AddFontFromFileTTF("res/fonts/msyh.ttc", 30.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());//���塢��С

		

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

	void ImGuiLayer::OnDetach()//ImGui����
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()//ImGui����ǰ��׼��
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()//ָ����������ݺ󣬿�ʼ����ImGui
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// ���ƽ���
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

	void ImGuiLayer::OnImGuiRender()//ImGui��������
	{
		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);//��ʾimguiʾ������
		
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu(u8"  �ļ�  "))
			{
				if (ImGui::MenuItem(u8"��ȡ����"))
				{

					//Application::Get().objects.reset(new Objects(Application::Get().modelmap));
					//Application::Get().insbos.reset(new InstanceBufferObjects());
					
					//if (Application::Get().objects->LoadScene())
					//{
						
						//Application::Get().insbos->AddObjects(Application::Get().objects);
						ToLoad = true;

						

					//}
				}
				


				if (ImGui::MenuItem(u8"���泡��"))
				{
					//if (Application::Get().objects->SaveScene())
					//{
						ToSave = true;
						//ImGui::OpenPopup(u8"������ʾ");
					//}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu(u8"�༭"))
			{
				if (ImGui::MenuItem(u8"���ȫ��"))
				{
					Application::Get().objects.reset(new Objects(Application::Get().modelmap));
					Application::Get().insbos.reset(new InstanceBufferObjects());
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (ToLoad||ToSave)
		{
			ImGui::OpenPopup(u8"��ʾ");
		}
		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(u8"��ʾ", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if(ToLoad)
			{
				ImGui::Text(u8"��ѡ���ȡ��λ");
				ImGui::RadioButton(u8"��λ0", &LoadAddress, 0);
				ImGui::RadioButton(u8"��λ1", &LoadAddress, 1);
				ImGui::RadioButton(u8"��λ2", &LoadAddress, 2);
				if (Loaded)
				{
					ImGui::Text(u8"��ȡ�����ɹ�");
				}
			}
			if(ToSave)
			{
				ImGui::Text(u8"��ѡ�񱣴浵λ");
				ImGui::RadioButton(u8"��λ0", &SaveAddress, 0);
				ImGui::RadioButton(u8"��λ1", &SaveAddress, 1);
				ImGui::RadioButton(u8"��λ2", &SaveAddress, 2);
				if (Saved)
				{
					ImGui::Text(u8"���泡���ɹ�");
				}
				
			}
			ImGui::Separator();
			if(ToSave)
			{
				if (ImGui::Button(u8"����", ImVec2(120, 0)))
				{
					if (Application::Get().objects->SaveScene(SaveAddress))
					{
						Saved = true;
					}
				}
			}
			if (ToLoad)
			{
				if (ImGui::Button(u8"��ȡ", ImVec2(120, 0)))
				{
					Application::Get().objects.reset(new Objects(Application::Get().modelmap));
					Application::Get().insbos.reset(new InstanceBufferObjects());
					if (Application::Get().objects->LoadScene(LoadAddress))
					{
						Application::Get().insbos->AddObjects(Application::Get().objects);
						Loaded = true;
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"�ر�", ImVec2(120, 0)))
			{

				ImGui::CloseCurrentPopup();
				ToLoad = false;
				ToSave = false;
				Loaded = false;
				Saved = false;
			}
			ImGui::EndPopup();
		}
		

		
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);
		
		//�Զ���imgui����
		ImGui::Begin(u8"�༭��");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text(u8"�л���Ⱦ���");           
		ImGui::RadioButton(u8"��ʵ", (int*)&Application::Get().graphicmode , 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"���", (int*)&Application::Get().graphicmode, 1);
		ImGui::SameLine();
		ImGui::RadioButton(u8"����Ӱ", (int*)&Application::Get().graphicmode, 2);
		
		if (Application::Get().graphicmode == Application::GraphicMode::Normal)
		{
			ImGui::Checkbox(u8"��Ӱ��˹ģ��", &Application::Get().ShadowGaussian);
		}
		ImGui::Text(u8"�л��ƹ�����");
		ImGui::RadioButton(u8"ֱ���", (int*)&Application::Get().lightmode, 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"���Դ", (int*)&Application::Get().lightmode, 1);
		if (Application::Get().objects->GetChoosedIndex() > -1)
		{
			if (ImGui::Button(u8"������ѡģ��"))
				Application::Get().objects->AddAmount();
			ImGui::SameLine();
			ImGui::Text(u8"��ѡģ���ܼ� %d ��", Application::Get().objects->GetMyAmount());
			if (ImGui::Button(u8"ģ�黯����"))
				Application::Get().ModularCopy = true;
			if (ImGui::Button(u8"ɾ����ѡģ��"))
			{
				if (Application::Get().objects->GetMyAmount() == 1)
				{
					Application::Get().insbos->ReduceObject(Application::Get().objects->GetChoosedObjectIndex());
					Application::Get().objects->ReduceObjectAmount();
				}
				else
				{
					Application::Get().objects->ReduceAmount();
				}
				
			}
				

			if ((int)Application::Get().objects->GetControlMode() > 0)
			{
				ImGui::Text(u8"�����˿���");
				ImGui::RadioButton(u8"�����˶�ѧ", Application::Get().objects->GetControlModeAddress(), 1);
				ImGui::SameLine();
				ImGui::RadioButton(u8"�����˶�ѧ", Application::Get().objects->GetControlModeAddress(), 2);
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
				HandPos = Application::Get().objects->GetHandPos()/10.0f;
				HandEular = Application::Get().objects->GetHandEular();
				if (ImGui::SliderFloat("HandPosX", (float*)&HandPos.x, -652.0f * Scale.x/10.0f, 652.0f * Scale.x / 10.0f))
				{
					Application::Get().objects->ChangeHandPos(HandPos*10.0f);
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				
				if (ImGui::SliderFloat("HandPosY", (float*)&HandPos.y, -184.0f * Scale.y / 10.0f, 1054.0f * Scale.y / 10.0f))
				{
					Application::Get().objects->ChangeHandPos(HandPos * 10.0f);
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				
				if (ImGui::SliderFloat("HandPosZ", (float*)&HandPos.z, -652.0f * Scale.z / 10.0f, 652.0f * Scale.z / 10.0f))
				{
					Application::Get().objects->ChangeHandPos(HandPos * 10.0f);
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				
				if (ImGui::InputFloat3("HandPos", (float*)&HandPos))
				{
					Application::Get().objects->ChangeHandPos(HandPos * 10.0f);
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}

				if (ImGui::SliderFloat("HandEularX", (float*)&HandEular.x, -180.0f, 180.0f))
				{
					Application::Get().objects->ChangeHandEular(HandEular);
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				
				if (ImGui::SliderFloat("HandEularY", (float*)&HandEular.y, -180.0f, 180.0f))
				{
					Application::Get().objects->ChangeHandEular(HandEular);
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				
				if (ImGui::SliderFloat("HandEularZ", (float*)&HandEular.z, -180.0f, 180.0f))
				{
					Application::Get().objects->ChangeHandEular(HandEular);
					Application::Get().AngleChanged = Application::Get().objects->SolveAngle();
				}
				
				if (ImGui::InputFloat3("HandEular", (float*)&HandEular))
				{
					Application::Get().objects->ChangeHandEular(HandEular);
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
		
		ImGui::Text(u8"��������� = ( %.3f , %.3f )", Application::Get().GetClickPos().x, Application::Get().GetClickPos().y);
		ImGui::Text(u8"��ǰѡ��ģ������ = %d", Application::Get().objects->GetChoosedIndex());
		ImGui::Text(u8"AABBmin( %.3f , %.3f , %.3f )", Application::Get().objects->GetAABBMinPos().x/10.0f, Application::Get().objects->GetAABBMinPos().y / 10.0f, Application::Get().objects->GetAABBMinPos().z / 10.0f);
		ImGui::Text(u8"AABBmax( %.3f , %.3f , %.3f )", Application::Get().objects->GetAABBMaxPos().x / 10.0f, Application::Get().objects->GetAABBMaxPos().y / 10.0f, Application::Get().objects->GetAABBMaxPos().z / 10.0f);
		if (Application::Get().objects->GetChoosedIndex() > -1)
		{
			Pos = Application::Get().objects->GetPos()/10.0f;
			
			Rotate = Application::Get().objects->GetRotate() * glm::vec3(180.0f / PI);;
			if (ImGui::InputFloat3(u8"ƽ��(m)", (float*)&Pos))
			{
				Application::Get().objects->ChangePos(Pos*10.0f - Application::Get().objects->GetPos());
			}
			if (ImGui::InputFloat3(u8"��ת(degree)", (float*)&Rotate))
			{
				LastRotate = Application::Get().objects->GetRotate() * glm::vec3(180.0f / PI);
				//glm::vec3 temp = Application::Get().objects->GetRotate();
				Application::Get().objects->ChangeRotateD(glm::vec3(Rotate.x,Rotate.y, Rotate.z)* PI / 180.0f);
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
 		ImGui::Text(u8"ÿ֡ƽ�� %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//ImGui::InputFloat(u8"bias", &Application::Get().bias);
		//ImGui::InputFloat(u8"radius", &Application::Get().radius);
		//ImGui::InputFloat(u8"bias1", &Application::Get().bias1);
		//ImGui::InputFloat(u8"bias2", &Application::Get().bias2);
		//ImGui::InputFloat(u8"bias3", &Application::Get().bias3);
		//ImGui::InputFloat(u8"bias4", &Application::Get().bias4);
// 		ImGui::InputFloat(u8"width0", &Application::Get().width0);
// 		ImGui::InputFloat(u8"height0", &Application::Get().height0);
// 		ImGui::InputFloat(u8"width1", &Application::Get().width1);
//  	ImGui::InputFloat(u8"height1", &Application::Get().height1);
		//ImGui::Checkbox("UIClicked", &Application::Get().UIClicked);
// 		ImGui::InputFloat(u8"ShadowRoundSize", &Application::Get().ShadowRoundSize);
// 		ImGui::InputFloat(u8"ShadowSoftSize", &Application::Get().ShadowSoftSize);
// 		ImGui::InputFloat(u8"biasmax", &Application::Get().BiasMax);
// 		ImGui::InputFloat(u8"biasmin", &Application::Get().BiasMin);
// 		ImGui::InputFloat(u8"shadowColorDepth", &Application::Get().shadowColorDepth);
//  	ImGui::InputFloat(u8"hbaoShadowColorDepth", &Application::Get().hbaoShadowColorDepth);
// 		ImGui::InputFloat(u8"basicP", &Application::Get().basicP);
// 		ImGui::InputFloat(u8"planeP", &Application::Get().planeP);
		//ImGui::InputFloat(u8"F0", &Application::Get().F0);
		//ImGui::InputFloat(u8"fp", &Application::Get().fp);
		
		

		ImGui::End();

		ImGui::Begin(u8"������");
		
		if (Application::Get().objects->GetChoosedIndex() > -1)
		{
			ImGui::RadioButton(u8"ֱ�߹켣", (int*)&Application::Get().objects->GetMyAnimation().pathmode, 0);
			ImGui::SameLine();
			ImGui::RadioButton(u8"Բ���켣", (int*)&Application::Get().objects->GetMyAnimation().pathmode, 1);
			if (Application::Get().objects->GetMyAnimation().pathmode == PathMode::Circle)
			{
				ImGui::SameLine();
				ImGui::InputFloat3(u8"Բ��Բ��", (float*)&CircleCenter);

			}
			ImGui::SetNextItemWidth(100);
			ImGui::InputFloat(u8"ʱ����", &PathTime, 0.0f, 10000.0f, "%.1f");
			//ImGui::PushItemWidth(10);
			ImGui::SameLine();
			if (ImGui::Button(u8"���"))
			{
				Application::Get().objects->GetMyAnimation().SetPathPos(Pos * 10.0f);
				Application::Get().objects->GetMyAnimation().SetPathRotate(Rotate * PI / 180.0f);
				Application::Get().objects->GetMyAnimation().SetPathHandPos(Application::Get().objects->GetHandPos());
				Application::Get().objects->GetMyAnimation().SetPathHandEular(Application::Get().objects->GetHandEular());
				Application::Get().objects->GetMyAnimation().SetPathTime(PathTime);
				Application::Get().objects->GetMyAnimation().SetPathMode(CircleCenter*10.0f);
			}
			ImGui::SameLine();
			
			if (ImGui::Button(u8"ɾ��"))
			{
				Application::Get().objects->GetMyAnimation().RemovePath();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"����"))
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
						//for(int k = Application::Get().objects->GetAnimation(i, j).GetPathKeySize()-1;k>-1;k--)
						Application::Get().objects->ChangeRotateD(Application::Get().objects->GetAnimation(i, j).GetPathKeyRotate(0),i,j);
						Application::Get().objects->ChangeHandPos(Application::Get().objects->GetAnimation(i, j).GetPathKeyHandPos(0),i,j);
						Application::Get().objects->ChangeHandEular(Application::Get().objects->GetAnimation(i, j).GetPathKeyHandEular(0),i,j);
					}
				}
			}
		}

		ImGui::SetNextItemWidth(100);
		ImGui::InputFloat(u8"�ܶ���ʱ��", &TotalTime, 0.0f, 10000.0f, "%.1f");

		

		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
		static int freeze_cols = 1;
		static int freeze_rows = 1;
		// When using ScrollX or ScrollY we need to specify a size for our table container!
		// Otherwise by default the table will fit all available space, like a BeginChild() call.
		ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
		int TotalObjAmount = Application::Get().objects->GetObjectAmount();
		int TotalAmount = 0;
		for (int i = 0; i < TotalObjAmount; i++)
		{
			TotalAmount += Application::Get().objects->objects[i].m_Amount;
		}
		if (ImGui::BeginTable("KeyTimes", 1 + TotalAmount, flags, outer_size))
		{
			ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
			ImGui::TableSetupColumn(u8"ʱ��ڵ�", ImGuiTableColumnFlags_NoHide); // Make the first column not hideable to match our use of TableSetupScrollFreeze()
			
			for (int i = 0; i < TotalObjAmount; i++)
			{
				for (int j = 0; j < Application::Get().objects->objects[i].m_Amount; j++)
				{
					std::string name(Application::Get().objects->objects[i].m_Name);
					
					// ׼�����ݸ�ʽ���ַ�����
					std::stringstream fmt;                       
					// ���ַ�����
					fmt << name << ":" << j;
					std::string namej = fmt.str();
					ImGui::TableSetupColumn(namej.c_str());
					
					Application::Get().objects->objects[i].m_Anima[j].Key_index = 0;
				}
			}
			ImGui::TableHeadersRow();
			

			for (int row = 0; row < (int)(TotalTime / 0.5 + 1); row++)
			{
				
				int column = 0;
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(column);
				ImGui::Text(u8"%.1f��", row * 0.5f);
				for (int i = 0; i < TotalObjAmount; i++)
				{
					for (int j = 0; j < Application::Get().objects->objects[i].m_Amount; j++)
					{
						column++;
						ImGui::TableSetColumnIndex(column);
						if (Application::Get().objects->objects[i].m_Anima[j].HaveAnimation)
						{
							int index = Application::Get().objects->objects[i].m_Anima[j].Key_index;
							if(index < Application::Get().objects->objects[i].m_Anima[j].GetPathKeySize())
							{
								float temp1 = Application::Get().objects->objects[i].m_Anima[j].GetPathTotalTime(index);
								float temp2 = row * 0.5f;
								if (Application::Get().objects->objects[i].m_Anima[j].GetPathTotalTime(index) == row * 0.5f)
								{

									std::string ButtomName = u8"��##" + std::to_string(index) + u8":" + std::to_string(column);//��ÿ��buttomȡ��Ϊ"��##1:1"��"��##1:2"...��Ϊ�˱���������ͬ����ͻ����ʵ�ʲ�����ʾ##1:1��##1:2�ȣ�
									if (ImGui::Button(ButtomName.c_str()))
									{
										Application::Get().objects->ChangePos(Application::Get().objects->GetAnimation(i, j).GetPathKeyPos(index) - Application::Get().objects->GetPos(i, j), i, j);
										Application::Get().objects->ChangeRotateD(Application::Get().objects->GetAnimation(i, j).GetPathKeyRotate(index), i, j);
										Application::Get().objects->ChangeHandPos(Application::Get().objects->GetAnimation(i, j).GetPathKeyHandPos(index), i, j);
										Application::Get().objects->ChangeHandEular(Application::Get().objects->GetAnimation(i, j).GetPathKeyHandEular(index), i, j);
										if (Application::Get().objects->objects[i].m_HaveAngle)
										{
											if (Application::Get().objects->SolveAngle(i, j))
											{
												Application::Get().objects->ChangeAngle(i, j);
											}
										}
									}
									Application::Get().objects->objects[i].m_Anima[j].Key_index++;


								}
								else
									ImGui::Text(u8"��");
							}
							else
								ImGui::Text(u8"��");
						}
						else
							ImGui::Text(u8"��");
					}
				}
				
			}
			ImGui::EndTable();
		}

		ImGui::End();




		ImGui::Begin(u8"�豸��");
		if (ImGui::Button(u8"ABB IRB120 �����е��"))
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
		if (ImGui::Button(u8"�ʹ��ʹ�"))
		{
			bool finded = false;
			for (auto it = Application::Get().objects->objects.begin(); it != Application::Get().objects->objects.end(); ++it)
			{
				if ((*it).m_Name == "belt1")
				{
					Application::Get().objects->AddAmount("belt1");
					finded = true;
				}
			}
			if (!finded)
			{
				Application::Get().objects->AddObject("belt1", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f), false);
				Application::Get().insbos->AddObject(Application::Get().objects);
			}

		}
		if (ImGui::Button(u8"�д��ʹ�"))
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
		if (ImGui::Button(u8"�ߴ��ʹ�"))
		{
			bool finded = false;
			for (auto it = Application::Get().objects->objects.begin(); it != Application::Get().objects->objects.end(); ++it)
			{
				if ((*it).m_Name == "belt2")
				{
					Application::Get().objects->AddAmount("belt2");
					finded = true;
				}
			}
			if (!finded)
			{
				Application::Get().objects->AddObject("belt2", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.005f, 0.005f, 0.005f), false);
				Application::Get().insbos->AddObject(Application::Get().objects);
			}

		}
		if (ImGui::Button(u8"AGV���䳵"))
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
		if (ImGui::Button(u8"ת�˺�"))
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
		if (ImGui::Button(u8"���ܻ�"))
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
		if (ImGui::Button(u8"����ֿ�"))
		{
			bool finded = false;
			for (auto it = Application::Get().objects->objects.begin(); it != Application::Get().objects->objects.end(); ++it)
			{
				if ((*it).m_Name == "storage")
				{
					Application::Get().objects->AddAmount("storage");
					finded = true;
				}
			}
			if (!finded)
			{
				Application::Get().objects->AddObject("storage", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f), false);
				Application::Get().insbos->AddObject(Application::Get().objects);
			}

		}

		ImGui::Text(u8" ");
		ImGui::Text(u8"��������Ʒ");
		for (int i = 0; i < TotalObjAmount; i++)
		{
			for (int j = 0; j < Application::Get().objects->objects[i].m_Amount; j++)
			{
				std::string name(Application::Get().objects->objects[i].m_Name);

				// ׼�����ݸ�ʽ���ַ�����
				std::stringstream fmt;
				// ���ַ�����
				fmt << name << ":" << j;
				std::string namej = fmt.str();
				if (ImGui::Button(namej.c_str()))
				{
					Application::Get().objects->SetChoosedIndex(i, j);
				}
			}
		}

		
		
		ImGui::End();

		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureMouse)
		{
			Application::Get().UIClicked = false;//��겻��UI��
		}
		else
		{
			Application::Get().UIClicked = true;//�����UI��
		}

// 		if (Application::Get().mousemode == Application::MouseMode::Disable)
// 		{
// 			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
// 		}
		
	}



}

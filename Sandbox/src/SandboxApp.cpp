#include <Hazel.h>
#include "imgui/imgui.h"

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
bool show_demo_window = true;
bool show_another_window = false;
static float f = 0.0f;
static int counter = 0;

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		//HZ_CLIENT_INFO("ExampleLayer::Update");
	}
	
	virtual void OnImGuiRender() override
	{
		//自定义imgui窗口
// 		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
// 
// 		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
// 		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
// 		ImGui::Checkbox("Another Window", &show_another_window);
// 
// 		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
// 		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
// 
// 		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
// 			counter++;
// 		ImGui::SameLine();
// 		ImGui::Text("counter = %d", counter);
// 
// 		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
// 		ImGui::End();
	}
	
	void OnEvent(Hazel::Event& event) override
	{
#ifdef HZ_DEBUG
		HZ_CLIENT_TRACE("{0}", event);
#endif // DEBUG

		
	}

	


};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		PushOverlay(new ExampleLayer());//示例层，显示外设输入事件，第三层
		//PushOverlay(new Hazel::ImGuiLayer());//ImGui层
	}
	~Sandbox()
	{

	}
};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}

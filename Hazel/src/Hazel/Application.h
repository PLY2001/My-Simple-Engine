#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "Hazel/LayerStack.h"
#include "Hazel/ImGui/ImGuiLayer.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/Model.h"
#include "Hazel/Renderer/Texture.h"
#include "Hazel/Renderer/Skybox.h"
#include "Hazel/Renderer/Light.h"
#include "Hazel/Renderer/Camera.h"
#include "Hazel/Renderer/OpenGLRendererAPI.h"
//#include "Objects/ABBIRB120.h"
#include "Objects/Arrow.h"
#include "Hazel/Renderer/AABB.h"
#include "Hazel/RenderLayers/ControlLayer.h"
//#include "Objects/Belt.h"
#include "Objects/Objects.h"
#include "Hazel/Renderer/InstanceBufferObjects.h"

namespace Hazel
{
	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application() = default;

		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	public:

		//unsigned int ModelCount = 1;//模型的数量

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		ControlLayer* m_ControlLayer;
		bool m_Running = true;
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnCameraRotate(MouseMovedEvent& e);
		bool OnCameraZoom(MouseScrolledEvent& e);
		bool OnMouseButtonEvent(MouseButtonPressedEvent& e);
		bool OnMousePos(MouseMovedEvent& e);
		bool OnMouseReleaseEvent(MouseButtonReleasedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		

		LayerStack m_LayerStack;

		//shader
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Shader> OutlineShader;
		std::shared_ptr<Shader> ScreenPostShader;
		std::shared_ptr<Shader> TShader;
		std::shared_ptr<Shader> SkyShader;
		std::shared_ptr<Shader> ScreenBasicShader;
		std::shared_ptr<Shader> ShadowMapShader;
		std::shared_ptr<Shader> ShadowDrawShader;
		std::shared_ptr<Shader> GaussianShader;
		std::shared_ptr<Shader> ShadowCubeMapShader;
		std::shared_ptr<Shader> ShadowCubeDrawShader;
		std::shared_ptr<Shader> BricksShader;
		std::shared_ptr<Shader> BloomShader;
		std::shared_ptr<Shader> AABBShader;
		//std::shared_ptr<Shader> ArrowShader;

		//灯光
		float count = 0;
		std::unique_ptr<Light> PointLight;
		std::unique_ptr<Light> DirectLight;

		

		//创建实例化数组
		
		//std::vector<std::unique_ptr<InstanceBuffer>> insbo;//irb120
		//std::vector<std::unique_ptr<InstanceBuffer>> insbobelt;//belt
		//std::vector<std::unique_ptr<InstanceBuffer>> insboAVG;//AVG
		std::unique_ptr<InstanceBuffer> insboplane;//plane

		//创建Uniform缓冲对象
		std::unique_ptr<UniformBuffer> ubo;
		

		//shadow map 分辨率
		unsigned int ShadowMapWidth = 2048;
		unsigned int ShadowMapHeight = 2048;

		//创建帧缓冲1
		std::unique_ptr<FrameBuffer> framebuffer1;
		unsigned int QuadID;//绘制主要内容的平面
		//创建帧缓冲2
		std::unique_ptr<FrameBuffer> framebuffer2;
		//创建帧缓冲MSAA
		std::unique_ptr<FrameBuffer> framebufferMSAA;
		//创建帧缓冲3
		std::unique_ptr<FrameBuffer> framebuffer3;
		//创建帧缓冲shadowmap
		std::unique_ptr<FrameBuffer> framebufferSM;
		//创建帧缓冲4
		std::unique_ptr<FrameBuffer> framebuffer4;
		unsigned int QuadID4;//绘制阴影的平面
		//创建帧缓冲5
		std::unique_ptr<FrameBuffer> framebuffer5;
		//创建帧缓冲shadowcubemap
		std::unique_ptr<FrameBuffer> framebufferSCM;
		//创建帧缓冲6
		//std::unique_ptr<FrameBuffer> framebuffer6;
		//创建帧缓冲7
		//std::unique_ptr<FrameBuffer> framebuffer7;
		//unsigned int QuadID7;

		/*IRB120*/
		
		/*平面*/
		std::shared_ptr<Model> plane;

		//std::shared_ptr<Model> ArrowModel;
		

		//创建天空盒
		std::unique_ptr<Skybox> skybox;
		unsigned int SkyboxID;

		//camera
		std::unique_ptr<Camera> camera;
		float deltaTime = 0;//每次循环耗时
		float lastTime = 0;
		

		//模式
		enum class GraphicMode { Normal = 0, Outline = 1 };
		enum class LightMode { Direct = 0, Point = 1 };
		

		

		enum class MouseMode { Disable = 0, Enable = 1 };
		

		

		std::unique_ptr<AABB> aabb;

	private:
		static Application* s_Instance;//单例，就需要在类里面直接声明当前类的唯一实例

	public:
		GraphicMode graphicmode = GraphicMode::Normal;
		LightMode lightmode = LightMode::Direct;
		MouseMode mousemode = MouseMode::Disable;

		
		bool AngleChanged = false;

		//std::unique_ptr<ABBIRB120> irb120;
		//std::unique_ptr<Belt> belt;
		std::shared_ptr<Objects> objects;

		inline glm::vec2 GetClickPos() { return ClickPos; }

		//bool Choosed = false;
		//int index = 0;

		//std::unique_ptr<Arrow> arrow;
		bool ToMove = false;
		//bool Choosed = false;
		int axis = 0;

		bool first = true;
		glm::vec3 LastWorldClickPos = glm::vec3(1);
		//glm::vec3 LastArrowPos = glm::vec3(1);

		std::vector<int> shaderIDs;
		glm::vec2 MousePos = glm::vec2(0);
		glm::vec2 ClickPos = glm::vec2(0);

		//创建变换矩阵

		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;

		std::unique_ptr<InstanceBufferObjects> insbos;
		std::shared_ptr<Model> IRB120Model;//读取模型，目录从当前项目根目录开始，或者生成的exe根目录。需将noise.jpg复制到每一个模型旁边。
		std::shared_ptr<Model> BeltModel;//读取模型，目录从当前项目根目录开始，或者生成的exe根目录。需将noise.jpg复制到每一个模型旁边。
		std::shared_ptr<Model> AVGModel;//读取模型，目录从当前项目根目录开始，或者生成的exe根目录。需将noise.jpg复制到每一个模型旁边。

	};

	Application* CreateApplication();
}


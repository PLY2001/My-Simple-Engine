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
//#include "Hazel/Renderer/Texture.h"
#include "Hazel/Renderer/Skybox.h"
#include "Hazel/Renderer/Light.h"
#include "Hazel/Renderer/Camera.h"
#include "Hazel/Renderer/OpenGLRendererAPI.h"

//gltf
#include "gltf/Sample.h"

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
		bool GraphicMode_Normal = true;
		bool GraphicMode_Outline = false;
		bool LightMode_Direct = true;
		bool LightMode_Point = false;

		unsigned int ModelCount = 1;//ģ�͵�����

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnCameraRotate(MouseMovedEvent& e);
		bool OnCameraZoom(MouseScrolledEvent& e);
		bool OnKeyEvent(KeyPressedEvent& e);
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

		//�ƹ�
		float count = 0;
		std::shared_ptr<Light> PointLight;
		std::shared_ptr<Light> DirectLight;

		//�����任����
		std::vector<glm::mat4> ModelMatrices;//����ģ�͵�model�任��������
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;

		//����ʵ��������
		std::unique_ptr<InstanceBuffer> insbo;
		std::unique_ptr<InstanceBuffer> insboplane;

		//����Uniform�������
		std::unique_ptr<UniformBuffer> ubo;

		//shadow map �ֱ���
		unsigned int ShadowMapWidth = 1024;
		unsigned int ShadowMapHeight = 1024;

		//����֡����1
		std::unique_ptr<FrameBuffer> framebuffer1;
		unsigned int QuadID;
		//����֡����2
		std::unique_ptr<FrameBuffer> framebuffer2;
		//����֡����MSAA
		std::unique_ptr<FrameBuffer> framebufferMSAA;
		//����֡����3
		std::unique_ptr<FrameBuffer> framebuffer3;
		//����֡����shadowmap
		std::unique_ptr<FrameBuffer> framebufferSM;
		//����֡����4
		std::unique_ptr<FrameBuffer> framebuffer4;
		unsigned int QuadID4;
		//����֡����5
		std::unique_ptr<FrameBuffer> framebuffer5;
		//����֡����shadowcubemap
		std::unique_ptr<FrameBuffer> framebufferSCM;
		//����֡����6
		std::unique_ptr<FrameBuffer> framebuffer6;
		//����֡����7
		std::unique_ptr<FrameBuffer> framebuffer7;
		unsigned int QuadID7;

		/*����*/
		std::shared_ptr<Model> modelmesh;//��ȡģ�ͣ�Ŀ¼�ӵ�ǰ��Ŀ��Ŀ¼��ʼ���������ɵ�exe��Ŀ¼���轫noise.jpg���Ƶ�ÿһ��ģ���Աߡ�
		/*ƽ��*/
		std::shared_ptr<Model> plane;

		//������պ�
		std::unique_ptr<Skybox> skybox;
		unsigned int SkyboxID;

		//camera
		std::shared_ptr<Camera> camera;
		float deltaTime = 0;//ÿ��ѭ����ʱ
		float lastTime = 0;
		

		//ģʽ
		enum class GraphicMode { Normal = 0, Outline = 1 };
		enum class LightMode { Direct = 0, Point = 1 };
		GraphicMode graphicmode = GraphicMode::Normal;
		LightMode lightmode = LightMode::Direct;

		//��ֹ����
		unsigned int FrameCount = 0;

		enum class MouseMode { Disable = 0, Enable = 1 };
		MouseMode mousemode = MouseMode::Disable;

		//gltf
		Sample* gApplication = 0;

	private:
		static Application* s_Instance;//����������Ҫ��������ֱ��������ǰ���Ψһʵ��
	


	};

	Application* CreateApplication();
}


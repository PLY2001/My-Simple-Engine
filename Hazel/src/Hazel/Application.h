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
#include "Hazel/Renderer/Animation.h"
#include "Hazel/Renderer/Origin.h"

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

		//unsigned int ModelCount = 1;//ģ�͵�����

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
		//std::shared_ptr<Shader> ShadowDrawPlaneShader;
		std::shared_ptr<Shader> GaussianShader;
		std::shared_ptr<Shader> ShadowCubeMapShader;
		std::shared_ptr<Shader> ShadowCubeDrawShader;
		std::shared_ptr<Shader> BricksShader;
		std::shared_ptr<Shader> BloomShader;
		std::shared_ptr<Shader> AABBShader;
		//std::shared_ptr<Shader> ArrowShader;
		std::shared_ptr<Shader> CameraDepthMapShader;
		//std::shared_ptr<Shader> ShadowColorMapShader;
		std::shared_ptr<Shader> OriginShader;
		std::shared_ptr<Shader> PlaneShader;
		std::shared_ptr<Shader> LightShader;

		//�ƹ�
		float count = 0;
		std::unique_ptr<Light> PointLight;
		std::unique_ptr<Light> DirectLight;

		

		//����ʵ��������
		
		//std::vector<std::unique_ptr<InstanceBuffer>> insbo;//irb120
		//std::vector<std::unique_ptr<InstanceBuffer>> insbobelt;//belt
		//std::vector<std::unique_ptr<InstanceBuffer>> insboAVG;//AVG
		std::unique_ptr<InstanceBuffer> insboplane;//plane
		//std::unique_ptr<InstanceBuffer> insbolight;//light
		//����Uniform�������
		std::unique_ptr<UniformBuffer> ubo;
		

		//shadow map �ֱ���
		unsigned int ShadowMapWidth = 2048;
		unsigned int ShadowMapHeight = 2048;

		//����֡����1
		std::unique_ptr<FrameBuffer> framebuffer1;
		unsigned int QuadID;//������Ҫ���ݵ�ƽ��
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
		unsigned int QuadID4;//������Ӱ��ƽ��
		//����֡����5
		std::unique_ptr<FrameBuffer> framebuffer5;
		//����֡����shadowcubemap
		std::unique_ptr<FrameBuffer> framebufferSCM;
		//����֡����6
		//std::unique_ptr<FrameBuffer> framebuffer6;
		//����֡����7
		//std::unique_ptr<FrameBuffer> framebuffer7;
		//unsigned int QuadID7;
		std::unique_ptr<FrameBuffer> framebufferCM;
		//����֡����4
		//std::unique_ptr<FrameBuffer> framebufferColorSM;
		//std::unique_ptr<FrameBuffer> framebufferCM1;
		std::unique_ptr<FrameBuffer> framebuffer6;
		unsigned int QuadID6;//���ƹ��ε�ƽ��
		/*IRB120*/
		
		/*ƽ��*/
		std::shared_ptr<Model> plane;
		std::shared_ptr<Model> light;

		//std::shared_ptr<Model> ArrowModel;
		

		//������պ�
		std::unique_ptr<Skybox> skybox;
		unsigned int SkyboxID;

		//camera
		std::unique_ptr<Camera> camera;
		float deltaTime = 0;//ÿ��ѭ����ʱ
		float lastTime = 0;
		

		
		

		std::unique_ptr<Origin> origin;

		std::unique_ptr<AABB> aabb;

	private:
		static Application* s_Instance;//����������Ҫ��������ֱ��������ǰ���Ψһʵ��

	public:
		//ģʽ
		enum class GraphicMode { Normal = 0, Outline = 1, NoShadow = 2 };
		enum class LightMode { Direct = 0, Point = 1 };
		enum class MouseMode { Disable = 0, Enable = 1 };
		GraphicMode graphicmode = GraphicMode::Normal;
		LightMode lightmode = LightMode::Direct;
		MouseMode mousemode = MouseMode::Enable;

		
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
		//glm::vec2 LastMousePos = glm::vec2(0);

		//�����任����

		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;

		std::shared_ptr<InstanceBufferObjects> insbos;
		std::shared_ptr<Model> IRB120Model;//��ȡģ�ͣ�Ŀ¼�ӵ�ǰ��Ŀ��Ŀ¼��ʼ���������ɵ�exe��Ŀ¼���轫noise.jpg���Ƶ�ÿһ��ģ���Աߡ�
		std::shared_ptr<Model> Belt1Model;//��ȡģ�ͣ�Ŀ¼�ӵ�ǰ��Ŀ��Ŀ¼��ʼ���������ɵ�exe��Ŀ¼���轫noise.jpg���Ƶ�ÿһ��ģ���Աߡ�
		std::shared_ptr<Model> Belt2Model;//��ȡģ�ͣ�Ŀ¼�ӵ�ǰ��Ŀ��Ŀ¼��ʼ���������ɵ�exe��Ŀ¼���轫noise.jpg���Ƶ�ÿһ��ģ���Աߡ�
		std::shared_ptr<Model> BeltModel;//��ȡģ�ͣ�Ŀ¼�ӵ�ǰ��Ŀ��Ŀ¼��ʼ���������ɵ�exe��Ŀ¼���轫noise.jpg���Ƶ�ÿһ��ģ���Աߡ�
		std::shared_ptr<Model> AVGModel;//��ȡģ�ͣ�Ŀ¼�ӵ�ǰ��Ŀ��Ŀ¼��ʼ���������ɵ�exe��Ŀ¼���轫noise.jpg���Ƶ�ÿһ��ģ���Աߡ�
		std::shared_ptr<Model> BoxModel;
		std::shared_ptr<Model> MachineModel;
		std::shared_ptr<Model> StorageModel;
		std::map<std::string, std::shared_ptr<Model>> modelmap;

		float bias = 0.00005f;//ֱ�����Ӱ���ڵ�ƫ��
		float radius = 0.2f;//hbao����뾶
		float bias1 = 0.01f;//hbao���ڵ�ƫ��
		float bias2 = 0.03f;//���Դ��Ӱ���ڵ�ƫ��
		float bias3 = 0.5f;//hbao�ڵ���������
		//float bias4 = 100.0f;//hbao��ʾ��������
		//std::unique_ptr<Texture> hbaotexture;
		float BiasMax = 2000.0f;//hbao���ڵ�ƫ�����ֵ
		float BiasMin = 1.0f;//hbao���ڵ�ƫ����Сֵ

		bool ShadowGaussian = true;
		//std::unique_ptr<Animation> anim;
		float width0 = 15.0f;//��ֵģ������С��
		float height0 = 10.0f;//��ֵģ������С��
		float width1 = 360000.0f;//��ֵģ��������
		float height1 = 240000.0f;//��ֵģ��������

		float ShadowRoundSize = 25.0f;//��ӰԲ��ȣ�ԽСԽԲ��
		float ShadowSoftSize = 100.0f;//��Ӱģ����
		float shadowColorDepth = 0.5f;//��Ӱ��ɫ���
		float hbaoShadowColorDepth = 1.0f;//hbao��ɫ���

		bool UIClicked = false;

		float FactoryLightPos[70] = { 0.0f };
		float LightPosX[7] = { -112.48f,-74.32f,-35.87f,-0.76f,36.85f,74.73f,113.26f };
		float LightPosZ[5] = { -55.96f,-27.74f,-0.11f,28.1f,55.78f };
		float LightPosY = 28.0f;
		
		float basicP = 20.0f;//basic�ĸ߹�ָ��
		float planeP = 50.0f;//plane�ĸ߹�ָ��
		float F0 = 0.4f;//plane�ĸ߹�ָ��
		float fp = 2.0f;//plane�ĸ߹�ָ��
		
		glm::mat4 lightModelMatrices[35];
		std::map<float, glm::mat4> LightSorted;

		bool ModularCopy = false;
		
		glm::vec3 FirstRotate = glm::vec3(1.0f);
	};

	Application* CreateApplication();
}


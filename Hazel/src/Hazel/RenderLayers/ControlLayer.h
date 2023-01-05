#pragma once
#include "Hazel/Layer.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Renderer/Model.h"
#include "Objects/Arrow.h"

namespace Hazel {

	class HAZEL_API ControlLayer : public Layer
	{
	public:
		ControlLayer();
		~ControlLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Hazel::Event& event) override;
		virtual void OnImGuiRender() override;

	private:
		std::shared_ptr<Shader> ArrowShader;
		std::shared_ptr<Model> ArrowModel;
		std::unique_ptr<Arrow> arrow;

		bool OnMouseButtonEvent(MouseButtonPressedEvent& e);
	};

}

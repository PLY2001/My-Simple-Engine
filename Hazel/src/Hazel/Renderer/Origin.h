#pragma once
#include "Buffer.h"

namespace Hazel {

	class Origin
	{
	public:
		Origin(std::shared_ptr<Shader>& shader);
		~Origin() = default;
		void Draw();
	private:
		std::shared_ptr<Shader> m_shader;
		std::unique_ptr<VertexArray> m_va;
		std::unique_ptr<VertexBuffer> m_vb;
		unsigned int vaID;//VertexArray
		unsigned int vbID;

	};
}

#pragma once
#include "Buffer.h"

namespace Hazel {

	class AABB
	{
	public:
		AABB(std::shared_ptr<Shader>& shader);
		~AABB() = default;
		void Draw(glm::vec3 MinPos, glm::vec3 MaxPos);
	private:
		std::shared_ptr<Shader> m_shader;
	};
}
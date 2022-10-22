#pragma once

namespace Hazel {
	class Light
	{
	private:

	public:
		glm::vec3 Pos;
		Light(glm::vec3 pos);
		std::vector<glm::mat4> GetShadowTransformsPoint(glm::mat4 shadowProj);
	};

}

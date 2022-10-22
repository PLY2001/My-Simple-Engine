#include "hzpch.h"
#include "Light.h"

namespace Hazel {
	Light::Light(glm::vec3 pos) :Pos(pos)
	{

	}

	std::vector<glm::mat4> Light::GetShadowTransformsPoint(glm::mat4 shadowProj)
	{
		std::vector<glm::mat4> shadowTransforms;//�ӽǱ任Ҫ��6����Ӧ����ı任����
		shadowTransforms.push_back(shadowProj * glm::lookAt(Pos, Pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(Pos, Pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(Pos, Pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(Pos, Pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(Pos, Pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(Pos, Pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
		return shadowTransforms;
	}
}
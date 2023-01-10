#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
class ModelMatrix
{
public:
	glm::mat4 matrix = glm::mat4(1.0f);
	ModelMatrix(glm::vec3 Pos)
	{
		matrix = glm::translate(matrix, Pos);
	}

};

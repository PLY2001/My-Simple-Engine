#pragma once

#include "Buffer.h"
#include "Model.h"
#include "Objects/Objects.h"

namespace Hazel {
	class InstanceBufferObjects
	{
	private:
		int ObjectsAmount = 0;
		std::vector<std::vector<std::unique_ptr<InstanceBuffer>>> m_insbos;
	public:
		InstanceBufferObjects() = default;
		void AddObject(std::shared_ptr<Model>& model);
		void SetDatamat4(std::shared_ptr<Objects>& objects);

	};
}
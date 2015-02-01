#pragma once
#include "interfaces\IObject.h"

namespace sqw{
	class Object : public IObject{
	public:
		Object(const glm::vec2& position, const glm::vec2& speed);
		virtual ~Object();

		virtual glm::vec2& getPosition() override;
		virtual glm::vec2& getSpeed() override;
	protected:
		glm::vec2 position;
		glm::vec2 speed;
	};
}


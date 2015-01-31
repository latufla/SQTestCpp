#pragma once
#include <memory>

namespace sqw {
	class IObject {
	public:		
		virtual void addObject(uint32_t id, std::shared_ptr<IObject> object) = 0;
		virtual void removeObject(uint32_t id) = 0;

		virtual std::shared_ptr<IObject> getObjectBy(uint32_t id) = 0;

		virtual bool doStep(long long step) = 0;
	};
}

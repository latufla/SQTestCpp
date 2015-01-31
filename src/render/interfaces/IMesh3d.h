#pragma once
#include "../../SharedHeaders.h"
#include <vector>
#include <memory>
#include <array>

namespace sqr {
	class IVertex3d { // sorry bout that - max extensibility, min c++ 
	public:
		virtual std::array<float, 3>& getPosition() = 0;
	};

	class IMesh3d {
	public:
		virtual std::vector<std::shared_ptr<IVertex3d>>& getVertices() = 0;
		virtual std::vector<uint16_t>& getIndices() = 0;
	};
}
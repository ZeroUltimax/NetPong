#pragma once

#include <memory>

namespace ZeroUltimax{

	class AABB{

	public:
		float left, right, top, bot;

		AABB();

		AABB(float left, float right, float top, float bot);

		// Split along a vertical axis
		// leftSide == true returns the left side of the split
		std::unique_ptr<AABB> splitVert(float x, bool leftSide) const;

		// Split along a horizontal axis
		// topSide == true returns the top side of the split
		std::unique_ptr<AABB> splitHoriz(float y, bool topSide) const;

		float area();

		// Returns if this box completely encompasses the area of the other
		bool includes(const AABB& other)const;

		// Returns of the two boxes intersect
		bool intersects(const AABB &other) const;

		// Returns if the point is in this box
		bool intersects(float x, float y) const;

		// Returns if the point is strictly in the box
		bool intersectsStrict(float x, float y) const;

		// Returns the biggest box that is in both, or a 0 area box if no intersection occurs
		AABB intersection(const AABB &other) const;

		// Returns the smallest box that bounds this and the other
		AABB bounding(const AABB &other) const;
	};


}
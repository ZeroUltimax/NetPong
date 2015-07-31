#pragma once

#include "Aabb.h"
#include <list>
#include <array>

namespace ZeroUltimax{

	struct Point{

		float x, y;

		bool operator==(const Point& other)const{
			return x == other.x && y == other.y;
		}
	};

	class ReducingBb
	{

		enum Corner{
			NW = 0,
			NE = 1,
			SW = 2,
			SE = 3
		};

		class CornerGroup{

			Corner direction;
			Point corner;

			std::list<Point> points;


			float xOrd(const Point& p)const;
			float yOrd(const Point& p)const;

			std::list<Point>::const_iterator findInsertPoint(const Point& p)const;
			std::list<Point>::const_iterator findErasePoint(const Point& p)const;

			std::list<Point>::const_iterator findHitPoint(const Point& p)const;
		public:

			enum Direction{
				top, bot, left, right
			};

			CornerGroup(Corner direction, const Point& corner);

			std::list<AABB> getBoxes()const;

			// Insert the box in the corner
			void insert(const Point& p);

			AABB getBounds()const;

			bool contains(const Point& p)const;

			float boundedArea(const AABB& bound)const;

			float area()const;

			static void adjustBoundaries(CornerGroup& a, CornerGroup& b, Direction d);

		};

		// The negative area in the 4 corners
		std::array<CornerGroup, 4> corners;

		std::array<CornerGroup, 4> makeCorners(const AABB& bounds)const;

		std::array<AABB, 4> getCornerBoxes(const Point& p)const;

		std::array<float, 4> getCornerAreas(const Point& p)const;

		void insertAtMinCorner(const Point& p);

	public:

		ReducingBb(const AABB& bounds);

		bool contains(const Point& p) const;

		AABB getBounds()const;

		void hit(const Point& p);

		std::list<AABB> getCornerBoxes()const;

		float area();

		template<class T, size_t U>
		friend class std::array;
	};

}
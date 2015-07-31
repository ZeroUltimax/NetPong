#include "ReducingBb.h"
#include <algorithm>
using namespace std;

namespace ZeroUltimax{

	ReducingBb::CornerGroup::CornerGroup(Corner direction, const Point& corner) 
		:direction(direction),
		corner(corner)
	{}

	float ReducingBb::CornerGroup::xOrd(const Point& p)const{
		switch (direction){
		case NW:
		case SW:
			return p.x;
		case NE:
		case SE:
			return -p.x;
		}
	}

	float ReducingBb::CornerGroup::yOrd(const Point& p)const{
		switch (direction){
		case NW:
		case NE:
			return -p.y;
		case SW:
		case SE:
			return p.y;
		}
	}

	list<Point>::const_iterator ReducingBb::CornerGroup::findInsertPoint(const Point& p)const{
		list<Point>::const_iterator insertion;

		for (insertion = points.begin(); insertion != points.cend() && xOrd(*insertion) <= xOrd(p); ++insertion)
			;

		return insertion;
	}

	list<Point>::const_iterator ReducingBb::CornerGroup::findHitPoint(const Point& p)const{
		list<Point>::const_iterator insertion;

		for (insertion = points.begin(); insertion != points.cend() && xOrd(*insertion) < xOrd(p); ++insertion)
			;

		return insertion;
	}

	// Returns an iterator to the first element for which opositeOrdering is smaller than the box's
	list<Point>::const_iterator ReducingBb::CornerGroup::findErasePoint(const Point& p) const{
		list<Point>::const_iterator erase;

		for (erase = points.begin(); erase != points.cend() && yOrd(*erase) < yOrd(p); ++erase)
			;

		return erase;
	}

	void ReducingBb::CornerGroup::insert(const Point& p){

		list<Point>::const_iterator insert = findInsertPoint(p);

		// Point is already represented by in this group.
		if (insert != points.cend() && yOrd(*insert) <= yOrd(p))
			return;

		list<Point>::const_iterator erase = findErasePoint(p);

		// Erase points now encompassed by the new point
		points.erase(erase, insert);

		// Insert it in it's place
		points.insert(insert, p);

	}

	bool ReducingBb::CornerGroup::contains(const Point& p)const{
		list<Point>::const_iterator insert = findHitPoint(p);

		return insert != points.cend() && yOrd(*insert) <= yOrd(p);
	}

	AABB ReducingBb::CornerGroup::getBounds()const{
		if (points.empty()){
			return AABB(corner.x, corner.x, corner.y, corner.y);
		}

		return AABB(corner.x, points.back().x, corner.y, points.front().y);
	}

	list<AABB> ReducingBb::CornerGroup::getBoxes()const{
		list<AABB> boxes;

		float prevX = corner.x;

		for (Point p : points){
			boxes.emplace_back(prevX, p.x, corner.y, p.y);
			prevX = p.x;
		}

		return boxes;
	}

	float ReducingBb::CornerGroup::boundedArea(const AABB& bound)const{

		float total = 0;

		for (AABB box : getBoxes()){
			total += box.intersection(bound).area();
		}

		return total;

	}

	float ReducingBb::CornerGroup::area()const{

		float total = 0;

		for (AABB box : getBoxes()){
			total += box.area();
		}

		return total;

	}

	void ReducingBb::CornerGroup::adjustBoundaries(CornerGroup& a, CornerGroup& b, Direction d){
	
		Point& (list<Point>::* pointSelect)() = nullptr;
		float& (*valueGet)(Point&) = nullptr;
		const float& (*valueSelect)(const float&, const float&) = nullptr;
		void (list<Point>::* popper)() = nullptr;

		switch (d){
		case top :
		case left:
			valueSelect = min;
			break;
		case bot:
		case right:
			valueSelect = max;
		}

		switch (d){
		case top:
		case bot:
			pointSelect = &list<Point>::back;
			popper = &list<Point>::pop_back;
			valueGet = [](Point& p)->float&{return p.y; };
			break;
		case left:
		case right:
			pointSelect = &list<Point>::front;
			popper = &list<Point>::pop_front;
			valueGet = [](Point& p)->float&{return p.x; };
			break;
		}

		// Get the clashing points
		Point &ap = (a.points.*pointSelect)();
		Point &bp = (b.points.*pointSelect)();

		// Find which has the value closest to the border
		float rise = valueSelect(valueGet(ap), valueGet(bp));

		// Rise the value of the border
		valueGet(a.corner) = rise;
		valueGet(b.corner) = rise;

		// If the point's value is on the new border, remove it.
		if (valueGet(ap) == rise){
			(a.points.*popper)();
		}

		if (valueGet(bp) == rise){
			(b.points.*popper)();
		}
	}


	array<ReducingBb::CornerGroup, 4> ReducingBb::makeCorners(const AABB& bounds)const{
		Point cornerPoints[4] {
			Point{ bounds.left, bounds.top }, // NW
			Point{ bounds.right, bounds.top }, // NE
			Point{ bounds.left, bounds.bot }, // SW
			Point{ bounds.right, bounds.bot } // SE
		};

		return array<ReducingBb::CornerGroup, 4>(
		{
			CornerGroup(NW, cornerPoints[NW]),
			CornerGroup(NE, cornerPoints[NE]),
			CornerGroup(SW, cornerPoints[SW]),
			CornerGroup(SE, cornerPoints[SE])
		}
		);
	}

	ReducingBb::ReducingBb(const AABB& bounds)
		:corners(makeCorners(bounds))
	{}

	bool ReducingBb::contains(const Point& p)const{
		return getBounds().intersectsStrict(p.x, p.y) && // Stric intesection removes the case where the point is RIGHT on the border
			!any_of(corners.begin(), corners.end(), [&](const CornerGroup& g){return g.contains(p); });
	}

	AABB ReducingBb::getBounds()const{
		AABB bounds(corners[0].getBounds());

		for (int i = 1; i < 4; i++){
			bounds = bounds.bounding(corners[i].getBounds());
		}

		return bounds;
	}

	std::array<AABB, 4> ReducingBb::getCornerBoxes(const Point& p)const
	{
		array<AABB, 4> cornerBoxes;

		AABB base = getBounds();

		unique_ptr<AABB> north = base.splitHoriz(p.y, true);
		//if (north){
		cornerBoxes[0] = *north->splitVert(p.x, true);
		cornerBoxes[1] = *north->splitVert(p.x, false);
		//}

		unique_ptr<AABB> south = base.splitHoriz(p.y, false);
		//if (south){
		cornerBoxes[2] = *south->splitVert(p.x, true);
		cornerBoxes[3] = *south->splitVert(p.x, false);
		//}

		return cornerBoxes;
	}

	std::array<float, 4> ReducingBb::getCornerAreas(const Point& p)const{
	
		array<AABB, 4> cornerBoxes(getCornerBoxes(p));

		array<float, 4> areas;

		for (int i = 0; i < 4; i++){

			// Find the remaining area in the corner i
			areas[i] = cornerBoxes[i].area();

			for (int j = 0; j < 4; j++){
				areas[i] -= corners[j].boundedArea(cornerBoxes[i]);
			}
		}

		return areas;

	}

	void ReducingBb::insertAtMinCorner(const Point& p){
		
		array<float, 4> areas(getCornerAreas(p));

		int minIndex = -1;

		float minValue = numeric_limits<float>::infinity();

		for (int i = 0; i < 4; i++){
			if (areas[i] < minValue){
				minValue = areas[i];
				minIndex = i;
			}
		}

		corners[minIndex].insert(p);
	}

	// Assumes point is in the bounds
	void ReducingBb::hit(const Point& p){

		insertAtMinCorner(p);

		{
			if (corners[NW].getBounds().intersects(corners[NE].getBounds())){
				CornerGroup::adjustBoundaries(corners[NW], corners[NE], CornerGroup::top);
			}

			if (corners[NE].getBounds().intersects(corners[SE].getBounds())){
				CornerGroup::adjustBoundaries(corners[NE], corners[SE], CornerGroup::right);
			}

			if (corners[SW].getBounds().intersects(corners[SE].getBounds())){
				CornerGroup::adjustBoundaries(corners[SW], corners[SE], CornerGroup::bot);
			}

			if (corners[NW].getBounds().intersects(corners[SW].getBounds())){
				CornerGroup::adjustBoundaries(corners[NW], corners[SW], CornerGroup::left);
			}
		
		}

	}

	std::list<AABB> ReducingBb::getCornerBoxes()const{
	
		list<AABB> result;

		for (CornerGroup g : corners){
			result.splice(result.end(), g.getBoxes());
		}

		return result;
	}

	float ReducingBb::area(){

		float area = getBounds().area();

		for (CornerGroup g:corners){
			area -= g.area();
		}

		return area;

	}
}
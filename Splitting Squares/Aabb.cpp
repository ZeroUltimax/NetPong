#include "Aabb.h"

#include <list>
#include <algorithm>

using namespace std;

namespace ZeroUltimax{

AABB::AABB() :
left(),
right(),
top(),
bot()
{}

AABB::AABB(float left, float right, float top, float bot) :
left(left),
right(right),
top(top),
bot(bot)
{
	if (left > right){
		swap(this->left, this->right);
	}

	if (top > bot){
		swap(this->top, this->bot);
	}
}

// Split along a vertical axis
// leftSide == true returns the left side of the split
unique_ptr<AABB> AABB::splitVert(float x, bool leftSide) const{
	if (leftSide){
		if (left > x){
			return nullptr;
		}
		else if (right < x){
			return make_unique<AABB>(*this);
		}
		else{
			return make_unique<AABB>(left, x, top, bot);
		}
	}
	else{
		if (left > x){
			return make_unique<AABB>(*this);
		}
		else if (right < x){
			return nullptr;
		}
		else{
			return make_unique<AABB>(x, right, top, bot);
		}
	}
}

unique_ptr<AABB> AABB::splitHoriz(float y, bool topSide) const{
	if (topSide){
		if (top > y){
			return nullptr;
		}
		else if (bot < y){
			return make_unique<AABB>(*this);
		}
		else{
			return make_unique<AABB>(left, right, top, y);
		}
	}
	else{
		if (top > y){
			return make_unique<AABB>(*this);
		}
		else if (bot < y){
			return nullptr;
		}
		else{
			return make_unique<AABB>(left, right, y, bot);
		}
	}
}

float AABB::area(){
	return (right - left)*(bot - top);
}

bool AABB::includes(const AABB& other)const{
	return
		other.left > left &&
		other.right < right &&
		other.top > top &&
		other.bot < bot;
}

bool AABB::intersects(const AABB &other) const{
	return right >= other.left &&
		left <= other.right &&
		bot >= other.top &&
		top <= other.bot;
}

bool AABB::intersects(float x, float y) const{
	return right >= x &&
		left <= x &&
		bot >= y &&
		top <= y;
}

bool AABB::intersectsStrict(float x, float y) const{
	return right > x &&
		left < x &&
		bot > y &&
		top < y;
}


AABB AABB::intersection(const AABB &other) const{
	if (intersects(other)){
		return AABB(
			max(left, other.left),
			min(right, other.right),
			max(top, other.top),
			min(bot, other.bot)
			);
	}
	else{
		return AABB();
	}
}

// returns the box that bounds this and the other
AABB AABB::bounding(const AABB &other) const{
	return AABB(
		min(left, other.left),
		max(right, other.right),
		min(top, other.top),
		max(bot, other.bot)
		);
}




}
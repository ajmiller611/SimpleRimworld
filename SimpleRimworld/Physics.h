#pragma once

#include "Entity.h"

struct Intersect
{
	bool intersected = false;
	Vec2 point;
};

class Physics
{
public:

	Physics() {}

	Vec2 static GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	Vec2 static GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	bool static IsInside(const Vec2& pos, std::shared_ptr<Entity> e);
	Intersect LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);
	bool EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e);
};
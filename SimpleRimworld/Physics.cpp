#include "Physics.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	if (a->has<CBoundingBox>() && b->has<CBoundingBox>())
	{
		Vec2 delta(abs(b->get<CBoundingBox>().pos.x - a->get<CTransform>().pos.x),
			abs(b->get<CBoundingBox>().pos.y - a->get<CTransform>().pos.y));
		float XOverlap = a->get<CBoundingBox>().halfSize.x + b->get<CBoundingBox>().halfSize.x - delta.x;
		float YOverlap = a->get<CBoundingBox>().halfSize.y + b->get<CBoundingBox>().halfSize.y - delta.y;
		return Vec2(XOverlap, YOverlap);
	}
	else { return Vec2(0, 0); }
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	if (a->has<CBoundingBox>() && b->has<CBoundingBox>())
	{
		Vec2 delta(abs(b->get<CBoundingBox>().pos.x - a->get<CTransform>().prevPos.x),
			abs(b->get<CBoundingBox>().pos.y - a->get<CTransform>().prevPos.y));
		float XOverlap = a->get<CBoundingBox>().halfSize.x + b->get<CBoundingBox>().halfSize.x - delta.x;
		float YOverlap = a->get<CBoundingBox>().halfSize.y + b->get<CBoundingBox>().halfSize.y - delta.y;
		return Vec2(XOverlap, YOverlap);
	}
	else { return Vec2(0, 0); }
}

bool Physics::IsInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
	sf::FloatRect globalBounds = e->get<CAnimation>().animation.getSprite().getGlobalBounds();
	if (pos.x > globalBounds.left && pos.x < globalBounds.left + globalBounds.width &&
		pos.y > globalBounds.top && pos.y < globalBounds.top + globalBounds.height)
	{
		return true;
	}

	return false;
}

Intersect Physics::LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
	Vec2 r = b - a;
	Vec2 s = d - c;
	float rxs = r.cross(s);
	Vec2 cma = c - a;
	float t = cma.cross(s) / rxs;
	float u = cma.cross(r) / rxs;

	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
	{
		return { true, Vec2(a.x + t * r.x, a.y + t * r.y) };
	}
	else
	{
		return { false, Vec2(0,0) };
	}	
}

bool Physics::EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e)
{
	sf::FloatRect globalBounds = e->get<CAnimation>().animation.getSprite().getGlobalBounds();
	Vec2 topLeft = Vec2(globalBounds.left, globalBounds.top);
	Vec2 topRight = Vec2(globalBounds.left + globalBounds.width, globalBounds.top);
	Vec2 bottomLeft = Vec2(globalBounds.left, globalBounds.top + globalBounds.height);
	Vec2 bottomRight = Vec2(globalBounds.left + globalBounds.width, globalBounds.top + globalBounds.height);

		 if (LineIntersect(a, b, topLeft, topRight).intersected)		{ return true; }
	else if (LineIntersect(a, b, topRight, bottomRight).intersected)	{ return true; }
	else if (LineIntersect(a, b, bottomRight, bottomLeft).intersected)  { return true; }
	else if (LineIntersect(a, b, bottomLeft, topLeft).intersected)		{ return true; }

	return false;
}
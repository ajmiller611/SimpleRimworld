#pragma once

#include "Animation.h"

class Component
{
public:
	bool has = false;
};

class CTransform : public Component
{
public:
	Vec2 pos = { 0.0, 0.0 };
	Vec2 prevPos = { 0.0, 0.0 };
	Vec2 scale = { 1.0, 1.0 };
	Vec2 velocity = { 0.0, 0.0 };
	Vec2 facing = { 0.0, 0.0 };
	float angle = 0;

	CTransform() {}
	CTransform(const Vec2& p)
		: pos(p) {}
	CTransform(const Vec2& p, const Vec2& sp, const Vec2& sc, float a)
		: pos(p), prevPos(p), velocity(sp), scale(sc), angle(a) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool attack = false;
	bool canAttack = true;

	CInput() {}
};

class CLifespan : public Component
{
public:
	int lifespan = 0;
	int frameCreated = 0;
	CLifespan() {}
	CLifespan(int duration, int frame)
		: lifespan(duration), frameCreated(frame) {}
};

class CDamage : public Component
{
public:
	int damage = 1;
	CDamage() {}
	CDamage(int d)
		: damage(d) {}
};

class CInvincibility : public Component
{
public:
	int iframes = 0;
	CInvincibility() {}
	CInvincibility(int f)
		: iframes(f) {}
};

class CHealth : public Component
{
public:
	int max = 1;
	int current = 1;
	CHealth() {}
	CHealth(int m, int c)
		: max(m), current(c) {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation& ani, bool r)
		: animation(ani), repeat(r) {}
};

class CState : public Component
{
public:
	std::string state = "stand";
	CState() {}
	CState(const std::string& s) : state(s) {}
};

class CBoundingBox : public Component
{
public:
	Vec2 pos;
	Vec2 offset;
	Vec2 size;
	Vec2 halfSize;
	bool blockMove = false;
	bool blockVision = false;
	CBoundingBox() {}
	CBoundingBox(const Vec2& p, const Vec2& o, const Vec2& s)
		: pos(p), offset(o), size(s), halfSize(s.x / 2, s.y / 2) {}
	CBoundingBox(const Vec2& p, const Vec2& o, const Vec2& s, bool m, bool v)
		: pos(p), offset(o), size(s), blockMove(m), blockVision(v), halfSize(s.x / 2, s.y / 2) {}
};

class CFollowPlayer : public Component
{
public:
	Vec2 home = { 0, 0 };
	float speed = 0;
	CFollowPlayer() {}
	CFollowPlayer(Vec2 p, float s)
		: home(p), speed(s) {}
};

class CPatrol : public Component
{
public:
	std::vector<Vec2> positions;
	size_t currentPosition = 0;
	float speed = 0;
	CPatrol() {}
	CPatrol(std::vector<Vec2>& pos, float s)
		: positions(pos), speed(s) {}
};

class CDraggable : public Component
{
public:
	bool dragging = false;
	CDraggable() {}
};

class CHand : public Component
{
public:
	size_t entityID = 0;
	int weaponID = -1;
	Vec2 offset = { 0, 0 };
	CHand() {}
	CHand(size_t e, Vec2 o)
		: entityID(e), offset(o) {}
};

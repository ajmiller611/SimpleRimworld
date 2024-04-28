#include "Vec2.h"
#include <math.h>

Vec2::Vec2() {}
Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin) {}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator / (const float val) const
{
	return Vec2(x / val, y / val);
}

Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x * val, y * val);
}

bool Vec2::operator == (const Vec2& rhs) const
{
	return (x == rhs.x) && (y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
	return (x != rhs.x) && (y != rhs.y);
}

void Vec2::operator += (const Vec2& rhs)
{
	this->x += rhs.x;
	this->y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	this->x -= rhs.x;
	this->y -= rhs.y;
}

void Vec2::operator *= (const float val)
{
	this->x *= val;
	this->y *= val;
}

void Vec2::operator /= (const float val)
{
	this->x /= val;
	this->y /= val;
}

Vec2 Vec2::difference(const Vec2& destination) const
{
	return Vec2(destination.x - this->x, destination.y - this->y);
}

float Vec2::dist(const Vec2& rhs) const
{
	Vec2 diff = this->difference(rhs);
	return sqrtf(diff.x * diff.x + diff.y * diff.y);
}

float Vec2::length() const
{
	return sqrtf((this->x * this->x) + (this->y * this->y));
}

Vec2& Vec2::normalize()
{
	float len = this->length();
	this->x = this->x / len;
	this->y = this->y / len;
	return *this;
}

float Vec2::cross(const Vec2& b) const
{
	return this->x * b.y - this->y * b.x;
}
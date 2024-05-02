#pragma once

#include "Vec2.h"
#include <string>

class Action
{
	std::string m_name = "NONE";
	std::string m_type = "NONE";
	Vec2		m_pos = Vec2(0, 0);

public:

	Action();
	Action(const std::string& name, const std::string& type);
	Action(const std::string& name, Vec2 pos);
	Action(const std::string& name, const std::string& type, Vec2 pos);

	const std::string& name() const;
	const std::string& type() const;
	const Vec2& pos() const;
	std::string toString() const;
};
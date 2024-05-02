#include "Action.h"

#include <sstream>

Action::Action()
{

}

Action::Action(const std::string& name, const std::string& type)
	: m_name(name)
	, m_type(type)
{

}

Action::Action(const std::string& name, Vec2 pos)
	: m_name(name)
	, m_pos(pos)
{

}

Action::Action(const std::string& name, const std::string& type, Vec2 pos)
	: m_name(name)
	, m_type(type)
	, m_pos(pos)
{

}

const std::string& Action::name() const
{
	return m_name;
}

const std::string& Action::type() const
{
	return m_type;
}

const Vec2& Action::pos() const
{
	return m_pos;
}

std::string Action::toString() const
{
	std::stringstream ss;
	ss << name() << " " << type() << " " << (int)m_pos.x << " " << (int)m_pos.y;
	return ss.str();
}

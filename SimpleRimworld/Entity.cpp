#include "Entity.h"

Entity::Entity(const size_t id, const std::string& tag)
	: m_id(id)
	, m_tag(tag)
{
	
}

void Entity::destroy()
{
	m_active = false;
}

const size_t Entity::id() const
{
	return m_id;
}

bool Entity::isActive() const
{
	return m_active;
}

const std::string& Entity::tag() const
{
	return m_tag;
}

std::vector<std::string> Entity::getComponentList()
{
	m_componentList.clear();
	this->getComponents();
	return m_componentList;
}
#include "EntityManager.h"

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
	for (auto& e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e.get()->m_tag].push_back(e);
	}
	m_entitiesToAdd.clear();

	removeDeadEntities(m_entities);

	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	// erase takes a beginning iterator and an ending iterator. The remove_if function returns
	// an iterator of the element right before the element to be removed. Lambda function is used
	// to look at the Entity isActive boolean variable.
	vec.erase(std::remove_if(vec.begin(), vec.end(),
		[](std::shared_ptr<Entity> object) {return !(object->isActive()); }), vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.push_back(entity);
	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}

const std::map<std::string, EntityVec>& EntityManager::getEntityMap()
{
	return m_entityMap;
}

std::shared_ptr<Entity> EntityManager::getEntity(size_t id)
{
	return m_entities.at(id);
}
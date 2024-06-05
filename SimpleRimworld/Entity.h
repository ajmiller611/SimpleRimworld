#pragma once

#include "Components.h"

#include <tuple>
#include <string>

class EntityManager;

typedef std::tuple <
	CTransform,
	CInput,
	CLifespan,
	CDamage,
	CInvincibility,
	CHealth,
	CAnimation,
	CState,
	CBoundingBox,
	CFollowPlayer,
	CPatrol,
	CDraggable,
	CHand
> ComponentTuple;

class Entity
{
	friend class EntityManager;

	bool           m_active = true;
	std::string    m_tag = "default";
	size_t		   m_id = 0;
	ComponentTuple m_components;
	std::vector<std::string> m_componentList;

	Entity(const size_t id, const std::string& tag);

public:

	void               destroy();
	const size_t       id() const;
	bool               isActive() const;
	const std::string& tag() const;
	std::vector<std::string> getComponentList();

	template <typename T>
	bool has() const
	{
		return get<T>().has;
	}

	template <typename T, typename... TArgs>
	T& add(TArgs&&... mArgs)
	{
		auto& component = get<T>();
		component = T(std::forward<TArgs>(mArgs)...);
		component.has = true;
		return component;
	}

	template <typename T>
	T& get()
	{
		return std::get<T>(m_components);
	}

	template <typename T>
	const T& get() const
	{
		return std::get<T>(m_components);
	}

	template <typename T>
	void remove()
	{
		get<T>() = T();
	}

	template <size_t I = 0>
	constexpr void getComponents()
	{
		if constexpr (I < std::tuple_size_v<ComponentTuple>)
		{
			if (std::get<I>(m_components).has)
			{
				std::string typeName = typeid(std::tuple_element_t<I, ComponentTuple>).name();

				// The type information of the component includes it is a class in its name.
				// Since each class name of a component is prefix with 'C', that index can
				// be used to find the component named in the string.
				size_t pos = typeName.find("C");
				std::string cleanedName = typeName.substr(pos);
				m_componentList.push_back(cleanedName);
			}
			getComponents<I + 1>();
		}
	}
};

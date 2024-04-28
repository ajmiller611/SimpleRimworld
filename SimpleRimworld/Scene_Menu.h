#pragma once

#include "Scene.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

class Scene_Menu : public Scene
{
protected:

	std::string              m_title;
	std::vector<std::string> m_menuStrings;
	std::vector<std::string> m_levelPaths;
	std::vector<std::string> m_musicPaths;
	sf::Text                 m_menuText;
	size_t                   m_selectedMenuIndex = 0;

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);

public:

	Scene_Menu() {}
	Scene_Menu(GameEngine* gameEngine = nullptr);
	void sRender();
};
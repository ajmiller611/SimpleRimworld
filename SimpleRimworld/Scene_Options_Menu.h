#pragma once

#include "Scene.h"

class Scene_Options_Menu : public Scene
{
protected:
	std::string m_levelPath;

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);

public:

	Scene_Options_Menu() {}
	Scene_Options_Menu(GameEngine* gameEngine, const std::string& levelPath);

	void sRender();
};
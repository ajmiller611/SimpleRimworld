#pragma once

#include "Scene.h"

class Scene_Level_Editor : public Scene
{
	std::string		   m_levelPath;
	bool			   m_drawTextures = true;
	bool			   m_drawCollision = false;
	bool			   m_drawGrid = false;
	const Vec2		   m_gridSize = { 64, 64 };
	sf::Text		   m_gridText;

protected:

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);
	void sCamera();
	void sGui();

public:

	Scene_Level_Editor() {}
	Scene_Level_Editor(GameEngine* gameEngine, const std::string& levelPath);

	void sRender();
};
#pragma once

#include "Scene.h"


class Scene_Home_Map : public Scene
{
	struct PlayerConfig
	{
		float X = 0, Y = 0, CX = 0, CY = 0, SPEED = 0, HEALTH = 0;
		std::string WEAPON;
	};

	struct Collision
	{
		bool collided = false;
		Vec2 overlap = { 0, 0 };
		char direction = '\0';
	};

protected:

	std::shared_ptr<Entity>  m_block;
	std::string              m_levelPath;
	std::string				 m_lastAction;
	PlayerConfig             m_playerConfig;
	bool                     m_drawTextures = true;
	bool                     m_drawCollision = false;
	bool                     m_drawGrid = false;
	const Vec2               m_gridSize = { 64, 64 };
	sf::Text                 m_gridText;

	void init(const std::string& levelPath);
	void loadLevel(const std::string& filename);
	std::shared_ptr<Entity> player();
	void spawnPlayer();
	void update();
	void onEnd();
	Collision collided(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

	void sDoAction(const Action& action);
	void sMovement();
	void sAI();
	void sStatus();
	void sAnimation();
	void sCollision();
	void sCamera();
	void sGui();

public:

	Scene_Home_Map() {}
	Scene_Home_Map(GameEngine* gameEngine, const std::string& levelPath);

	void sRender();
};
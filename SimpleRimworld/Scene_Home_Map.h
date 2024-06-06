#pragma once

#include "Scene.h"
#include "variant"

typedef std::vector<std::variant<CAnimation, CBoundingBox, CDamage>> WeaponsVec;
typedef std::map<std::string, WeaponsVec>	 WeaponsMap;

class Scene_Home_Map : public Scene
{
	struct PlayerConfig
	{
		float GX = 0, GY = 0, SX = 0, SY = 0, SPEED = 0, HEALTH = 0;
		std::string WEAPON;
	};

	struct Collision
	{
		bool collided = false;
		Vec2 overlap = { 0, 0 };
		char direction = '\0';
	};

protected:

	WeaponsMap				 m_weaponsMap;
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
	void spawnWeapon(std::shared_ptr<Entity> e, const std::string& name);
	void update();
	void onEnd();
	Collision collided(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	void moveHand(std::shared_ptr<Entity> e);
	void moveWeapon(std::shared_ptr<Entity> e);

	void sDoAction(const Action& action);
	void sMovement();
	void sAI();
	void sStatus();
	void sAnimation();
	void sCollision();
	void sCamera();
	void displayEntityData(std::shared_ptr<Entity> e);
	void sGui();

public:

	Scene_Home_Map() {}
	Scene_Home_Map(GameEngine* gameEngine, const std::string& levelPath);

	void sRender();
};
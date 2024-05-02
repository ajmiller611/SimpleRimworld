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

	Vec2				m_mousePos;
	sf::CircleShape		m_mouseDot;

	std::vector<std::string> m_entityTypes;
	const char* combo_preview_value = nullptr;
	size_t selected_index = 0;
	bool m_blockMoveCheckbox = false;
	bool m_blockVisionCheckbox = false;
	int m_boundingBoxWidth = 0;
	int m_boundingBoxHeight= 0;


protected:

	void init();
	void update();
	void onEnd();
	Vec2 windowToWorld(const Vec2& window) const;
	void sDoAction(const Action& action);
	void sDragAndDrop();
	void sCamera();
	void sGui();

public:

	Scene_Level_Editor() {}
	Scene_Level_Editor(GameEngine* gameEngine, const std::string& levelPath);

	void sRender();
};
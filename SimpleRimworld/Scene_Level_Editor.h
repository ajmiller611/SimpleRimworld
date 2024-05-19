#pragma once

#include "Scene.h"

class Scene_Level_Editor : public Scene
{
	std::string					m_levelPath;
	bool						m_drawTextures = true;
	bool						m_drawCollision = false;
	bool						m_drawGrid = false;
	const Vec2					m_gridSize = { 64, 64 };
	sf::Text					m_gridText;
	Vec2						m_mousePos;
	std::vector<std::string>	m_entityTypes;
	Animation					m_animationSelected = Animation();
	std::shared_ptr<Entity>		m_entityBeingDragged = nullptr;

	// ImGui member variables
	const char* m_animTypeComboPreviewValue = nullptr;
	size_t m_animTypeComboSelectedIndex = 0;
	bool m_blockMoveCheckbox = false;
	bool m_blockVisionCheckbox = false;
	Vec2 m_selectionAreaBoundingBoxPos;
	int m_boundingBoxLeft = 0;
	int m_boundingBoxRight = m_gridSize.x;
	int m_boundingBoxTop = 0;
	int m_boundingBoxBottom = m_gridSize.y;


protected:

	void init();
	void loadLevel(const std::string& filename);
	Vec2 windowToWorld(const Vec2& window) const;
	Vec2 rotate(std::shared_ptr<Entity> e, float angle);
	void update();
	bool saveToFile(const char* filename);
	void onEnd();
	void sDoAction(const Action& action);
	void sDragAndDrop();
	void sCamera();
	void sGui();

public:

	Scene_Level_Editor() {}
	Scene_Level_Editor(GameEngine* gameEngine, const std::string& levelPath);

	void sRender();
};
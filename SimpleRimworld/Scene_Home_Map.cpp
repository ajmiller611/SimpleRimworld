#include "Scene_Home_Map.h"
#include "Scene_Menu.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <math.h>

Scene_Home_Map::Scene_Home_Map(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Home_Map::init(const std::string& levelPath)
{
	
}

void Scene_Home_Map::loadLevel(const std::string& filename)
{
	
}

std::shared_ptr<Entity> Scene_Home_Map::player()
{
	return nullptr;
}

void Scene_Home_Map::spawnPlayer()
{

}

void Scene_Home_Map::update()
{

}

void Scene_Home_Map::sMovement()
{

}

void Scene_Home_Map::sAI()
{

}

void Scene_Home_Map::sStatus()
{
	
}

void Scene_Home_Map::sCollision()
{

}

void Scene_Home_Map::sDoAction(const Action& action)
{
	
}

void Scene_Home_Map::sAnimation()
{
	
}

void Scene_Home_Map::sCamera()
{
	
}

void Scene_Home_Map::onEnd()
{
	
}

void Scene_Home_Map::sGui()
{
	
}

void Scene_Home_Map::sRender()
{
	if (!m_paused) { m_game->window().clear(sf::Color(252, 216, 168)); }
	else { m_game->window().clear(sf::Color(50, 50, 150)); }

	sf::RectangleShape tick({ 1.0f, 6.0f });
	tick.setFillColor(sf::Color::Black);

	if (m_drawTextures)
	{
		for (auto& e : m_entityManager.getEntities())
		{
			auto& transform = e->get<CTransform>();
			sf::Color c = sf::Color::White;
			if (e->has<CAnimation>())
			{
				auto& animation = e->get<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				animation.getSprite().setColor(c);
				m_game->window().draw(animation.getSprite());
			}
		}

		// draw entity health bars
		for (auto& e : m_entityManager.getEntities())
		{
			auto& transform = e->get<CTransform>();
			if (e->has<CHealth>())
			{
				auto& h = e->get<CHealth>();
				Vec2 size(64, 6);
				sf::RectangleShape rect({ size.x, size.y });
				rect.setPosition(transform.pos.x - 32, transform.pos.y - 48);
				rect.setFillColor(sf::Color(96, 96, 96));
				rect.setOutlineColor(sf::Color::Black);
				rect.setOutlineThickness(2);
				m_game->window().draw(rect);

				float ratio = (float)h.current / h.max;
				size.x *= ratio;
				rect.setSize({ size.x, size.y });
				rect.setFillColor(sf::Color(255, 0, 0));
				rect.setOutlineThickness(0);
				m_game->window().draw(rect);

				for (int i = 0; i < h.max; i++)
				{
					tick.setPosition(rect.getPosition() + sf::Vector2f((float)(i * 64 / h.max), 0));
					m_game->window().draw(tick);
				}
			}
		}
	}

	// draw collision boxes
	if (m_drawCollision)
	{
		sf::CircleShape dot(4);
		dot.setOrigin(4, 4);
		dot.setFillColor(sf::Color::Black);
		for (auto& e : m_entityManager.getEntities())
		{
			if (e->has<CBoundingBox>())
			{
				auto& box = e->get<CBoundingBox>();
				auto& transform = e->get<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));

				if (box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Black); }
				if (box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Blue); }
				if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Red); }
				if (!box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::White); }
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}
		}
	}

	if (m_drawGrid)
	{
		float leftX = m_game->window().getView().getCenter().x - (float)width() / 2;
		float rightX = leftX + width() + m_gridSize.x;
		float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);
		float topY = m_game->window().getView().getCenter().y - (float)height() / 2;
		float bottomY = topY + height() + m_gridSize.y;
		float nextGridY = topY - ((int)topY % (int)m_gridSize.y);

		for (float y = nextGridY; y < bottomY; y += m_gridSize.y)
		{
			drawLine(Vec2(leftX, y), Vec2(rightX, y));

			for (float x = nextGridX; x < rightX; x += m_gridSize.x)
			{
				drawLine(Vec2(x, topY), Vec2(x, bottomY));

				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				m_gridText.setString("(" + xCell + ", " + yCell + ")");
				m_gridText.setPosition(x + 3, y + 2);
				m_game->window().draw(m_gridText);
			}
		}
	}

	ImGui::SFML::Render(m_game->window());
}
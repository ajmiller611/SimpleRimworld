#include "Scene_Level_Editor.h"
#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Action.h"
#include "Components.h"
#include "Physics.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

Scene_Level_Editor::Scene_Level_Editor(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_levelPath(levelPath)
{
	init();
}

void Scene_Level_Editor::init()
{
	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("Tech"));

	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Escape, "QUIT");

	std::ifstream file("config.txt");
	std::string str;
	while (file.good())
	{
		file >> str;
		if (str == "EntityTypes")
		{
			getline(file, str);
			std::stringstream ss(str);
			std::string token;
			while (ss >> token) { m_entityTypes.push_back(token); }
		}
	}
}

void Scene_Level_Editor::update()
{
	m_entityManager.update();

	sDragAndDrop();
	sGui();
}

void Scene_Level_Editor::onEnd()
{
	m_game->changeScene("Menu", nullptr, true);
}

Vec2 Scene_Level_Editor::windowToWorld(const Vec2& window) const
{
	auto& view = m_game->window().getView();

	float wx = view.getCenter().x - (m_game->window().getSize().x / 2.0f);
	float wy = view.getCenter().y - (m_game->window().getSize().y / 2.0f);

	return Vec2(window.x + wx, window.y + wy);
}

void Scene_Level_Editor::sDoAction(const Action& action)
{
	if (action.name() == "MOUSE_MOVE")
	{
		m_mousePos = action.pos();
	}

	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{

		}
		else if (action.name() == "DOWN")
		{
			
		}
		else if (action.name() == "LEFT")
		{
			
		}
		else if (action.name() == "RIGHT")
		{

		}
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "QUIT") { onEnd(); }


		else if (action.name() == "LEFT_CLICK")
		{
			Physics phy;
			for (auto& e : m_entityManager.getEntities())
			{
				Vec2 wPos = windowToWorld(m_mousePos);
				if (phy.IsInside(wPos, e))
				{
					if (!e->has<CDraggable>()) { continue; }

					auto& dragging = e->get<CDraggable>().dragging;
					dragging = !dragging;
				}
			}
		}
		
	}
}

void Scene_Level_Editor::sDragAndDrop()
{
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->has<CDraggable>() && e->get<CDraggable>().dragging)
		{
			Vec2 wPos = windowToWorld(m_mousePos);
			e->get<CTransform>().pos = wPos;
		}
	}
}

void Scene_Level_Editor::sCamera()
{

}

void Scene_Level_Editor::sGui()
{
	ImGui::Begin("Assests and Debug");
	if (ImGui::BeginTabBar("Tab Bar"))
	{
		if (ImGui::BeginTabItem("Debug"))
		{
			ImGui::Checkbox("Draw Grid", &m_drawGrid);
			ImGui::Checkbox("Draw Textures", &m_drawTextures);
			ImGui::Checkbox("Draw Debug", &m_drawCollision);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Animations"))
		{
			combo_preview_value = m_entityTypes[selected_index].c_str();
			if (ImGui::BeginCombo("Type", combo_preview_value))
			{
				for (size_t n = 0; n < m_entityTypes.size(); ++n)
				{
					const bool is_selected = (selected_index == n);
					if (ImGui::Selectable(m_entityTypes[n].c_str(), is_selected))
					{
						selected_index = n;
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Checkbox("Block Movement", &m_blockMoveCheckbox);
			ImGui::SameLine();
			ImGui::Checkbox("Block Vision", &m_blockVisionCheckbox);
			ImGui::SliderInt("Bounding Box Width", &m_boundingBoxWidth, 0, m_gridSize.x);
			ImGui::SliderInt("Bounding Box Height", &m_boundingBoxHeight, 0, m_gridSize.y);



			int count = 0;
			for (const auto& [name, anim] : m_game->assets().getAnimations())
			{
				count++;
				if (ImGui::ImageButton(("id##" + std::to_string(count)).c_str(), anim.getSprite(), sf::Vector2f(32, 32)))
				{
					auto entity = m_entityManager.addEntity(m_entityTypes[selected_index]);
					entity->add<CAnimation>(anim, true);
					auto wPos = windowToWorld(m_mousePos);
					entity->add<CTransform>(wPos);
					entity->add<CBoundingBox>(Vec2(m_boundingBoxWidth, m_boundingBoxHeight), m_blockMoveCheckbox, m_blockVisionCheckbox);
					entity->add<CDraggable>().dragging = true;
				}
				if (count % 6 != 0)
				{
					ImGui::SameLine();
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Entity Manager"))
		{
			if (ImGui::CollapsingHeader("Entities by Tag"))
			{
				ImGui::Indent(20.0f);
				if (ImGui::CollapsingHeader("enemies"))
				{
					// check for the key named "bullet" is existing in map before querying the map
					if (m_entityManager.getEntityMap().find("npc") != m_entityManager.getEntityMap().end())
					{
						ImGui::Indent(20.0f);
						for (auto& e : m_entityManager.getEntityMap().at("npc"))
						{
							if (ImGui::Button(("D##" + std::to_string(e->id())).c_str()))
							{
								e->destroy();
							}
							ImGui::SameLine();
							ImGui::Text(std::to_string(e->id()).c_str());
							ImGui::SameLine();
							ImGui::Text(e->tag().c_str());
							ImGui::SameLine();
							ImGui::Text(e->get<CAnimation>().animation.getName().c_str());
							ImGui::SameLine();
							ImGui::Text(("(" + std::to_string((int)e->get<CTransform>().pos.x) + "," +
								std::to_string((int)e->get<CTransform>().pos.y) + ")").c_str());
						}
						ImGui::Unindent(20.0f);
					}
				}
				if (ImGui::CollapsingHeader("tile"))
				{
					// check for the key named "tile" is existing in map before querying the map
					if (m_entityManager.getEntityMap().find("tile") != m_entityManager.getEntityMap().end())
					{
						ImGui::Indent(20.0f);
						for (auto& e : m_entityManager.getEntityMap().at("tile"))
						{
							if (ImGui::Button(("D##" + std::to_string(e->id())).c_str()))
							{
								e->destroy();
							}
							ImGui::SameLine();
							ImGui::Text(std::to_string(e->id()).c_str());
							ImGui::SameLine();
							ImGui::Text(e->tag().c_str());
							ImGui::SameLine();
							ImGui::Text(e->get<CAnimation>().animation.getName().c_str());
							ImGui::SameLine();
							ImGui::Text(("(" + std::to_string((int)e->get<CTransform>().pos.x) + "," +
								std::to_string((int)e->get<CTransform>().pos.y) + ")").c_str());
						}
						ImGui::Unindent(20.0f);
					}
				}
				if (ImGui::CollapsingHeader("player"))
				{
					// check for the key named "player" is existing in map before querying the map
					if (m_entityManager.getEntityMap().find("player") != m_entityManager.getEntityMap().end())
					{
						ImGui::Indent(20.0f);
						for (auto& e : m_entityManager.getEntityMap().at("player"))
						{
							if (ImGui::Button(("D##" + std::to_string(e->id())).c_str()))
							{
								e->destroy();
							}
							ImGui::SameLine();
							ImGui::Text(std::to_string(e->id()).c_str());
							ImGui::SameLine();
							ImGui::Text(e->tag().c_str());
						}

						ImGui::Unindent(20.0f);
					}
				}
				ImGui::Unindent(20.0f);
			}

			if (ImGui::CollapsingHeader("All Entities"))
			{
				ImGui::Indent(20.0f);
				for (auto& e : m_entityManager.getEntities())
				{
					if (ImGui::Button(("D##" + std::to_string(e->id())).c_str()))
					{
						e->destroy();
					}
					ImGui::SameLine();
					ImGui::Text(std::to_string(e->id()).c_str());
					ImGui::SameLine();
					ImGui::Text(e->tag().c_str());
					ImGui::SameLine();
					ImGui::Text(e->get<CAnimation>().animation.getName().c_str());
					ImGui::SameLine();
					ImGui::Text(("(" + std::to_string((int)e->get<CTransform>().pos.x) + "," +
						std::to_string((int)e->get<CTransform>().pos.y) + ")").c_str());
				}
				ImGui::Unindent(20.0f);
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void Scene_Level_Editor::sRender()
{
	m_game->window().clear(sf::Color::Black);

	m_mouseDot.setFillColor(sf::Color::Red);
	m_mouseDot.setRadius(8);
	m_mouseDot.setOrigin(8, 8);
	Vec2 worldPos = windowToWorld(m_mousePos);
	m_mouseDot.setPosition(worldPos.x, worldPos.y);
	m_game->window().draw(m_mouseDot);

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
	}

	if (m_drawCollision)
	{
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
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

Vec2 Scene_Level_Editor::windowToWorld(const Vec2& window) const
{
	auto& view = m_game->window().getView();

	float wx = view.getCenter().x - (m_game->window().getSize().x / 2.0f);
	float wy = view.getCenter().y - (m_game->window().getSize().y / 2.0f);

	return Vec2(window.x + wx, window.y + wy);
}

void Scene_Level_Editor::update()
{
	m_entityManager.update();

	sDragAndDrop();
	sGui();
}

void Scene_Level_Editor::onEnd()
{
	m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game), true);
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
			// Prevent mouse clicks from registering under the ImGui window.
			// This prevents an entity currently being dragged from being dropped under the ImGui window.
			if (!ImGui::GetIO().WantCaptureMouse) 
			{
				Vec2 wMousePos = windowToWorld(m_mousePos);
				if (m_entityBeingDragged != nullptr)
				{
					bool cellOccupied = false;
					int gridX = wMousePos.x / m_gridSize.x;
					int gridY = wMousePos.y / m_gridSize.y;
					int topLeftX = (int)gridX * (int)m_gridSize.x;
					int topLeftY = (int)gridY * (int)m_gridSize.y;
					Vec2 newPos(topLeftX + m_gridSize.x / 2, topLeftY + m_gridSize.y / 2);
					for (auto& e : m_entityManager.getEntities())
					{
						if (newPos == e->get<CTransform>().pos && m_entityBeingDragged != e)
						{
							cellOccupied = true;
							break;
						}
					}
					if (!cellOccupied)
					{
						auto& dragging = m_entityBeingDragged->get<CDraggable>().dragging;
						dragging = !dragging;
						m_entityBeingDragged->get<CTransform>().pos = newPos;
						m_entityBeingDragged = nullptr;
					}
				}
				else
				{
					Physics phy;
					for (auto& e : m_entityManager.getEntities())
					{
						if (!e->has<CDraggable>()) { continue; }

						if (phy.IsInside(wMousePos, e))
						{
							auto& dragging = e->get<CDraggable>().dragging;
							dragging = !dragging;
							if (dragging) {	m_entityBeingDragged = e; }
							else { m_entityBeingDragged = nullptr; }
						}
					}
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
			ImGui::BeginGroup();
			{
				// create padding for the selected animation
				ImGui::Dummy(ImVec2(0.0f, 45.0f));
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				if (m_animationSelected.getName() != "none")
				{
					if (ImGui::ImageButton("selectArea", m_animationSelected.getSprite(), sf::Vector2f(m_gridSize.x, m_gridSize.y)))
					{
						// create the entity and set the draggable component to true so the drag and drop system
						// can starting dragging the entity
						if (m_entityBeingDragged == nullptr)
						{
							auto entity = m_entityManager.addEntity(m_entityTypes[m_animTypeComboSelectedIndex]);
							entity->add<CAnimation>(m_animationSelected, true);
							auto wPos = windowToWorld(m_mousePos);
							entity->add<CTransform>(wPos);
							entity->add<CBoundingBox>(Vec2(abs(m_boundingBoxLeft - m_boundingBoxRight), abs(m_boundingBoxTop - m_boundingBoxBottom)), m_blockMoveCheckbox, m_blockVisionCheckbox);
							entity->add<CDraggable>().dragging = true;
							m_entityBeingDragged = entity;
							m_animationSelected = Animation();
						}
					}

					// get positioning information for the drawing of the bounding box
					ImVec2 buttonPos = ImGui::GetItemRectMin(); // Capture the position of the button
					ImGuiStyle style;
					buttonPos.x += style.FramePadding.x;        // offset the x
					buttonPos.y += style.FramePadding.y;        // offset the y
					m_widgetPos = Vec2(buttonPos.x, buttonPos.y);
				}
				else
				{
					// no animation selected so switch to an empty button
					if (ImGui::Button("##empty", sf::Vector2f(m_gridSize.x, m_gridSize.y))) {}
				}
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::EndGroup();
			}
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				ImGui::PushItemWidth(300.0f);
				m_animTypeComboPreviewValue = m_entityTypes[m_animTypeComboSelectedIndex].c_str();
				if (ImGui::BeginCombo("Type", m_animTypeComboPreviewValue))
				{
					for (size_t n = 0; n < m_entityTypes.size(); ++n)
					{
						const bool isSelected = (m_animTypeComboSelectedIndex == n);
						if (ImGui::Selectable(m_entityTypes[n].c_str(), isSelected))
						{
							m_animTypeComboSelectedIndex = n;
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				ImGui::Checkbox("Block Movement", &m_blockMoveCheckbox);
				ImGui::SameLine(0, 60);
				ImGui::Checkbox("Block Vision", &m_blockVisionCheckbox);

				ImGui::SeparatorText("Bounding Box Parameters");

				// manual inputs allows values outside of the range so disable it
				static ImGuiSliderFlags flags = ImGuiSliderFlags_NoInput;

				ImGui::PushItemWidth(300.0f);
				// bounding box sliders
				ImGui::SliderInt("Left", reinterpret_cast<int*>(&m_boundingBoxLeft), 0, m_gridSize.x, "%d", flags);
				ImGui::SliderInt("Right", reinterpret_cast<int*>(&m_boundingBoxRight), 0, m_gridSize.x, "%d", flags);
				ImGui::SliderInt("Top", reinterpret_cast<int*>(&m_boundingBoxTop), 0, m_gridSize.x, "%d", flags);
				ImGui::SliderInt("Bottom", reinterpret_cast<int*>(&m_boundingBoxBottom), 0, m_gridSize.x, "%d", flags);
			
				ImGui::EndGroup();
			}

			int counterOfAnimations = 0;
			ImVec2 windowSize = ImGui::GetWindowSize();
			for (const auto& [name, anim] : m_game->assets().getAnimations())
			{
				counterOfAnimations++;
				
				if (ImGui::ImageButton(("id##" + std::to_string(counterOfAnimations)).c_str(), anim.getSprite(), sf::Vector2f(64, 64)))
				{
					m_animationSelected = anim;
				}
				ImGuiStyle style;
				int buttonsPerRow = (int)ImGui::GetWindowSize().x / ((int)ImGui::GetItemRectSize().x + (style.FramePadding.x * 2.0f));
				if (buttonsPerRow != 0)
				{
					if (counterOfAnimations % buttonsPerRow != 0)
					{
						ImGui::SameLine();
					}
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

	if (m_drawTextures)
	{
		for (auto& e : m_entityManager.getEntities())
		{
			// skip over the entity being dragged as we want that entity to be drawn last and not drawn twice
			if (e == m_entityBeingDragged) { continue; }

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

	// draw visual representation of a bounding box that is adjustable by the sliders
	if (m_animationSelected.getName() != "none")
	{
		sf::RectangleShape rectangle;
		rectangle.setPosition(m_widgetPos.x + m_boundingBoxLeft, m_widgetPos.y + m_boundingBoxTop);
		rectangle.setSize(sf::Vector2f(m_boundingBoxRight - m_boundingBoxLeft, m_boundingBoxBottom - m_boundingBoxTop));
		rectangle.setFillColor(sf::Color::Transparent);
		rectangle.setOutlineColor(sf::Color::Red);
		rectangle.setOutlineThickness(1);
		m_game->window().draw(rectangle);
	}

	// draw the being dragged entity after the rendering of ImGui so the dragged entity is drawn on top
	if (m_entityBeingDragged != nullptr)
	{
		auto& transform = m_entityBeingDragged->get<CTransform>();
		sf::Color c = sf::Color::White;
		if (m_entityBeingDragged->has<CAnimation>())
		{
			auto& animation = m_entityBeingDragged->get<CAnimation>().animation;
			animation.getSprite().setRotation(transform.angle);
			animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
			animation.getSprite().setScale(transform.scale.x, transform.scale.y);
			animation.getSprite().setColor(c);
			m_game->window().draw(animation.getSprite());
		}
	}
}
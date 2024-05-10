#include "Scene_Home_Map.h"
#include "Scene_Menu.h"
#include "Physics.h"
#include "GameEngine.h"

#include <fstream>
#include <iostream>
#include <string>

Scene_Home_Map::Scene_Home_Map(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Home_Map::init(const std::string& levelPath)
{
	loadLevel(levelPath);

	m_gridText.setCharacterSize(12);
	m_gridText.setFillColor(sf::Color::Black);
	m_gridText.setFont(m_game->assets().getFont("Tech"));
	
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");

	spawnPlayer();
}

void Scene_Home_Map::loadLevel(const std::string& filename)
{
	m_entityManager = EntityManager();

	std::ifstream file(filename);
	if (!file) { std::cerr << "Failed to open file " << filename; }

	std::string str;
	while (file >> str)
	{
		if (str == "Tile")
		{
			std::shared_ptr<Entity> entity;
			entity = m_entityManager.addEntity(str);
			file >> str;
			entity->add<CAnimation>(m_game->assets().getAnimation(str), true);

			int gridX, gridY;
			file >> gridX >> gridY;
			float x = gridX * m_gridSize.x + (m_gridSize.x / 2);
			float y = gridY * m_gridSize.y + (m_gridSize.y / 2);
			entity->add<CTransform>(Vec2(x, y));

			float bbPosX, bbPosY, bbOffsetX, bbOffsetY, bbWidth, bbHeight;
			bool blockMove, blockVision;
			file >> bbPosX >> bbPosY >> bbOffsetX >> bbOffsetY
				>> bbWidth >> bbHeight >> blockMove >> blockVision;
			entity->add<CBoundingBox>(Vec2(bbPosX, bbPosY), Vec2(bbOffsetX, bbOffsetY),
				Vec2(bbWidth, bbHeight), blockMove, blockVision);
		}
		else if (str == "Decoration")
		{
			std::shared_ptr<Entity> entity;
			entity = m_entityManager.addEntity(str);
			file >> str;
			entity->add<CAnimation>(m_game->assets().getAnimation(str), true);

			int gridX, gridY;
			file >> gridX >> gridY;
			float x = gridX * m_gridSize.x + (m_gridSize.x / 2);
			float y = gridY * m_gridSize.y + (m_gridSize.y / 2);
			entity->add<CTransform>(Vec2(x, y));
		}
		else { std::cout << "Invalid entity type: " + str << "\n"; }
	}
}

std::shared_ptr<Entity> Scene_Home_Map::player()
{
	return m_entityManager.getEntityMap().at("Player").front();
}

void Scene_Home_Map::spawnPlayer()
{
	auto entity = m_entityManager.addEntity("Player");
	entity->add<CAnimation>(m_game->assets().getAnimation("GreenCharacter"), true);
	entity->add<CTransform>(Vec2(32, 224));
	entity->add<CBoundingBox>(entity->get<CTransform>().pos, Vec2(0, 0), Vec2(40, 40));
	entity->add<CInput>();

	auto hand = m_entityManager.addEntity("Decoration");
	hand->add<CAnimation>(m_game->assets().getAnimation("GreenHand"), true);
	hand->add<CTransform>(entity->get<CTransform>().pos + Vec2(28, 16));

	entity->add<CHand>(hand->id(), Vec2(28, 16));
}

void Scene_Home_Map::update()
{
	m_entityManager.update();

	sMovement();
	sCollision();
	sGui();
}

void Scene_Home_Map::sMovement()
{
	auto& pInput = player()->get<CInput>();
	auto& pTransform = player()->get<CTransform>();
	pTransform.prevPos = pTransform.pos;
	Vec2 playerVelocity(0, 0);

	if (pInput.up)
	{
		playerVelocity.y -= 3;
		pTransform.facing = Vec2(0, -1);
	}
	else if (pInput.down)
	{
		playerVelocity.y += 3;
		pTransform.facing = Vec2(0, 1);
	}
	else if (pInput.left)
	{
		playerVelocity.x -= 3;
		pTransform.facing = Vec2(-1, 0);
	}
	else if (pInput.right)
	{
		playerVelocity.x += 3;
		pTransform.facing = Vec2(1, 0);
	}

	pTransform.velocity = playerVelocity;

	// Update the entities positions
	for (auto& e : m_entityManager.getEntities())
	{
		auto& eTransform = e->get<CTransform>();
		eTransform.pos += eTransform.velocity;
		e->get<CBoundingBox>().pos += eTransform.velocity;

		if (e->has<CHand>())
		{
			auto hand = m_entityManager.getEntity(e->get<CHand>().entityID);

			Vec2 newPos = hand->get<CTransform>().pos;
			float x = 0, y = 0;
			if (eTransform.facing.x != 0)
			{
				x = eTransform.pos.x + (e->get<CHand>().offset.x * eTransform.facing.x);
				y = eTransform.pos.y + (e->get<CHand>().offset.y * eTransform.facing.x);
				newPos = Vec2(x, y);
			}
			else if (eTransform.facing.y != 0)
			{
				// For hand placement when facing up or down, the inverse of the offset is applied.
				// To keep the hand representing the right side hand, the opposite of the y-offset is needed.
				x = eTransform.pos.x + (-e->get<CHand>().offset.y * eTransform.facing.y);
				y = eTransform.pos.y + (e->get<CHand>().offset.x * eTransform.facing.y);
				newPos = Vec2(x, y);
			}
			hand->get<CTransform>().pos = newPos + eTransform.velocity;
		}
	}
}

void Scene_Home_Map::sAI()
{

}

void Scene_Home_Map::sStatus()
{
	
}

Scene_Home_Map::Collision Scene_Home_Map::collided(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& aBoundingBox = a->get<CBoundingBox>();
	auto& bBoundingBox = b->get<CBoundingBox>();
	Vec2 overlap = Physics::GetOverlap(a, b);
	Vec2 previousOverlap = Physics::GetPreviousOverlap(a, b);

	if (a->tag() == "Player")
	{
		if (overlap.x > 0 && overlap.y > 0)
		{
			// Entity 'a' collison on the left side
			if (previousOverlap.y > 0 && aBoundingBox.pos.x > bBoundingBox.pos.x) { return { true, overlap, 'l' }; }
			// Entity 'a' collison on the right side
			else if (previousOverlap.y > 0 && aBoundingBox.pos.x < bBoundingBox.pos.x) { return { true, overlap, 'r' }; }
			// Entity 'a' collison on the top side
			else if (previousOverlap.x > 0 && aBoundingBox.pos.y > bBoundingBox.pos.y) { return { true, overlap, 't' }; }
			// Entity 'a' collison on the bottom side
			else if (previousOverlap.x > 0 && aBoundingBox.pos.y < bBoundingBox.pos.y) { return { true, overlap, 'b' }; }
		}
	}
	else
	{
		if (overlap.x > 0 && overlap.y > 0)
		{
			return { true, overlap, '\0' };
		}
	}

	return { false, Vec2(0,0), '\0' };
}

void Scene_Home_Map::sCollision()
{
	auto& tiles = m_entityManager.getEntityMap().at("Tile");

	for (auto& e : m_entityManager.getEntities())
	{
		for (auto& tile : tiles)
		{
			if (tile->get<CBoundingBox>().blockMove)
			{
				auto& entityTransform = e->get<CTransform>();
				auto& entityBoundingBox = e->get<CBoundingBox>();

				Collision result = collided(e, tile);
				if (result.collided)
				{
					switch (result.direction)
					{
					case 'l':
						entityTransform.pos.x += result.overlap.x;
						entityBoundingBox.pos.x += result.overlap.x;
						if (e->has<CHand>())
						{
							auto hand = m_entityManager.getEntity(e->get<CHand>().entityID);
							hand->get<CTransform>().pos.x += result.overlap.x;
						}
						break;
					case 'r':
						entityTransform.pos.x -= result.overlap.x;
						entityBoundingBox.pos.x -= result.overlap.x;
						if (e->has<CHand>())
						{
							auto hand = m_entityManager.getEntity(e->get<CHand>().entityID);
							hand->get<CTransform>().pos.x -= result.overlap.x;
						}
						break;
					case 't': 
						entityTransform.pos.y += result.overlap.y;
						entityBoundingBox.pos.y += result.overlap.y;
						if (e->has<CHand>())
						{
							auto hand = m_entityManager.getEntity(e->get<CHand>().entityID);
							hand->get<CTransform>().pos.y += result.overlap.y;
						}
						break;
					case 'b':
						entityTransform.pos.y -= result.overlap.y;
						entityBoundingBox.pos.y -= result.overlap.y;
						if (e->has<CHand>())
						{
							auto hand = m_entityManager.getEntity(e->get<CHand>().entityID);
							hand->get<CTransform>().pos.y -= result.overlap.y;
						}
						break;
					case '\0':
						// For now enemies don't detect which direction they are colliding from.
						entityTransform.pos -= entityTransform.velocity;
					}
				}
			}
		}
	}
}

void Scene_Home_Map::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP") { player()->get<CInput>().up = true; }
		else if (action.name() == "DOWN") { player()->get<CInput>().down = true; }
		else if (action.name() == "LEFT") { player()->get<CInput>().left = true; }
		else if (action.name() == "RIGHT") { player()->get<CInput>().right = true; }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "QUIT") { onEnd(); }
	}
	else if (action.type() == "END")
	{
		if (action.name() == "UP") { player()->get<CInput>().up = false; }
		else if (action.name() == "DOWN") { player()->get<CInput>().down = false; }
		else if (action.name() == "LEFT") { player()->get<CInput>().left = false; }
		else if (action.name() == "RIGHT") { player()->get<CInput>().right = false; }
	}
}

void Scene_Home_Map::sAnimation()
{
	
}

void Scene_Home_Map::sCamera()
{
	
}

void Scene_Home_Map::onEnd()
{
	m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game), true);
}

void Scene_Home_Map::sGui()
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

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
			ImGui::BeginChild("Animations", ImVec2(ImGui::GetContentRegionAvail()), ImGuiChildFlags_None, window_flags);

			int counterOfAnimations = 0;
			ImVec2 windowSize = ImGui::GetWindowSize();
			for (const auto& [name, anim] : m_game->assets().getAnimations())
			{
				counterOfAnimations++;

				if (ImGui::ImageButton(("id##" + std::to_string(counterOfAnimations)).c_str(), anim.getSprite(), sf::Vector2f(64, 64)))
				{
					
				}
				ImGuiStyle style;
				int buttonsPerRow = (int)ImGui::GetWindowSize().x / (int)(ImGui::GetItemRectSize().x + (style.FramePadding.x * 2.0f));
				if (buttonsPerRow != 0)
				{
					if (counterOfAnimations % buttonsPerRow != 0)
					{
						ImGui::SameLine();
					}
				}
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Entity Manager"))
		{
			if (ImGui::CollapsingHeader("Entities by Tag"))
			{
				ImGui::Indent(20.0f);
				if (ImGui::CollapsingHeader("tile"))
				{
					// check for the key named "Tile" is existing in map before querying the map
					if (m_entityManager.getEntityMap().find("Tile") != m_entityManager.getEntityMap().end())
					{
						ImGui::Indent(20.0f);
						for (auto& e : m_entityManager.getEntityMap().at("Tile"))
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
							ImGui::Text(("Pos: (" + std::to_string((int)e->get<CTransform>().pos.x) + "," +
								std::to_string((int)e->get<CTransform>().pos.y) + ")").c_str());
							ImGui::SameLine();
							ImGui::Text(("BBpos: (" + std::to_string((int)e->get<CBoundingBox>().pos.x) + "," + std::to_string((int)e->get<CBoundingBox>().pos.y) + ")" +
								" BBOffset: (" + std::to_string((int)e->get<CBoundingBox>().offset.x) + ", " + std::to_string((int)e->get<CBoundingBox>().offset.y) + ")").c_str());
						}
						ImGui::Unindent(20.0f);
					}
				}
				if (ImGui::CollapsingHeader("decoration"))
				{
					// check for the key named "Tile" is existing in map before querying the map
					if (m_entityManager.getEntityMap().find("Decoration") != m_entityManager.getEntityMap().end())
					{
						ImGui::Indent(20.0f);
						for (auto& e : m_entityManager.getEntityMap().at("Decoration"))
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
				if (ImGui::CollapsingHeader("enemies"))
				{
					// check for the key named "bullet" is existing in map before querying the map
					if (m_entityManager.getEntityMap().find("Enemies") != m_entityManager.getEntityMap().end())
					{
						ImGui::Indent(20.0f);
						for (auto& e : m_entityManager.getEntityMap().at("Enemies"))
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
					// check for the key named "Player" is existing in map before querying the map
					if (m_entityManager.getEntityMap().find("Player") != m_entityManager.getEntityMap().end())
					{
						ImGui::Indent(20.0f);
						for (auto& e : m_entityManager.getEntityMap().at("Player"))
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
				rect.setPosition(box.pos.x, box.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));

				if (box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Red); }
				if (box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Blue); }
				if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Green); }
				if (!box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Magenta); }
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
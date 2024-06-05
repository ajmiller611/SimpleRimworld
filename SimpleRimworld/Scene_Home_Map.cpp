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
		else if (str == "Weapon")
		{
			// Variant allows this vector to hold multiple components even though they are different data types.
			std::vector<std::variant<CAnimation, CBoundingBox>> weaponVec;

			file >> str;
			CAnimation anim(m_game->assets().getAnimation(str), true);
			weaponVec.push_back(anim);
			
			float bbOffsetX, bbOffsetY, bbSizeX, bbSizeY;
			file >> bbOffsetX >> bbOffsetY >> bbSizeX >> bbSizeY;
			CBoundingBox bb(Vec2(0, 0), Vec2(bbOffsetX, bbOffsetY), Vec2(bbSizeX, bbSizeY));
			weaponVec.push_back(bb);

			// "str" still holds the animation name of the weapon which will be used as the key to identifiy weapons.
			m_weaponsMap[str] = weaponVec;
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
	hand->add<CTransform>(entity->get<CTransform>().pos + Vec2(16, -28));

	entity->add<CHand>(hand->id(), Vec2(28, 16));
}

void Scene_Home_Map::spawnWeapon(std::shared_ptr<Entity> e, const std::string& name)
{
	auto weapon = m_entityManager.addEntity("Weapon");
	e->get<CHand>().weaponID = weapon->id();

	// Using the variant class template to get the animation. This could also be retrieved from the assets class.
	weapon->add<CAnimation>(std::get<CAnimation>(m_weaponsMap[name].at(0)).animation, true);

	auto hand = m_entityManager.getEntity(e->get<CHand>().entityID);
	weapon->add<CTransform>(hand->get<CTransform>().pos + Vec2(0, -e->get<CBoundingBox>().halfSize.y));

	// get the bounding box positional data from the variant class template to create the weapon's bounding box
	auto& bb = std::get<CBoundingBox>(m_weaponsMap[name].at(1));
	weapon->add<CBoundingBox>(weapon->get<CTransform>().pos + bb.offset, Vec2(bb.offset.x, bb.offset.y), bb.size);
}

void Scene_Home_Map::update()
{
	m_entityManager.update();

	sMovement();
	sCollision();
	sGui();
}

void Scene_Home_Map::moveHand(std::shared_ptr<Entity> e)
{
	auto& eTransform = e->get<CTransform>();
	auto hand = m_entityManager.getEntity(e->get<CHand>().entityID);

	Vec2 newHandPos = hand->get<CTransform>().pos;
	float x = 0, y = 0;
	if (eTransform.facing.x != 0)
	{
		x = eTransform.pos.x + (e->get<CHand>().offset.x * eTransform.facing.x);
		y = eTransform.pos.y + (e->get<CHand>().offset.y * eTransform.facing.x);
		newHandPos = Vec2(x, y);
	}
	else if (eTransform.facing.y != 0)
	{
		// For hand placement when facing up or down, the inverse of the offset is applied.
		// To keep the hand representing the right side hand, the opposite of the y-offset is needed.
		x = eTransform.pos.x + (-e->get<CHand>().offset.y * eTransform.facing.y);
		y = eTransform.pos.y + (e->get<CHand>().offset.x * eTransform.facing.y);
		newHandPos = Vec2(x, y);
	}
	hand->get<CTransform>().pos = newHandPos + eTransform.velocity;
}

void Scene_Home_Map::moveWeapon(std::shared_ptr<Entity> e)
{
	auto& eTransform = e->get<CTransform>();
	auto hand = m_entityManager.getEntity(e->get<CHand>().entityID);
	auto weapon = m_entityManager.getEntity(e->get<CHand>().weaponID);
	auto& wTransform = weapon->get<CTransform>();
	auto& wBB = weapon->get<CBoundingBox>();
	Vec2 offset = weapon->get<CBoundingBox>().offset;

	if (eTransform.facing.x != 0)
	{
		wTransform.pos.x = hand->get<CTransform>().pos.x + (e->get<CBoundingBox>().halfSize.x * eTransform.facing.x);
		wTransform.pos.y = hand->get<CTransform>().pos.y;

		// entity facing right
		if (eTransform.facing.x == 1)
		{
			wTransform.scale.x = eTransform.facing.x;
			wTransform.angle = 90;
			offset = Vec2(-wBB.offset.y, wBB.offset.x);
		}
		// entity facing left
		else if (eTransform.facing.x == -1)
		{
			wTransform.scale.x = -eTransform.facing.x;
			wTransform.angle = -90;
			offset = Vec2(wBB.offset.y, -wBB.offset.x);
		}
	}
	else if (eTransform.facing.y != 0)
	{
		wTransform.pos.x = hand->get<CTransform>().pos.x;
		wTransform.pos.y = hand->get<CTransform>().pos.y + (e->get<CBoundingBox>().halfSize.y * eTransform.facing.y);

		// entity facing down
		if (eTransform.facing.y == 1)
		{
			wTransform.scale.y = eTransform.facing.y;
			wTransform.angle = 180;
			offset = Vec2(-wBB.offset.x, -wBB.offset.y);
		}
		// entity facing up
		else if (eTransform.facing.y == -1)
		{
			wTransform.scale.y = -eTransform.facing.y;
			wTransform.angle = 0;
			offset = Vec2(wBB.offset.x, wBB.offset.y);
		}
	}

	weapon->get<CTransform>().pos += eTransform.velocity;
	weapon->get<CBoundingBox>().pos = weapon->get<CTransform>().pos + offset + eTransform.velocity;
}

void Scene_Home_Map::sMovement()
{
	auto& pInput = player()->get<CInput>();
	auto& pTransform = player()->get<CTransform>();
	pTransform.prevPos = pTransform.pos;
	Vec2 pVelocity(0, 0);

	if (pInput.up)
	{
		pVelocity.y -= 3;
		pTransform.facing = Vec2(0, -1);
	}
	else if (pInput.down)
	{
		pVelocity.y += 3;
		pTransform.facing = Vec2(0, 1);
	}
	else if (pInput.left)
	{
		pVelocity.x -= 3;
		pTransform.facing = Vec2(-1, 0);
	}
	else if (pInput.right)
	{
		pVelocity.x += 3;
		pTransform.facing = Vec2(1, 0);
	}

	pTransform.velocity = pVelocity;

	// Update the entities positions
	for (auto& e : m_entityManager.getEntities())
	{
		auto& eTransform = e->get<CTransform>();
		eTransform.pos += eTransform.velocity;
		e->get<CBoundingBox>().pos += eTransform.velocity;

		if (e->has<CHand>())
		{
			moveHand(e);

			if (e->get<CHand>().weaponID >= 0)
			{
				moveWeapon(e);
			}
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
		if (ImGui::BeginTabItem("Player Info"))
		{
			ImGui::SeparatorText("Entity Info");
			ImGui::Text(("Entity ID: " + std::to_string(player()->id())).c_str());
			ImGui::Text(("Tag: " + player()->tag()).c_str());
			ImGui::Text(("IsActive: " + std::to_string(player()->isActive())).c_str());

			for (auto& component : player()->getComponentList())
			{
				if (component == "CTransform")
				{
					ImGui::SeparatorText("Transform");
					ImGui::Text(("Position X: " + std::to_string(player()->get<CTransform>().pos.x) + " ").c_str());
					ImGui::SameLine();
					ImGui::Text(("Y: " + std::to_string(player()->get<CTransform>().pos.y)).c_str());

					ImGui::Text(("Scale X: " + std::to_string(player()->get<CTransform>().scale.x) + " ").c_str());
					ImGui::SameLine();
					ImGui::Text(("Y: " + std::to_string(player()->get<CTransform>().scale.y)).c_str());

					ImGui::Text(("Velocity X: " + std::to_string(player()->get<CTransform>().velocity.x) + " ").c_str());
					ImGui::SameLine();
					ImGui::Text(("Y: " + std::to_string(player()->get<CTransform>().velocity.y)).c_str());

					ImGui::Text(("Facing X: " + std::to_string(player()->get<CTransform>().facing.x) + " ").c_str());
					ImGui::SameLine();
					ImGui::Text(("Y: " + std::to_string(player()->get<CTransform>().facing.y)).c_str());
				}
				else if (component == "CInput")
				{
					ImGui::SeparatorText("Input");
					ImGui::Text(("Up: " + std::to_string(player()->get<CInput>().up)).c_str());
					ImGui::SameLine();
					ImGui::Text(("Down: " + std::to_string(player()->get<CInput>().down)).c_str());

					ImGui::Text(("Left: " + std::to_string(player()->get<CInput>().left)).c_str());
					ImGui::SameLine();
					ImGui::Text(("Right: " + std::to_string(player()->get<CInput>().right)).c_str());

					ImGui::Text(("Attack: " + std::to_string(player()->get<CInput>().attack)).c_str());
					ImGui::SameLine();
					ImGui::Text(("CanAttack: " + std::to_string(player()->get<CInput>().canAttack)).c_str());
				}
				else if (component == "CLifespan")
				{
					ImGui::SeparatorText("Lifespan");
					ImGui::Text(("Lifespan: " + std::to_string(player()->get<CLifespan>().lifespan)).c_str());
					ImGui::Text(("Frame Created: " + std::to_string(player()->get<CLifespan>().frameCreated)).c_str());
				}
				else if (component == "CDamage")
				{
					ImGui::SeparatorText("Damage");
					ImGui::Text(("Damage: " + std::to_string(player()->get<CDamage>().damage)).c_str());
				}
				else if (component == "CInvincibility")
				{
					ImGui::SeparatorText("Invincibility");
					ImGui::Text(("iFrames: " + std::to_string(player()->get<CInvincibility>().iframes)).c_str());
				}
				else if (component == "CHealth")
				{
					ImGui::SeparatorText("Health");
					ImGui::Text(("Max: " + std::to_string(player()->get<CHealth>().max)).c_str());
					ImGui::Text(("Current: " + std::to_string(player()->get<CHealth>().current)).c_str());
				}
				else if (component == "CAnimation")
				{
					ImGui::SeparatorText("Animation");
					ImGui::Text(("Name: " + player()->get<CAnimation>().animation.getName()).c_str());

					ImGui::Text(("Size X: " + std::to_string(player()->get<CAnimation>().animation.getSize().x)).c_str());
					ImGui::SameLine();
					ImGui::Text(("Y: " + std::to_string(player()->get<CAnimation>().animation.getSize().y)).c_str());

					ImGui::Text(("Repeat: " + std::to_string(player()->get<CAnimation>().repeat)).c_str());
				}
				else if (component == "CState")
				{
					ImGui::SeparatorText("State");
					ImGui::Text(("State: " + player()->get<CState>().state).c_str());
				}
				else if (component == "CBoundingBox")
				{
					ImGui::SeparatorText("BoundingBox");
					ImGui::Text(("Position X: " + std::to_string(player()->get<CBoundingBox>().pos.x) + " ").c_str());
					ImGui::SameLine();
					ImGui::Text(("Y: " + std::to_string(player()->get<CBoundingBox>().pos.y)).c_str());

					ImGui::Text(("Offset X: " + std::to_string(player()->get<CBoundingBox>().offset.x) + " ").c_str());
					ImGui::SameLine();
					ImGui::Text(("Y: " + std::to_string(player()->get<CBoundingBox>().offset.y)).c_str());

					ImGui::Text(("Size X: " + std::to_string(player()->get<CBoundingBox>().size.x) + " ").c_str());
					ImGui::SameLine();
					ImGui::Text(("Y: " + std::to_string(player()->get<CBoundingBox>().size.y)).c_str());

					ImGui::Text(("Block Move: " + std::to_string(player()->get<CBoundingBox>().blockMove)).c_str());
					ImGui::SameLine();
					ImGui::Text(("Block Vision: " + std::to_string(player()->get<CBoundingBox>().blockVision)).c_str());
				}
				else if (component == "CDraggable")
				{
					ImGui::SeparatorText("Draggable");
					ImGui::Text(("Draggable: " + std::to_string(player()->get<CDraggable>().dragging)).c_str());
				}
				else if (component == "CHand")
				{
					ImGui::SeparatorText("Hand");
					ImGui::Text(("Entity ID: " + std::to_string(player()->get<CHand>().entityID)).c_str());
					ImGui::Text(("Weapon ID: " + std::to_string(player()->get<CHand>().weaponID)).c_str());

					ImGui::Text(("Offset X: " + std::to_string(player()->get<CHand>().offset.x) + " ").c_str());
					ImGui::SameLine();
					ImGui::Text(("Y: " + std::to_string(player()->get<CHand>().offset.y)).c_str());
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Debug"))
		{
			ImGui::Checkbox("Draw Grid", &m_drawGrid);
			ImGui::Checkbox("Draw Textures", &m_drawTextures);
			ImGui::Checkbox("Draw Collision", &m_drawCollision);
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
				if (ImGui::CollapsingHeader("weapons"))
				{
					if (m_entityManager.getEntityMap().find("Weapon") != m_entityManager.getEntityMap().end())
					{
						ImGui::Indent(20.0f);
						for (auto& e : m_entityManager.getEntityMap().at("Weapon"))
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
				rect.setRotation(transform.angle);
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
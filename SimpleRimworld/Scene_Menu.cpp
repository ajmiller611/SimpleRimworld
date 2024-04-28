#include "Scene_Menu.h"
#include "Scene_Home_Map.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <iostream>

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Menu::init()
{
	m_title = "Simple Rimworld";
	m_menuStrings.push_back("Start");
	m_menuStrings.push_back("Options");

	m_levelPaths.push_back("map_home.txt");

	m_menuText.setFont(m_game->assets().getFont("Tech"));

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");
}

void Scene_Menu::update()
{
	sRender();
}

void Scene_Menu::onEnd()
{
	m_game->quit();
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "PLAY")
		{
			m_game->changeScene("PLAY", std::make_shared<Scene_Home_Map>(m_game, m_levelPaths[m_selectedMenuIndex]));
		}
		else if (action.name() == "UP")
		{
			m_selectedMenuIndex = (m_selectedMenuIndex > 0) ? --m_selectedMenuIndex : m_menuStrings.size() - 1;
		}
		else if (action.name() == "DOWN")
		{
			m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

void Scene_Menu::sRender()
{
	auto& window = m_game->window();

	// Clear the window with intended background color
	window.clear(sf::Color(20, 22, 26));

	sf::CircleShape planet(400.0f);
	planet.setPointCount(150);
	planet.setFillColor(sf::Color(158, 97, 22));
	planet.setOrigin(planet.getRadius() / 2.0f, planet.getRadius() / 2.0f);
	planet.setPosition((width() / 2.0f) / 2.0f - planet.getOrigin().x, height() / 2.0f - planet.getOrigin().y);
	window.draw(planet);

	// Setup the title text
	m_menuText.setString(m_title);
	m_menuText.setFillColor(sf::Color::White);
	m_menuText.setCharacterSize(100);

	float posX = window.getSize().x / 2.0f -
		m_menuText.getLocalBounds().width / 2.0f;
	float posY = window.getSize().y / 2.0f -
		m_menuText.getLocalBounds().height / 2.0f;
	m_menuText.setPosition(posX, posY);
	window.draw(m_menuText);

	// Setup the selectable options
	m_menuText.setCharacterSize(40);
	m_menuTextBackground.setSize(sf::Vector2f(200, 75));
	m_menuTextBackground.setFillColor(sf::Color(158, 97, 22));

	for (int stringIndex = 0; stringIndex < m_menuStrings.size(); stringIndex++)
	{
		posX = width() - 250.0f;
		posY = (height() / 2.0f) / 2.0f + (float)stringIndex * 100;
		m_menuTextBackground.setPosition(posX, posY);

		if (m_selectedMenuIndex == stringIndex)
		{
			m_menuText.setString(m_menuStrings[stringIndex]);
			m_menuText.setFillColor(sf::Color::Black);

			// The bounding box's position of a sf:Text object is not (0, 0).
			// To correct this offset, subtract the local bounding rectangle's position.
			posX = m_menuTextBackground.getPosition().x + (m_menuTextBackground.getLocalBounds().width / 2.0f) - 
				(m_menuText.getLocalBounds().width / 2.0f) - m_menuText.getLocalBounds().left;
			posY = m_menuTextBackground.getPosition().y + (m_menuTextBackground.getLocalBounds().height / 2.0f) - 
				(m_menuText.getLocalBounds().height / 2.0f) - m_menuText.getLocalBounds().top;
			m_menuText.setPosition(posX, posY);

			// Use the outline of the rectangle shape as a highlight to show the option that would be selected.
			m_menuTextBackground.setOutlineColor(sf::Color::Red);
			m_menuTextBackground.setOutlineThickness(3);
		}
		else
		{
			m_menuText.setString(m_menuStrings[stringIndex]);
			m_menuText.setFillColor(sf::Color::White);
			posX = m_menuTextBackground.getPosition().x + (m_menuTextBackground.getLocalBounds().width / 2.0f) - 
				(m_menuText.getLocalBounds().width / 2.0f) - -m_menuText.getLocalBounds().left;
			posY = m_menuTextBackground.getPosition().y + (m_menuTextBackground.getLocalBounds().height / 2.0f) - 
				(m_menuText.getLocalBounds().height / 2.0f) - m_menuText.getLocalBounds().top;
			m_menuText.setPosition(posX, posY);

			m_menuTextBackground.setOutlineColor(sf::Color(150, 150, 150));
			m_menuTextBackground.setOutlineThickness(1);
		}
		window.draw(m_menuTextBackground);
		window.draw(m_menuText);
	}

	// Setup navigation helper text
	sf::Font font = m_game->assets().getFont("Tech");
	sf::Text upText("UP: W", font, 25);
	sf::Text downText("DOWN: S", font, 25);
	sf::Text playText("PLAY: D", font, 25);
	sf::Text backText("QUIT: ESC", font, 25);

	// Calculate the x position for the first sf::Text's x position by
	// adding the width of all the local bounding boxes and a 20 pixel spacing between each.
	float xOffset = upText.getLocalBounds().width + downText.getLocalBounds().width + 
		playText.getLocalBounds().width + backText.getLocalBounds().width + (4 * 20);

	upText.setPosition(width() - xOffset, (float)window.getSize().y - 100);
	window.draw(upText);

	downText.setPosition(upText.getPosition().x + upText.getLocalBounds().width + 20, (float)window.getSize().y - 100);
	window.draw(downText);

	playText.setPosition(downText.getPosition().x + downText.getLocalBounds().width + 20, (float)window.getSize().y - 100);
	window.draw(playText);

	backText.setPosition(playText.getPosition().x + playText.getLocalBounds().width + 20, (float)window.getSize().y - 100);
	window.draw(backText);

	window.display();
}
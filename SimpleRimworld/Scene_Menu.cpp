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
}

void Scene_Menu::update()
{
	sRender();
}

void Scene_Menu::onEnd()
{
	
}

void Scene_Menu::sDoAction(const Action& action)
{
	
}

void Scene_Menu::sRender()
{
	auto& window = m_game->window();

	window.clear(sf::Color::Blue);

	m_menuText.setString(m_title);
	m_menuText.setFillColor(sf::Color::White);

	const float posX = window.getSize().x / 2.0f -
		m_menuText.getLocalBounds().width / 2.0f;
	const float posY = 10;
	m_menuText.setPosition(posX, posY);
	window.draw(m_menuText);

	for (int stringIndex = 0; stringIndex < m_menuStrings.size(); stringIndex++)
	{
		const float posX = 20;
		const float posY = 150 + (float)stringIndex * 100;
		m_menuText.setPosition(posX, posY);

		if (m_selectedMenuIndex == stringIndex)
		{
			m_menuText.setString(m_menuStrings[stringIndex] + " <-");
			m_menuText.setFillColor(sf::Color::Black);
		}
		else
		{
			m_menuText.setString(m_menuStrings[stringIndex]);
			m_menuText.setFillColor(sf::Color::White);
		}

		window.draw(m_menuText);
	}

	sf::Text upText("UP: W", m_game->assets().getFont("Tech"), 25);
	upText.setPosition(10, (float)window.getSize().y - 100);
	window.draw(upText);

	sf::Text downText("DOWN: S", m_game->assets().getFont("Tech"), 25);
	downText.setPosition(upText.getPosition().x + upText.getLocalBounds().width + 20, (float)window.getSize().y - 100);
	window.draw(downText);

	sf::Text playText("PLAY: D", m_game->assets().getFont("Tech"), 25);
	playText.setPosition(downText.getPosition().x + downText.getLocalBounds().width + 20, (float)window.getSize().y - 100);
	window.draw(playText);

	sf::Text backText("BACK: ESC", m_game->assets().getFont("Tech"), 25);
	backText.setPosition(playText.getPosition().x + playText.getLocalBounds().width + 20, (float)window.getSize().y - 100);
	window.draw(backText);

	window.display();
}
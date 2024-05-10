#include "GameEngine.h"
#include "Scene_Home_Map.h"
#include "Scene_Menu.h"

#include <fstream>
#include <iostream>

GameEngine::GameEngine(const std::string& path)
{
	init(path);
}

void GameEngine::init(const std::string& path)
{
	m_assets.loadFromFile(path);

	std::ifstream file("config.txt");
	std::string str;
	int wWidth = 0, wHeight = 0;

	while (file.good())
	{
		file >> str;
		if (str == "Window")
		{
			file >> wWidth >> wHeight >> m_fps;
		}
	}

	m_window.create(sf::VideoMode(wWidth, wHeight), "Game Dev Practice: Rimworld", sf::Style::Titlebar | sf::Style::Close);
	m_window.setFramerateLimit(m_fps);

	ImGui::SFML::Init(m_window);

	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_sceneMap[m_currentScene];
}

bool GameEngine::isRunning()
{
	return m_running && m_window.isOpen();
}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

void GameEngine::run()
{
	while (isRunning())
	{
		update();
		ImGui::SFML::Update(m_window, m_deltaClock.restart());
	}
}

void GameEngine::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(m_window, event);

		if (event.type == sf::Event::Closed)
		{
			quit();
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
			{ continue; }

			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

			currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}

		auto mousePos = sf::Mouse::getPosition(m_window);
		Vec2 pos((float)mousePos.x, (float)mousePos.y);
		if (event.type == sf::Event::MouseButtonPressed)
		{
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left: { currentScene()->doAction(Action("LEFT_CLICK", "START", pos)); break; }
			case sf::Mouse::Middle: { currentScene()->doAction(Action("MIDDLE_CLICK", "START", pos)); break; }
			case sf::Mouse::Right: { currentScene()->doAction(Action("RIGHT_CLICK", "START", pos)); break; }
			default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left: { currentScene()->doAction(Action("LEFT_CLICK", "END", pos)); break; }
			case sf::Mouse::Middle: { currentScene()->doAction(Action("MIDDLE_CLICK", "END", pos)); break; }
			case sf::Mouse::Right: { currentScene()->doAction(Action("RIGHT_CLICK", "END", pos)); break; }
			default: break;
			}
		}

		if (event.type == sf::Event::MouseMoved)
		{
			currentScene()->doAction(Action("MOUSE_MOVE", Vec2((float)event.mouseMove.x, (float)event.mouseMove.y)));
		}
	}
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	if (scene)
	{
		m_sceneMap[sceneName] = scene;
		
	}
	else
	{
		if (m_sceneMap.find(sceneName) == m_sceneMap.end())
		{
			std::cerr << "Warning: Scene does not exist: " << sceneName << std::endl;
			return;
		}
	}

	if (endCurrentScene)
	{
		m_sceneMap.erase(m_sceneMap.find(m_currentScene));
	}
	m_currentScene = sceneName;
}

void GameEngine::quit()
{
	m_running = false;
}

const Assets& GameEngine::assets() const
{
	return m_assets;
}

void GameEngine::update()
{
	if (!isRunning()) { return; }
	if (m_sceneMap.empty()) { return; }

	sUserInput();
	currentScene()->simulate(m_simulationSpeed);

	// Render system is seperated from the scene update so the game engine can
	// simulate a specified number of frames without rendering each frame of simulation.
	currentScene()->sRender();
	m_window.display();
}

void GameEngine::playSound(const std::string& soundName)
{
	if (soundName.find("Music") != std::string::npos)
	{
		if (!m_music.openFromFile(this->assets().getMusic(soundName)))
		{
			std::cerr << "Music failed to open.";
		}
		m_music.play();
	}
	else { m_assets.getSound(soundName).play(); }
}

void GameEngine::stopSound(const std::string& soundName)
{
	if (soundName.find("Music") != std::string::npos)
	{
		m_music.stop();
	}
	else { m_assets.getSound(soundName).stop(); }
}

const int GameEngine::getFps() const
{
	return m_fps;
}
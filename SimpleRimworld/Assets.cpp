#include "Assets.h"
#include <cassert>
#include <fstream>
#include <iostream>

Assets::Assets()
{

}

void Assets::loadFromFile(const std::string& path)
{
	std::ifstream file(path);
	std::string str;
	while (file.good())
	{
		file >> str;

		if (str == "Texture")
		{
			std::string name, path;
			file >> name >> path;
			addTexture(name, path);
		}
		else if (str == "Animation")
		{
			std::string name, texture;
			size_t frames, speed;
			file >> name >> texture >> frames >> speed;
			addAnimation(name, texture, frames, speed);
		}
		else if (str == "Font")
		{
			std::string name, path;
			file >> name >> path;
			addFont(name, path);
		}
		else if (str == "Sound")
		{
			std::string name, path;
			file >> name >> path;
			addSound(name, path);
		}
		else if (str == "Music")
		{
			std::string name, path;
			file >> name >> path;
			addMusic(name, path);
		}
		else
		{
			std::cerr << "Unknown Asset Type: " << str << std::endl;
		}
	}
}

void Assets::addTexture(const std::string& textureName, const std::string& path, bool smooth)
{
	m_textureMap[textureName] = sf::Texture();

	if (!m_textureMap[textureName].loadFromFile(path))
	{
		std::cerr << "Could not load texture file: " << path << std::endl;
	}
	else
	{
		m_textureMap[textureName].setSmooth(smooth);
	}
}

const sf::Texture& Assets::getTexture(const std::string& textureName) const
{
	assert(m_textureMap.find(textureName) != m_textureMap.end());
	return m_textureMap.at(textureName);
}

void Assets::addAnimation(const std::string& animationName, const std::string& textureName, size_t frameCount, size_t speed)
{
	m_animationMap[animationName] = Animation(animationName, getTexture(textureName), frameCount, speed);
}

const Animation& Assets::getAnimation(const std::string& animationName) const
{
	assert(m_animationMap.find(animationName) != m_animationMap.end());
	return m_animationMap.at(animationName);
}

void Assets::addFont(const std::string& fontName, const std::string& path)
{
	m_fontMap[fontName] = sf::Font();
	if (!m_fontMap[fontName].loadFromFile(path))
	{
		std::cerr << "Could not load font file: " << path << std::endl;
	}
}

const sf::Font& Assets::getFont(const std::string& fontName) const
{
	assert(m_fontMap.find(fontName) != m_fontMap.end());
	return m_fontMap.at(fontName);
}

void Assets::addSound(const std::string& soundName, const std::string& path)
{
	sf::SoundBuffer buffer;
	if (buffer.loadFromFile(path))
	{
		m_soundBufferMap[soundName] = buffer;
		m_soundMap[soundName] = sf::Sound(m_soundBufferMap[soundName]);
	}
	else { std::cerr << "Buffer couldn't load from file."; }
}

sf::Sound& Assets::getSound(const std::string& soundName)
{
	return m_soundMap.at(soundName);
}

void Assets::addMusic(const std::string& musicName, const std::string& path)
{
	m_musicMap[musicName] = path;
}

const std::string& Assets::getMusic(const std::string& musicName) const
{
	return m_musicMap.at(musicName);
}

const std::map<std::string, sf::Texture>& Assets::getTextures() const
{
	return m_textureMap;
}

const std::map<std::string, Animation>& Assets::getAnimations() const
{
	return m_animationMap;
}

const std::map<std::string, sf::Sound>& Assets::getSounds() const
{
	return m_soundMap;
}

const std::map<std::string, std::string>& Assets::getMusic() const
{
	return m_musicMap;
}
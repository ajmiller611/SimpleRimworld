#include "Assets.h"
#include <cassert>
#include <fstream>
#include <sstream>
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
		else if (str == "Tilesheet")
		{
			std::string name, path, line;
			std::vector<std::string> tileNames;
			file >> name >> path;

			// In assets.txt, all the names of each tile is defined on a single line. Since the number
			// of tiles a tilesheet has can vary, we can't read in a single name at a time. Instead,
			// get the whole line as a string and process that string.
			std::getline(file, line);

			// Get rid of any spaces in the beginning of the string until we find the first non-space character.
			for (auto iter = line.begin(); iter < line.end(); ++iter)
			{
				if (*(iter) == ' ')
				{
					line.erase(iter);
					++iter;
				}
				else { break; }
			}

			// Seperate the string into substrings by a space character seperator.
			std::stringstream ss(line);
			while (std::getline(ss, str, ' '))
			{
				tileNames.push_back(str);
			}

			// Send the tilesheet file and the names to be processed into individual tiles.
			processTilesheet(name, path, tileNames);
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

bool Assets::isTileEmpty(const sf::Image& tileImage, Vec2& tileSize)
{
	for (int x = 0; x < tileSize.x; ++x)
	{
		for (int y = 0; y < tileSize.y; ++y)
		{
			if (tileImage.getPixel(x, y).a != 0)
			{
				return false;
			}
		}
	}
	return true;
}

void Assets::processTilesheet(const std::string& tilesheetName, const std::string& path, std::vector<std::string>& tileNames)
{
	sf::Image tilesheet;
	if (!tilesheet.loadFromFile(path))
	{
		std::cerr << "Could not load tilesheet: " << path << std::endl;
	}
	else
	{
		const int numberOfRows = (int)(tilesheet.getSize().y / m_tileSize.y);
		const int numberOfColumns = (int)(tilesheet.getSize().x / m_tileSize.x);

		// Initialize the tile object
		sf::Image tile;
		tile.create((unsigned int)m_tileSize.x, (unsigned int)m_tileSize.y);

		int tileNameIndex = 0;
		for (size_t c = 0; c < numberOfColumns; ++c)
		{
			for (size_t r = 0; r < numberOfRows; ++r)
			{
				// Create a rect at the position the tile exists on the tilesheet
				sf::IntRect tileRect((int)(c * m_tileSize.x), (int)(r * m_tileSize.y), (int)m_tileSize.x, (int)m_tileSize.y);

				// Copy the pixels in the subregion defined by the sf::IntRect.
				// SFML documentation says this is a slow pixel copy so will look to optimze in the future.
				// Since tilesheets are not always symmetrical with their grid, that can leave empty space in the tilesheet.
				// sf::Image has a function to get a pixel at a position so we are using that function to ignore empty space.
				// For this reason, sf::Image is used over sf::Texture.
				tile.copy(tilesheet, 0, 0, tileRect);
				
				if (!isTileEmpty(tile, m_tileSize))
				{
					// Create a texture from the sf::Image to create a tile usable by the animation system.
					sf::Texture texTile;
					texTile.loadFromImage(tile);
					m_textureMap[tileNames[tileNameIndex]] = texTile;
					++tileNameIndex;
				}
			}
		}
	}
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
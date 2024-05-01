#include "Assets.h"
#include "MemoryMapping.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>

Assets::Assets()
{

}

void Assets::loadFromFile(const std::string& path)
{
	MemoryMapping mm(path);
	char* fileData = mm.getData();
	std::stringstream fileContentStream(fileData);
	std::string line, token, identifier;
	std::vector<std::string> tempVector;

	std::stringstream lineStream;
	while (getline(fileContentStream, line))
	{
		// proccess each line
		lineStream.str(line);
		while (lineStream >> token)
		{
			if (token == "Tilesheet" ||
				token == "Texture" ||
				token == "Animation" ||
				token == "Font")
			{
				identifier = token;
				continue;
			}
			tempVector.push_back(token);
		}

		// clear the stream to be able to reuse it
		lineStream.clear();
		
		// Since the asset.txt file has a specification for how it it structured, the order of the data stays the same.
		// This means we can specify the index and always get the correct data. E.g. The identifier for what kind of
		// assets that line represents is always the first string.

			 if (identifier == "Tilesheet") { processTilesheet(tempVector[0], tempVector[1], tempVector); }
		else if (identifier == "Texture")	{ addTexture(tempVector[0], tempVector[1]); }
		else if (identifier == "Animation")
		{
			// std::istringstream can be used to store the input strings into a different data type
			std::istringstream iss(tempVector[2]);
			size_t frameCount, interval;
			iss >> frameCount;
			iss.str(tempVector[3]);
			iss >> interval;
			addAnimation(tempVector[0], tempVector[1], frameCount, interval);
			iss.clear();
		}
		else if (identifier == "Font")		{ addFont(tempVector[0], tempVector[1]); }

		// clear vector for next line's data
		tempVector.clear();
	}

	// clean up for memory mapping
	mm.close();
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

		int tileNameIndex = 2;
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
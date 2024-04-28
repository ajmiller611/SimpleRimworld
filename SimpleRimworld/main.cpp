#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include "GameEngine.h"

int main()
{
	GameEngine g("assets.txt");
	g.run();
}
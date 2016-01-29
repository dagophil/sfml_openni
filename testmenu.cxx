#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <string>

#include <SFML/Graphics.hpp>

#include "menu_overlay.hxx"
#include "utility.hxx"

int main(int argc, char** argv)
{
    using namespace std;

    // Window width and height.
    size_t const WIDTH = 800;
    size_t const HEIGHT = 600;

    // Load the default font.
    sf::Font font;
    if (!font.LoadFromFile("fonts/opensans/OpenSans-Regular.ttf"))
        throw runtime_error("Could not load font.");

//    sf::Image overlay_texture(WIDTH, HEIGHT);
//    sf::Sprite overlay_sprite(overlay_texture);
    kin::MenuOverlay overlay("", WIDTH, HEIGHT);

    // Create window.
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Test menu");
    while (window.IsOpened())
    {
        // Handle window events.
        sf::Event event;
        while (window.GetEvent(event))
        {
            if (event.Type == sf::Event::Closed)
                window.Close();
        }



        window.Clear();


        // TODO: Draw the menu.
        overlay.draw(window, font);
//        overlay.draw(overlay_sprite);
//        window.Draw(overlay_sprite);

        window.Display();
    }
}

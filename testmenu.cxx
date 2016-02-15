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
    FPS fps_measure;
    while (window.IsOpened())
    {
        int mouse_x;
        int mouse_y;

        // Handle window events.
        sf::Event event;
        while (window.GetEvent(event))
        {
            if (event.Type == sf::Event::Closed)
                window.Close();
            else if (event.Type == sf::Event::MouseMoved)
            {
                mouse_x = event.MouseMove.X;
                mouse_y = event.MouseMove.Y;
            }
        }

        // Compute the fps.
        auto fps = fps_measure.update();
        auto elapsed_time = fps_measure.elapsed_time();

        // Update the menu.
        overlay.update(elapsed_time, mouse_x, mouse_y);

        window.Clear();


        // TODO: Draw the menu.
        overlay.draw(window, font);
//        overlay.draw(overlay_sprite);
//        window.Draw(overlay_sprite);

        window.Display();
    }
}

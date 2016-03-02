#include <iostream>

#include <SFML/Graphics.hpp>

#include "options.hxx"
#include "utility.hxx"
#include "hdm.hxx"

int main(int argc, char** argv)
{
    using namespace std;

    // Window width and height.
    size_t const WIDTH = 800;
    size_t const HEIGHT = 600;

    // Load the default font.
    kin::opts.load_default_font("fonts/opensans/OpenSans-Regular.ttf");

    // Create the game class.
    kin::HDMGame game(WIDTH, HEIGHT);

    // Create the window.
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Hau den Maulwurf");
    window.ShowMouseCursor(false);
    FPS fps_measure;
    while (window.IsOpened())
    {
        // Handle window events.
        sf::Event event;
        while (window.GetEvent(event))
        {
            if (event.Type == sf::Event::Closed)
                window.Close();
        }

        // Process the input.
        sf::Input const & input = window.GetInput();
        int mouse_x = input.GetMouseX();
        int mouse_y = input.GetMouseY();
        game.hover(mouse_x, mouse_y);

        // Update the widgets.
        auto fps = fps_measure.update();
        auto elapsed_time = fps_measure.elapsed_time();
        game.update(elapsed_time);

        // Draw everything.
        window.Clear();
        game.render(window);
        window.Display();
    }
}

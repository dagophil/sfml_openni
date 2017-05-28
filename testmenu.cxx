#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <string>

#include <SFML/Graphics.hpp>

#include "menu_overlay.hxx"
#include "utility.hxx"
#include "widgets.hxx"
#include "options.hxx"

int main(int argc, char** argv)
{
    using namespace std;
    using namespace kin;

    // Read the xml file from command line.
    if (argc != 2)
    {
        throw runtime_error("Wrong number of arguments.");
    }
    string xml_filename = argv[1];

    bool FULLSCREEN = false;

    // Window width and height.
    size_t WIDTH = 800;
    size_t HEIGHT = 600;
    if (FULLSCREEN)
    {
        auto mode = sf::VideoMode::getDesktopMode();
        WIDTH = mode.width;
        HEIGHT = mode.height;
    }

    // Load the default font.
    opts.load_default_font("fonts/opensans/OpenSans-Regular.ttf");

    // Create the mouse widget.
    opts.mouse_ = make_shared<AnimatedWidget>("animations/hand_load_2s.pf", 999);
    opts.mouse_->overwrite_render_rectangle({0, 0, 75, 75});
    opts.mouse_->hoverable_ = false;
    opts.mouse_->stop();
    opts.mouse_->repeatable_ = false;

    // Create the menu overlay.
    MenuOverlay overlay(xml_filename, WIDTH, HEIGHT);

    // Create the callback for the menu item clicks.
    string call_command;
    bool clicked_item = false;
    overlay.handle_menu_item_click_ = [&](string const & s)
    {
        call_command = s;
        clicked_item = true;
    };

    do
    {
        // Reset the call command.
        call_command = "";
        clicked_item = false;

        // Create the window.
        int mouse_x = -1;
        int mouse_y = -1;
        auto style = sf::Style::Close;
        if (FULLSCREEN)
            style = sf::Style::Fullscreen;
        sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Kinect menu", style);
        window.setMouseCursorVisible(false);
        FPS fps_measure(1.0f);
        overlay.handle_close_ = [&]()
        {
            window.close();
        };
        while (window.isOpen())
        {
            // Handle window events.
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                else if (event.type == sf::Event::MouseMoved)
                {
                    mouse_x = event.mouseMove.x;
                    mouse_y = event.mouseMove.y;
                }
            }

            // Process the input.
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
            overlay.hover(mouse_x, mouse_y);

            // Check if a menu item was clicked.
            if (clicked_item)
                window.close();

            // Compute the fps.
            auto fps = fps_measure.update();
            auto elapsed_time = fps_measure.elapsed_time();

            // Update the menu.
            overlay.update(elapsed_time);

            window.clear();

            // Draw the menu.
            overlay.render(window);

            window.display();
        }

        // Start the next command.
        overlay.hide_mouse();
        if (call_command.size() > 0)
        {
            std::cout << "Starting: " << call_command << std::endl;
            auto ret = system(call_command.c_str());
            std::cout << "Returned with value: " << ret << std::endl;
        }
    } while (call_command.size() > 0);


}

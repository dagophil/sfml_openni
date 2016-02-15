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

    // Read the xml file from command line.
    if (argc != 2)
    {
        throw runtime_error("Wrong number of arguments.");
    }
    string xml_filename = argv[1];

    // Window width and height.
    size_t const WIDTH = 800;
    size_t const HEIGHT = 600;

    // Load the default font.
    sf::Font font;
    if (!font.LoadFromFile("fonts/opensans/OpenSans-Regular.ttf"))
        throw runtime_error("Could not load font.");

    // Create the menu overlay.
    kin::MenuOverlay overlay(xml_filename, WIDTH, HEIGHT);

    // The next call command.
    std::string call_command;
    do
    {
        call_command = "";

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

            // Update the menu (hover, etc...) and get the command that shall be executed.
            auto call_id = overlay.update(elapsed_time, mouse_x, mouse_y);
            if (call_id >= 0)
            {
                call_command = overlay.get_call_command(call_id);

                // There is a command, so close the window, run the command, and reopen the window.
                window.Close();
                break;
            }

            window.Clear();

            // Draw the menu.
            overlay.draw(window, font);

            window.Display();
        }

        // Start the next command.
        if (call_command.size() > 0)
        {
            std::cout << "Starting: " << call_command << std::endl;
            auto ret = system(call_command.c_str());
            std::cout << "Returned with value: " << ret << std::endl;
        }
    } while (call_command.size() > 0);


}

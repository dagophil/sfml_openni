#include <iostream>

#include <SFML/Graphics.hpp>

#include "options.hxx"
#include "utility.hxx"
#include "hdm.hxx"
#include "events.hxx"
#include "widgets.hxx"

int main(int argc, char** argv)
{
    using namespace std;
    using namespace kin;

    // Window width and height.
    size_t const WIDTH = 800;
    size_t const HEIGHT = 600;

    // Load the default font.
    opts.load_default_font("fonts/opensans/OpenSans-Regular.ttf");

    // Create the mouse widget.
    opts.mouse_ = std::make_shared<AnimatedWidget>(
                "animations/hand_load_2s.pf",
                WIDTH, HEIGHT,
                75, 75,
                999
    );
    opts.mouse_->hoverable_ = false;
    opts.mouse_->stop();
    opts.mouse_->repeatable_ = false;

    // Create the window.
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Whac a Mole");
    window.ShowMouseCursor(false);
    FPS fps_measure;

    // Create the game class.
    HDMGame game(WIDTH, HEIGHT);
    game.handle_close_ = [&](){
        window.Close();
    };
    game.add_widget(opts.mouse_);

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

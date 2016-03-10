#include <iostream>

#include <SFML/Graphics.hpp>

#include "options.hxx"
#include "utility.hxx"
#include "hdm.hxx"
#include "events.hxx"
#include "widgets.hxx"
#include "sound_controller.hxx"

int main(int argc, char** argv)
{
    using namespace std;
    using namespace kin;

    bool FULLSCREEN = true;

    // Window width and height.
    size_t WIDTH = 800;
    size_t HEIGHT = 600;
    if (FULLSCREEN)
    {
        auto mode = sf::VideoMode::GetDesktopMode();
        WIDTH = mode.Width;
        HEIGHT = mode.Height;
    }

    // Load the default font.
    opts.load_default_font("fonts/opensans/OpenSans-Regular.ttf");

    // Create the mouse widget.
    opts.mouse_ = std::make_shared<AnimatedWidget>("animations/hand_load_2s.pf", 999);
    opts.mouse_->overwrite_render_rectangle({0, 0, 75, 75});
    opts.mouse_->hoverable_ = false;
    opts.mouse_->stop();
    opts.mouse_->repeatable_ = false;

    // Create the window.
    auto style = sf::Style::Close;
    if (FULLSCREEN)
        style = sf::Style::Fullscreen;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Whac a Mole", style);
    window.ShowMouseCursor(false);
    FPS fps_measure;

    // Create the game class.
    HDMGame game;
    game.handle_close_ = [&](){
        window.Close();
    };
    game.add_widget(opts.mouse_);

    // Create the sound controller.
    auto sound_controller = std::make_shared<HDMSoundController>();
    event_manager.register_listener(sound_controller);

    while (window.IsOpened())
    {
        // Handle window events.
        sf::Event event;
        opts.mouse_clicked_ = false;
        while (window.GetEvent(event))
        {
            if (event.Type == sf::Event::Closed)
            {
                window.Close();
            }
            else if (event.Type == sf::Event::KeyPressed)
            {
                if (event.Key.Code == sf::Key::Escape)
                    window.Close();
            }
            else if (event.Type == sf::Event::MouseButtonPressed)
            {
                if (event.MouseButton.Button == sf::Mouse::Button::Left)
                    opts.mouse_clicked_ = true;
            }
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

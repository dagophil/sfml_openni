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
        auto mode = sf::VideoMode::getDesktopMode();
        WIDTH = mode.width;
        HEIGHT = mode.height;
    }

    // Load the default font.
    opts.load_default_font("fonts/opensans/OpenSans-Regular.ttf");
    TextWidget::set_default_font(opts.default_font());

    // Create the mouse widget.
    opts.mouse_ = std::make_shared<AnimatedWidget>("animations/hand_load_2s.pf", 999);
    opts.mouse_->overwrite_render_rectangle({0, 0, 75, 75});
    opts.mouse_->hoverable_ = false;
    opts.mouse_->stop();
    opts.mouse_->repeatable_ = false;

    // Set sreen Height and width in options.
    opts.screen_height_ = HEIGHT;
    opts.screen_width_ = WIDTH;

    // Create the window.
    auto style = sf::Style::Close;
    if (FULLSCREEN)
        style = sf::Style::Fullscreen;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Whac a Mole", style);
    window.setMouseCursorVisible(false);
    FPS fps_measure(1.0f);

    // Create the game class.
    HDMGame game;
    game.handle_close_ = [&](){
        window.close();
    };
    game.add_widget(opts.mouse_);

    // Create the sound controller.
    auto sound_controller = std::make_shared<HDMSoundController>();
    event_manager.register_listener(sound_controller);



    while (window.isOpen())
    {
        // Handle window events.
        sf::Event event;
        opts.mouse_clicked_ = false;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Button::Left)
                    opts.mouse_clicked_ = true;
            }
        }

        // Process the input.
        auto mouse_pos = sf::Mouse::getPosition(window);
        game.hover(mouse_pos.x, mouse_pos.y);

        // Update the widgets.
        auto fps = fps_measure.update();
        auto elapsed_time = fps_measure.elapsed_time();
        game.update(elapsed_time);

        // Draw everything.
        window.clear();
        game.render(window);
        window.display();
    }
}

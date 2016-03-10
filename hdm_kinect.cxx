#include <iostream>
#include <functional>

#include <SFML/Graphics.hpp>

#include "options.hxx"
#include "utility.hxx"
#include "hdm.hxx"
#include "events.hxx"
#include "widgets.hxx"
#include "sound_controller.hxx"
#include "kinect.hxx"

class KinectClickListener : public kin::Listener
{
protected:
    void notify_impl(const kin::Event &event)
    {
        std::cout << "CLICK" << std::endl;
        kin::opts.mouse_clicked_ = true;
    }
};

int main(int argc, char** argv)
{
    using namespace std;
    using namespace kin;

    bool FULLSCREEN = false;

    // Window width and height.
    size_t WIDTH = 800;
    size_t HEIGHT = 600;
    if (FULLSCREEN)
    {
        auto mode = sf::VideoMode::GetDesktopMode();
        WIDTH = mode.Width;
        HEIGHT = mode.Height;
    }

    std::cout << "before all" << std::endl;

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

    // Create the kinect sensor.
    KinectSensor k;

    // Create a callback for the click event.
    auto click_listener = std::make_shared<KinectClickListener>();
    event_manager.register_listener(click_listener);

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
//        sf::Input const & input = window.GetInput();
//        int mouse_x = input.GetMouseX();
//        int mouse_y = input.GetMouseY();
//        game.hover(mouse_x, mouse_y);

        // Update the kinect data.
        float mouse_x;
        float mouse_y;
        auto updates = k.update();// Get the hand positions.
        auto hand_left = k.hand_left();
        auto hand_right = k.hand_right();
        bool hand_left_visible = k.hand_left_visible() && hand_left.Z >= 0.0;
        bool hand_right_visible = k.hand_right_visible() && hand_right.Z >= 0.0;
        bool hand_visible = hand_left_visible || hand_right_visible;
        if (hand_visible)
        {
            bool both_visible = hand_left_visible && hand_right_visible;
            if (!hand_right_visible || (both_visible && hand_left.Z > hand_right.Z))
            {
                mouse_x = (hand_left.X + 1.5) * WIDTH / 1.75;
                mouse_y = (hand_left.Y - 0.7) * HEIGHT / 1.5;
            }
            else
            {
                mouse_x = (hand_right.X + 0.33) * WIDTH / 1.75;
                mouse_y = (hand_right.Y - 0.7) * HEIGHT / 1.5;
            }

            // Check that the mouse is actually visible.
            if (mouse_x < 0 || mouse_x >= WIDTH || mouse_y < 0 || mouse_y >= HEIGHT)
                hand_visible = false;
        }
        if (!hand_visible)
        {
            mouse_x = -1;
            mouse_y = -1;
        }
        if (mouse_x == -1 || mouse_y == -1)
            ;
        else
            game.hover(mouse_x, mouse_y);

        // Update the widgets.
        auto fps = fps_measure.update();
        auto elapsed_time = fps_measure.elapsed_time();
        game.update(elapsed_time);

        // Draw everything.
        window.Clear();
        game.render(window);
        window.Display();

        opts.mouse_clicked_ = false;
    }
}

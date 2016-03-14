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

    // Create the kinect sensor.
    KinectSensor k;
    bool clicked_left = false;
    bool clicked_right = false;
    k.handle_click_left() = [&](){
        clicked_left = true;
    };
    k.handle_click_right() = [&](){
        clicked_right = true;
    };
    bool use_right = true;

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
        }

        // Update the kinect data.
        clicked_left = false;
        clicked_right = false;
        auto fps = fps_measure.update();
        auto elapsed_time = fps_measure.elapsed_time();
        auto updates = k.update(elapsed_time);

        // Get the hand positions.
        float mouse_x;
        float mouse_y;
        float mouse_z;
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
                use_right = false;
                mouse_x = hand_left.X * WIDTH;
                mouse_y = hand_left.Y * HEIGHT;
                mouse_z = (1.5 - hand_left.Z) * HEIGHT;
            }
            else
            {
                use_right = true;
                mouse_x = hand_right.X * WIDTH;
                mouse_y = hand_right.Y * HEIGHT;
                mouse_z = (1.5 - hand_right.Z) * HEIGHT;
            }

            // Check that the mouse is actually visible.
            if (mouse_x < 0 || mouse_x >= WIDTH || mouse_y < 0 || mouse_y >= HEIGHT)
                hand_visible = false;
        }
        if (!hand_visible)
        {
            mouse_x = -1;
            mouse_y = -1;
            mouse_z = -1;
        }

//        mouse_z = mouse_y;
        if (mouse_x != -1 && mouse_z != -1)
        {
            game.hover(mouse_x, mouse_z);

            // Find the currently hovered field.
            int fx = -1;
            int fy = -1;
            if (mouse_x < WIDTH/3.5)
                fx = 0;
            else if (mouse_x < 2.5*WIDTH/3.5)
                fx = 1;
            else
                fx = 2;
            if (mouse_z < HEIGHT/4.0)
                fy = 0;
            else if (mouse_z < 2*HEIGHT/4.0)
                fy = 1;
            else
                fy = 2;

            game.hover_field(fx, fy);
        }

        if (clicked_left && !use_right)
            opts.mouse_clicked_ = true;
        if (clicked_right && use_right)
            opts.mouse_clicked_ = true;

        // Update the widgets.
        game.update(elapsed_time);

        // Draw everything.
        window.Clear();
        game.render(window);
        window.Display();

        opts.mouse_clicked_ = false;
    }
}

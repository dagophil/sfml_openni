#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <string>

#include <SFML/Graphics.hpp>

#include "menu_overlay.hxx"
#include "utility.hxx"
#include "widgets.hxx"
#include "options.hxx"
#include "kinect.hxx"


class DrawOptions
{
public:
    DrawOptions(
            bool draw_depth = true,
            bool draw_fps = false,
            bool draw_users = true,
            bool draw_joints = true,
            bool draw_menu = false
    )   :
          draw_depth_(draw_depth),
          draw_fps_(draw_fps),
          draw_users_(draw_users),
          draw_joints_(draw_joints),
          draw_menu_(draw_menu)
    {}

    bool draw_depth() const
    {
        if (draw_menu_)
            return false;
        else
            return draw_depth_;
    }
    void set_draw_depth(bool draw_depth)
    {
        draw_depth_ = draw_depth;
        if (draw_depth)
            draw_menu_ = false;
    }

    bool draw_fps() const
    {
        return draw_fps_;
    }
    void set_draw_fps(bool draw_fps)
    {
        draw_fps_ = draw_fps;
    }

    bool draw_users() const
    {
        if (draw_menu_)
            return false;
        else
            return draw_users_;
    }
    void set_draw_users(bool draw_users)
    {
        draw_users_ = draw_users;
        if (draw_users)
            draw_menu_ = false;
    }

    bool draw_joints() const
    {
        if (draw_menu_)
            return false;
        else
            return draw_joints_;
    }
    bool set_draw_joints(bool draw_joints)
    {
        draw_joints_ = draw_joints;
        if (draw_joints)
            draw_menu_ = false;
    }

    bool draw_menu() const
    {
        return draw_menu_;
    }
    void set_draw_menu(bool draw_menu)
    {
        draw_menu_ = draw_menu;
    }

private:
    bool draw_depth_;
    bool draw_fps_;
    bool draw_users_;
    bool draw_joints_;
    bool draw_menu_;
};


int main(int argc, char** argv)
{
    using namespace std;
    using namespace kin;

    // Read the xml file from command line.
    if (argc != 2)
        throw runtime_error("Wrong number of arguments.");
    string xml_filename = argv[1];

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

    // The drawing options.
    DrawOptions draw_opts;

    // Create the kinect sensor.
    kin::KinectSensor k;
    double const SCALE_X = WIDTH / (double) k.x_res();
    double const SCALE_Y = HEIGHT / (double) k.y_res();

    // Load the default font.
    opts.load_default_font("fonts/opensans/OpenSans-Regular.ttf");

    // Create the mouse widget.
    opts.mouse_ = make_shared<AnimatedWidget>("animations/hand_load_2s.pf", 999);
    opts.mouse_->overwrite_render_rectangle({0, 0, 75, 75});
    opts.mouse_->hoverable_ = false;
    opts.mouse_->stop();
    opts.mouse_->repeatable_ = false;

    // Create the menu overlay.
    kin::MenuOverlay overlay(xml_filename, WIDTH, HEIGHT);

    // Create the callback for the menu item clicks.
    string call_command;
    bool clicked_item = false;
    overlay.handle_menu_item_click_ = [&](std::string const & s)
    {
        call_command = s;
        clicked_item = true;
    };

    // Measure the FPS.
    FPS fps_measure;
    sf::String fps_text;
    fps_text.SetFont(opts.default_font());
    fps_text.SetSize(16);

    // Create the sprite for the depth RGBA.
    Array2D<sf::Color> depth_rgba(k.x_res(), k.y_res());
    sf::Image depth_texture(k.x_res(), k.y_res());
    sf::Sprite depth_sprite(depth_texture);
    depth_sprite.SetScale(SCALE_X, SCALE_Y);

    // Create the sprite for the user RGBA.
    Array2D<sf::Color> user_rgba(k.x_res(), k.y_res());
    sf::Image user_texture(k.x_res(), k.y_res());
    sf::Sprite user_sprite(user_texture);
    user_sprite.SetScale(SCALE_X, SCALE_Y);

    // Create the texture for the user joints.
    sf::Image joint_texture(3, 3, sf::Color(255, 255, 255, 255));
    std::vector<sf::Sprite> joint_sprites;

    // Window open/close loop.
    do
    {
        // Reset the call command.
        call_command = "";
        clicked_item = false;

        // Create the window and go into the main loop.
        float mouse_x = 0;
        float mouse_y = 0;
        auto style = sf::Style::Close;
        if (FULLSCREEN)
            style = sf::Style::Fullscreen;
        sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Kinect menu", style);
        window.ShowMouseCursor(false);
        overlay.handle_close_ = [&]()
        {
            window.Close();
        };
        while (window.IsOpened())
        {
            ///////////////////////////////////////////////
            //             Process the input             //
            ///////////////////////////////////////////////

            // Handle the window events.
            sf::Event event;
            while (window.GetEvent(event))
            {
                if (event.Type == sf::Event::Closed)
                    window.Close();
                if (event.Type == sf::Event::TextEntered)
                {
                    if (tolower(event.Text.Unicode) == 'u')
                        draw_opts.set_draw_users(!draw_opts.draw_users());
                    if (tolower(event.Text.Unicode) == 'f')
                        draw_opts.set_draw_fps(!draw_opts.draw_fps());
                    if (tolower(event.Text.Unicode) == 'j')
                        draw_opts.set_draw_joints(!draw_opts.draw_joints());
                    if (tolower(event.Text.Unicode) == 'm')
                        draw_opts.set_draw_menu(!draw_opts.draw_menu());
                }
            }

            // Process the input.
            sf::Input const & input = window.GetInput();
            if (input.IsKeyDown(sf::Key::Escape))
                window.Close();


            ///////////////////////////////////////////////
            //             Update everything             //
            ///////////////////////////////////////////////

            // Compute the fps.
            auto fps = fps_measure.update();
            auto elapsed_time = fps_measure.elapsed_time();

            // Update the kinect data.
            auto updates = k.update();
            if (updates.depth_)
            {
                depth_to_rgba(k.depth_data(), k.z_res(), depth_rgba);
                depth_texture.LoadFromPixels(k.x_res(), k.y_res(), uint8_ptr(depth_rgba));
            }
            if (updates.user_)
            {
                user_to_rgba(k.user_data(), user_rgba);
                user_texture.LoadFromPixels(k.x_res(), k.y_res(), uint8_ptr(user_rgba));
                joint_sprites.clear();
                for (auto const & user : k.users())
                {
                    for (auto const & p : user.joints_)
                    {
                        joint_sprites.emplace_back(joint_texture);
                        joint_sprites.back().SetPosition(SCALE_X*p.second.proj_position_.X,
                                                         SCALE_Y*p.second.proj_position_.Y);
                    }
                }
            }

            // Get the hand positions.
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
            if (hand_visible)
            {
//                cursor_sprite.SetX(mouse_x);
//                cursor_sprite.SetY(mouse_y);
            }
            else
            {
                mouse_x = -1;
                mouse_y = -1;
            }

            // Update the menu.
            if (draw_opts.draw_menu())
            {
                if (mouse_x == -1 || mouse_y == -1)
                    overlay.hide_mouse();
                else
                    overlay.hover(mouse_x, mouse_y);
                overlay.update(elapsed_time);
                if (clicked_item)
                    window.Close();
            }


            ///////////////////////////////////////////////
            //              Draw everything              //
            ///////////////////////////////////////////////

            // Clear to black.
            window.Clear();

            // Draw the depth map.
            if (draw_opts.draw_depth())
            {
                window.Draw(depth_sprite);
            }

            // Draw the users.
            if (draw_opts.draw_users())
            {
                window.Draw(user_sprite);
            }

            // Draw the joints.
            if (draw_opts.draw_joints())
            {
                for (auto const & spr : joint_sprites)
                {
                    window.Draw(spr);
                }
            }

            // Draw the menu.
            if (draw_opts.draw_menu())
                overlay.render(window);

            // Draw the FPS text.
            if (draw_opts.draw_fps())
            {
                fps_text.SetText("FPS: " + to_string(fps));
                window.Draw(fps_text);
            }

            // Show the drawed stuff on the window.
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

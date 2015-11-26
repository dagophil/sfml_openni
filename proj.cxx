#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <sstream>

#include <SFML/Graphics.hpp>

#include "kinect.hxx"
#include "utility.hxx"



int main(int argc, char** argv)
{
    using namespace std;

    typedef std::map<XnSkeletonJoint, kin::JointInfo> JointMap;

    // Window width and height.
    size_t WIDTH = 800;
    size_t HEIGHT = 600;
    bool DRAW_FPS = false;
    bool DRAW_USERS = true;
    bool DRAW_JOINTS = true;

    // Create the kinect sensor.
    kin::KinectSensor k;
    double SCALE_X = WIDTH / (double) k.x_res();
    double SCALE_Y = HEIGHT / (double) k.y_res();

    // Load the default font.
    sf::Font font;
    if (!font.LoadFromFile("fonts/opensans/OpenSans-Regular.ttf"))
        throw runtime_error("Could not load font.");

    // Measure the FPS.
    FPS fps_measure;
    sf::String fps_text;
    fps_text.SetFont(font);
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

    // Create the window and go into the main loop.
//    auto const & modes = sf::VideoMode::getFullscreenModes();
//    sf::RenderWindow window(modes.front(), "Kinect menu", sf::Style::Fullscreen);
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Kinect menu");
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
                    DRAW_USERS = !DRAW_USERS;
                if (tolower(event.Text.Unicode) == 'f')
                    DRAW_FPS = !DRAW_FPS;
                if (tolower(event.Text.Unicode) == 'j')
                    DRAW_JOINTS = !DRAW_JOINTS;
            }
        }

        // Process the input.
        sf::Input const & input = window.GetInput();
        if (input.IsKeyDown(sf::Key::Escape))
        {
            window.Close();
            break;
        }


        ///////////////////////////////////////////////
        //             Update everything             //
        ///////////////////////////////////////////////

        // Compute the fps.
        float fps = fps_measure.update();
        float elapsed_time = fps_measure.elapsed_time();

        // Update the kinect data.
        kin::UpdateDetails updates = k.update();
        if (updates.depth_)
        {
            depth_to_rgba(k.depth_data(), k.z_res(), depth_rgba);
            depth_texture.LoadFromPixels(k.x_res(), k.y_res(), &depth_rgba.front().r);
        }
        if (updates.user_)
        {
            user_to_rgba(k.user_data(), user_rgba);
            user_texture.LoadFromPixels(k.x_res(), k.y_res(), &user_rgba.front().r);
            joint_sprites.clear();
            for (size_t i = 0; i < k.users().size(); ++i)
            {
                kin::User const & user = k.users()[i];
                for (JointMap::const_iterator it = user.joints_.begin(); it != user.joints_.end(); ++it)
                {
                    pair<XnSkeletonJoint, kin::JointInfo> const & p = *it;
                    joint_sprites.push_back(sf::Sprite(joint_texture));
                    joint_sprites.back().SetPosition(SCALE_X*p.second.proj_position_.X,
                                                     SCALE_Y*p.second.proj_position_.Y);
                }
            }
        }


        ///////////////////////////////////////////////
        //              Draw everything              //
        ///////////////////////////////////////////////

        // Clear to black.
        window.Clear();

        // Draw the depth map.
        window.Draw(depth_sprite);

        // Draw the users.
        if (DRAW_USERS)
        {
            window.Draw(user_sprite);
        }

        // Draw the FPS text.
        if (DRAW_FPS)
        {
            stringstream ss;
            ss << fps;

            fps_text.SetText("FPS: " + ss.str());
            window.Draw(fps_text);
        }

        // Draw the joints.
        if (DRAW_JOINTS)
        {
            for (size_t i = 0; i < joint_sprites.size(); ++i)
            {
                window.Draw(joint_sprites[i]);
            }
        }

        // Show the drawed stuff on the window.
        window.Display();
    }
}

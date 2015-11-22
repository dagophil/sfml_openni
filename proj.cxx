#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#include "kinect.hxx"
#include "utility.hxx"



int main(int argc, char** argv)
{
    using namespace std;

    typedef std::array<uint8_t, 4> RGBA;

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
    if (!font.loadFromFile("fonts/opensans/OpenSans-Regular.ttf"))
        throw runtime_error("Could not load font.");

    // Measure the FPS.
    FPS fps_measure;
    sf::Text fps_text;
    fps_text.setFont(font);
    fps_text.setCharacterSize(16);

    // Create the sprite for the depth RGBA.
    Array2D<RGBA> depth_rgba(k.x_res(), k.y_res());
    sf::Texture depth_texture;
    if (!depth_texture.create(k.x_res(), k.y_res()))
        throw runtime_error("Could not create depth texture.");
    sf::Sprite depth_sprite(depth_texture);
    depth_sprite.setScale(SCALE_X, SCALE_Y);

    // Create the sprite for the user RGBA.
    Array2D<RGBA> user_rgba(k.x_res(), k.y_res());
    sf::Texture user_texture;
    if (!user_texture.create(k.x_res(), k.y_res()))
        throw runtime_error("Could not create user texture.");
    sf::Sprite user_sprite(user_texture);
    user_sprite.setScale(SCALE_X, SCALE_Y);

    // Create the texture for the user joints.
    sf::Texture joint_texture;
    if (!joint_texture.create(3, 3))
        throw runtime_error("Could not create joint texture.");
    std::vector<RGBA> joint_rgba(9, {255, 255, 255, 255});
    joint_texture.update(&joint_rgba.front()[0]);
    std::vector<sf::Sprite> joint_sprites;

    // Create the window and go into the main loop.
//    auto const & modes = sf::VideoMode::getFullscreenModes();
//    sf::RenderWindow window(modes.front(), "Kinect menu", sf::Style::Fullscreen);
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Kinect menu");
    while (window.isOpen())
    {
        ///////////////////////////////////////////////
        //             Process the input             //
        ///////////////////////////////////////////////

        // Handle the window events.
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::TextEntered)
            {
                if (tolower(event.text.unicode) == 'u')
                    DRAW_USERS = !DRAW_USERS;
                if (tolower(event.text.unicode) == 'f')
                    DRAW_FPS = !DRAW_FPS;
                if (tolower(event.text.unicode) == 'j')
                    DRAW_JOINTS = !DRAW_JOINTS;
            }
        }

        // Process the input.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
            break;
        }


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
            depth_texture.update(&depth_rgba.front()[0]);
        }
        if (updates.user_)
        {
            user_to_rgba(k.user_data(), user_rgba);
            user_texture.update(&user_rgba.front()[0]);
            joint_sprites.clear();
            for (auto const & user : k.users())
            {
                for (auto const & p : user.joints_)
                {
                    joint_sprites.emplace_back(joint_texture);
                    joint_sprites.back().setPosition(SCALE_X*p.second.proj_position_.X,
                                                     SCALE_Y*p.second.proj_position_.Y);
                }
            }
        }


        ///////////////////////////////////////////////
        //              Draw everything              //
        ///////////////////////////////////////////////

        // Clear to black.
        window.clear();

        // Draw the depth map.
        window.draw(depth_sprite);

        // Draw the users.
        if (DRAW_USERS)
        {
            window.draw(user_sprite);
        }

        // Draw the FPS text.
        if (DRAW_FPS)
        {
            fps_text.setString("FPS: " + to_string(fps));
            window.draw(fps_text);
        }

        // Draw the joints.
        if (DRAW_JOINTS)
        {
            for (auto const & spr : joint_sprites)
            {
                window.draw(spr);
            }
        }

        // Show the drawed stuff on the window.
        window.display();
    }
}

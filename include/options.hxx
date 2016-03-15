#ifndef OPTIONS_HXX
#define OPTIONS_HXX

#include <stdexcept>
#include <random>

#include <SFML/Graphics.hpp>

#include "widgets.hxx"

namespace kin
{

/**
 * @brief Class that stores some options.
 */
class Globals
{
public:

    Globals()
        :
          mouse_clicked_(false),
          rand_engine_(std::random_device()()),
          highscore_pos_(-1),
          screen_width_(0),
          screen_height_(0),
          use_kinect_(false)
    {}

    /**
     * @brief Load and set the default font.
     */
    void load_default_font(std::string const & default_font)
    {
        if (!default_font_.LoadFromFile(default_font))
            throw std::runtime_error("Could not load default font.");
    }

    /**
     * @brief Return the default font.
     */
    sf::Font const & default_font() const
    {
        return default_font_;
    }

    std::shared_ptr<AnimatedWidget> mouse_; // the mouse widget
    bool mouse_clicked_; // whether a mouse was clicked in the current frame
    std::mt19937 rand_engine_; // the random engine
    int highscore_pos_; // the position of the new Highscore
    size_t screen_width_; // the screen height
    size_t screen_height_; // the screen width
    bool use_kinect_; // whether the kinect is used

private:

    sf::Font default_font_;

};

Globals opts; // the global options object

}

#endif

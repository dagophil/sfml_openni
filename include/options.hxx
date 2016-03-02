#ifndef OPTIONS_HXX
#define OPTIONS_HXX

#include <SFML/Graphics.hpp>

namespace kin
{

/**
 * @brief Class that stores some options.
 */
class Options
{
public:

    void load_default_font(std::string const & default_font)
    {
        if (!default_font_.LoadFromFile(default_font))
            throw std::runtime_error("Could not load default font.");
    }

    sf::Font const & default_font() const
    {
        return default_font_;
    }

private:

    sf::Font default_font_;

};

Options opts; // the global options object

}

#endif

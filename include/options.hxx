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
          rand_engine_(std::random_device()())
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

private:

    sf::Font default_font_;

};

Globals opts; // the global options object

template <typename T, typename W>
void attach_mouse_events(std::shared_ptr<T> m, std::shared_ptr<W> w)
{
    typedef typename W::DiffType DiffType;
    w->handle_mouse_enter_ = [m, w](DiffType x, DiffType y){
        m->restart();
    };
    w->handle_mouse_leave_ = [m, w](DiffType x, DiffType y){
        m->reset();
        m->stop();
    };
}

}

#endif

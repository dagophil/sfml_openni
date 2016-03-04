#ifndef SOUND_CONTROLLER_HXX
#define SOUND_CONTROLLER_HXX

#include <stdexcept>
#include <random>

#include <SFML/Audio.hpp>

#include "events.hxx"
#include "options.hxx"

namespace kin
{

class HDMSoundController : public Listener
{
public:

    /**
     * @brief Load the sound files.
     */
    HDMSoundController()
    {
        if (!wilhelm_buffer_.LoadFromFile("sounds/wilhelm_scream.ogg"))
            throw std::runtime_error("Could not load sound: Wilhelm scream.");
        wilhelm_sound_.SetBuffer(wilhelm_buffer_);
    }

protected:

    /**
     * @brief Play sounds to the events.
     */
    void notify_impl(Event const & event)
    {
        if (event.type_ == Event::MoleHit)
        {
            std::uniform_int_distribution<int> r(0, 9);
            int sound = r(opts.rand_engine_);
            if (sound == 0)
                wilhelm_sound_.Play();
        }
    }

private:

    sf::SoundBuffer wilhelm_buffer_; // buffer for the wilhelm scream
    sf::Sound wilhelm_sound_; // the wilhelm sound

};

}

#endif

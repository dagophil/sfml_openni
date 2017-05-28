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

    enum SoundEffects
    {
        WilhelmScream,
        Punch
    };

    struct Sound
    {
        sf::SoundBuffer buffer;
        sf::Sound sound;

        void play()
        {
            sound.play();
        }
    };

    /**
     * @brief Load the sound files.
     */
    HDMSoundController()
    {
        // Load the sounds.
        if (!sounds_[WilhelmScream].buffer.loadFromFile("sounds/wilhelm_scream.ogg"))
            throw std::runtime_error("Could not load sound: Wilhelm scream.");
        if (!sounds_[Punch].buffer.loadFromFile("sounds/punch.ogg"))
            throw std::runtime_error("Could not load sound: Punch.");


        // Assign the buffers to the sounds.
        for (auto & p : sounds_)
        {
            p.second.sound.setBuffer(p.second.buffer);
        }

        // Load the background music.
        if (!background_.openFromFile("sounds/joplin_maple_leaf_rag.ogg"))
            throw std::runtime_error("Could not load sound: Maple leaf rag.");
        background_.play();
        background_.setLoop(true);
        background_.setVolume(90);
    }

protected:

    /**
     * @brief Play sounds to the events.
     */
    void notify_impl(Event const & event)
    {
        if (event.type_ == Event::ToggleSound)
        {
            if (opts.sound_)
                background_.play();
            else
                background_.stop();
        }
        else if (event.type_ == Event::MoleHit)
        {
            if (opts.sound_)
            {
                std::uniform_int_distribution<int> r(0, 4);
                int sound = r(opts.rand_engine_);
                if (sound == 0)
                    sounds_[WilhelmScream].play();
                sounds_[Punch].play();
            }
        }
    }

private:

    std::map<SoundEffects, Sound> sounds_; // the sounds
    sf::Music background_; // the background music

};

}

#endif

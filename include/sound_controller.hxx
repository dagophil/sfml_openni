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

        void Play()
        {
            sound.Play();
        }
    };

    /**
     * @brief Load the sound files.
     */
    HDMSoundController()
    {
        // Load the sounds.
        if (!sounds_[WilhelmScream].buffer.LoadFromFile("sounds/wilhelm_scream.ogg"))
            throw std::runtime_error("Could not load sound: Wilhelm scream.");
        if (!sounds_[Punch].buffer.LoadFromFile("sounds/punch.ogg"))
            throw std::runtime_error("Could not load sound: Punch.");


        // Assign the buffers to the sounds.
        for (auto & p : sounds_)
        {
            p.second.sound.SetBuffer(p.second.buffer);
        }

        // Load the background music.
        if (!background_.OpenFromFile("sounds/joplin_maple_leaf_rag.ogg"))
            throw std::runtime_error("Could not load sound: Maple leaf rag.");
        background_.Play();
        background_.SetLoop(true);
        background_.SetVolume(90);
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
                background_.Play();
            else
                background_.Stop();
        }
        else if (event.type_ == Event::MoleHit)
        {
            if (opts.sound_)
            {
                std::uniform_int_distribution<int> r(0, 4);
                int sound = r(opts.rand_engine_);
                if (sound == 0)
                    sounds_[WilhelmScream].Play();
                sounds_[Punch].Play();
            }
        }
    }

private:

    std::map<SoundEffects, Sound> sounds_; // the sounds
    sf::Music background_; // the background music

};

}

#endif

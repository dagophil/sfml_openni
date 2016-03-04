#ifndef GAME_SCREEN_HXX
#define GAME_SCREEN_HXX

#include <random>

#include "../widgets.hxx"
#include "../events.hxx"
#include "../options.hxx"

namespace kin
{

class GameScreen :public Widget
{
public:

    typedef std::shared_ptr<AnimatedWidget> MolePointer;

    template <typename... Args>
    GameScreen(
            Args... args
    )
        :
          Widget(args...),
          rand_engine_(std::random_device()()),
          running_(false)
    {
        // Set all moles to "in".
        mole_out_.resize(9, false);
        mole_hit_.resize(9, false);

        auto height = rect_.GetHeight();
        auto width = rect_.GetWidth();
        auto sprites_top = 0.25 * height;
        auto sprites_height = 0.22 * height;
        auto sprite_distance_y = 0.25 * height;
        auto sprites_width = 1.314*sprites_height;
        auto sprite_distance_x = sprites_width + 0.03 * width;
        auto sprite_total_width = 2 * sprite_distance_x + sprites_width;
        auto sprites_left = (width - sprite_total_width) / 2;
        auto score_width = 0.8 * width;
        auto score_height = 0.24 * height;

        for (int i = 0; i < 9; ++i)
        {
            auto w = std::make_shared<AnimatedWidget>(
                    "animations/mole.pf",
                    sprites_left+(i%3)*sprite_distance_x,sprites_top+(i/3)*sprite_distance_y,
                    sprites_width,sprites_height,
                    5);
            moles_.push_back(w);
            add_widget(w);
            w->repeatable_ = false;
            w->freeze_finish_ = true;
            w->stop();
        }

        auto s = std::make_shared<ImageWidget>(
                    "images/scoreboard.png",
                    (width-score_width)/2, 0,
                    score_width, score_height,
                    1);
        // TODO: Show the scoreboard.
//        add_widget(s);

        // Create the widgets for the 3-2-1 counter.
        auto h = height/4;
        auto w = 1.0 * h;
        auto timer = std::make_shared<AnimatedWidget>(
                    "animations/timer.pf",
                    (width-w)/2, (height-h)/2,
                    w, h,
                    50
        );
        timer->hoverable_ = false;
        timer->repeatable_ = false;
        timer->freeze_finish_ = true;
        add_widget(timer);
        h = height/3.5;
        w = 1.659 * h;
        auto go = std::make_shared<ImageWidget>(
                    "images/timer0.png",
                    (width-w)/2, (height-h)/2,
                    w, h,
                    50
        );

        // Show / hide the 3-2-1 counter.
        event_manager.add_delayed_call(3.0, [&, timer, go](){
            remove_widget(timer);
            add_widget(go);
        });
        event_manager.add_delayed_call(4.0, [&, go](){
            remove_widget(go);
            running_ = true;
        });

        // Create the back button.
        w = 240;
        h = 80;
        auto back_button = std::make_shared<HoverclickWidget<ImageWidget> >(
                    "images/back_button.png",
                    (width-w)/2, 5,
                    w, h,
                    2
        );
        attach_mouse_events(opts.mouse_, back_button);
        back_button->handle_click_ = [&](DiffType x, DiffType y){
            event_manager.post(Event(Event::MainMenuScreen));
        };
        add_widget(back_button);
    }

protected:

    void update_impl(float elapsed_time)
    {
        if (running_)
        {
            // Check if a new mole wave should be started.
            bool mole_out = false;
            for (auto b : mole_out_)
                if (b)
                    mole_out = true;
            if (!mole_out)
                start_wave();

            // Check if a mole was hit.
            if (opts.mouse_clicked_)
            {
                auto m = hovered_mole();
                if (m < 0 || !mole_out_[m] || mole_hit_[m])
                    miss();
                else
                    hit(m);
            }
        }
    }

private:

    void miss()
    {
        // TODO: What happens on a miss?
        std::cout << "daneben" << std::endl;
    }

    void hit(int i)
    {
        // Start the hide animation.
        moles_[i]->clear_actions();
        hide_mole(i);
        mole_hit_[i] = true;

        // TODO: What else happens on a hit?
        std::cout << "treffer auf " << i << std::endl;
    }

    /**
     * @brief Show 1-3 moles at random positions.
     */
    void start_wave()
    {
        // Set all moles to "in".
        std::fill(mole_out_.begin(), mole_out_.end(), false);
        std::fill(mole_hit_.begin(), mole_hit_.end(), false);

        // Fine the number of moles for this wave.
        std::uniform_int_distribution<int> mole_count(1, 3);
        int c = mole_count(rand_engine_);

        // Randomly select the moles.
        for (int i = 0; i < c; ++i)
        {
            std::uniform_int_distribution<int> rand_int(0, 8);
            int mole_index;
            do
            {
                mole_index = rand_int(rand_engine_);
            } while (mole_out_[mole_index]);
            show_mole(mole_index);
        }
    }

    /**
     * @brief Show the mole with index i.
     */
    void show_mole(int i)
    {
        // Start the animation.
        mole_out_[i] = true;
        moles_[i]->backwards(false);
        moles_[i]->restart();

        // Add an action that hides the mole after t seconds.
        std::uniform_real_distribution<float> rand(2.0, 3.5);
        float t = rand(rand_engine_);
        auto hide_action = std::make_shared<FunctionAction>([&, i](Widget & w, float elapsed_time){
                hide_mole(i);
                return true;
        });
        auto delay = std::make_shared<DelayedAction>(t-0.5, hide_action);
        moles_[i]->add_action(delay);
    }

    /**
     * @brief Hide the mole with index i.
     */
    void hide_mole(int i)
    {
        bool already_hiding = moles_[i]->running() && moles_[i]->backwards();
        if (!already_hiding)
        {
            moles_[i]->backwards(true);
            moles_[i]->restart();
            event_manager.add_delayed_call(0.5, [&, i]() {
                mole_out_[i] = false;
            });
        }
    }

    /**
     * @brief Return index of the hovered mole (or -1 of no mole is hovered).
     */
    int hovered_mole()
    {
        for (int i = 0; i < moles_.size(); ++i)
            if (moles_[i]->hovered())
                return i;
        return -1;
    }

    std::vector<MolePointer> moles_; // the mole widgets
    std::vector<bool> mole_out_; // the moles that are currently out
    std::vector<bool> mole_hit_; // whether a mole was hit in the current wave
    std::mt19937 rand_engine_; // the random engine
    bool running_; // whether the game started

};


} //namespace kin

#endif

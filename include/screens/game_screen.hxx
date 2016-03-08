#ifndef GAME_SCREEN_HXX
#define GAME_SCREEN_HXX

#include <random>
#include <memory>
#include <string>
#include <cmath>

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
          running_(false),
          total_time_(60),
          remaining_time_(total_time_),
          moletime_min_(2.0),
          moletime_max_(3.5),
          gmole_position_(0),
          gmole_wave_(0),
          score_(0),
          combo_count_(0),
          perfect_game_(true)
    {
        // Set all moles to "in".
        mole_out_.resize(9, false);
        mole_hit_.resize(9, false);

        // Add the mole widgets.
        auto const height = rect_.GetHeight();
        auto const width = rect_.GetWidth();

        auto timebar_height = height;
        auto timebar_width = std::ceil(0.194 * timebar_height);

        auto sprites_top = 0.25 * height;
        auto sprites_height = 0.22 * height;
        auto sprite_distance_y = 0.25 * height;
        auto sprites_width = 1.314*sprites_height;
        auto sprite_distance_x = sprites_width + 0.03 * width;
        auto sprite_total_width = 2 * sprite_distance_x + sprites_width;
        auto sprites_left = (width - sprite_total_width) / 2 - timebar_width/2;
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

        // Add the scoreboard.
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
                    (width-w-timebar_width)/2, (height-h)/2,
                    w, h,
                    50
        );
        timer->hoverable_ = false;
        timer->repeatable_ = false;
        timer->freeze_finish_ = true;
        add_widget(timer);

        // Create the shrink actions for the timer.
        auto f0 = std::make_shared<ShrinkAction>(1);
        auto f1 = std::make_shared<ShrinkAction>(1);
        auto f2 = std::make_shared<ShrinkAction>(1);

        // Create the resize actions for the timer.
        auto func = [width,w,height,h,timebar_width](Widget &wid, float elapsed_time){
            wid.rect_.Left = (width-w-timebar_width)/2;
            wid.rect_.Right = (width-w-timebar_width)/2 + w;
            wid.rect_.Top  = (height-h)/2;
            wid.rect_.Bottom = (height-h)/2 + h;
            return true;
        };
        auto resize_action1 = std::make_shared<FunctionAction>(func);
        auto resize_action2 = std::make_shared<FunctionAction>(func);

        // Chain the actions together.
        auto chain = std::make_shared<ChainedAction>(f0,resize_action1, f1,resize_action2,f2);
        timer->add_action(chain);

        // Create the "go" widget after the timer.
        h = height/3.5;
        w = 1.659 * h;
        auto go = std::make_shared<ImageWidget>(
                    "images/timer0.png",
                    (width-w-timebar_width)/2, (height-h)/2,
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
                    (width-w-timebar_width)/2, 5,
                    w, h,
                    2
        );
        attach_mouse_events(opts.mouse_, back_button);
        back_button->handle_click_ = [&](DiffType x, DiffType y){
            event_manager.post(Event(Event::MainMenuScreen));
        };
        add_widget(back_button);

        // Show the time bar.
        auto timebar = std::make_shared<ImageWidget>(
                    "images/timebar_frame.png",
                    width-timebar_width, 0,
                    timebar_width, timebar_height,
                    5);
        add_widget(timebar);
        timefill_original_top_ = std::ceil(0.00729 * timebar_height);
        timefill_original_height_ = std::ceil(0.985 * timebar_height);
        timefill_ = std::make_shared<ColorWidget>(
                    sf::Color(255, 149, 14),
                    width-timebar_width + std::ceil(0.0472*timebar_width),
                    timefill_original_top_,
                    std::ceil(0.913 * timebar_width),
                    timefill_original_height_,
                    6
        );
        add_widget(timefill_);

        // Find the position and wave of the golden mole.
        std::uniform_int_distribution<int> gmole_index(0, 8);
        gmole_position_ = gmole_index(opts.rand_engine_);
        int max_wave = std::floor(total_time_ / moletime_max_);
        std::uniform_int_distribution<int> gmole_wave_index(0, max_wave);
        gmole_wave_ = gmole_wave_index(opts.rand_engine_);

        // Make golden mole widget.
        gmole_ = std::make_shared<AnimatedWidget>(
                    "animations/mole_golden.pf",
                    sprites_left+(gmole_position_%3)*sprite_distance_x,
                    sprites_top+(gmole_position_/3)*sprite_distance_y,
                    sprites_width,sprites_height,
                    6);
        gmole_->repeatable_ = false;
        gmole_->freeze_finish_ = true;
        gmole_->stop();
        add_widget(gmole_);
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
                if (m >= 0 && mole_out_[m] && !mole_hit_[m])
                    hit(m);
                else
                    miss();
            }

            // Update the timer.
            remaining_time_ -= elapsed_time;
            if (remaining_time_ <= 0)
            {
                remaining_time_ = 0;
                time_up();
            }
            auto t = 1.0 - remaining_time_ / total_time_;
            auto delta = t * timefill_original_height_;
            timefill_->rect_.Top = timefill_original_top_ + delta;
        }
    }

private:

    /**
     * @brief Reset the combo counter.
     */
    void miss()
    {
        combo_count_ = 0;
        perfect_game_ = false;
    }

    /**
     * @brief Hide the mole that was hit.
     */
    void hit(int i)
    {
        // Post the hit-event.
        event_manager.post(Event(Event::MoleHit));
        combo_count_ += 1;

        // Start the hide animation.
        int points = 1;
        if(gmole_wave_ == -1 && gmole_position_ == i)
        {
            hide_gmole();
            points = 5;
        }
        else
        {
            moles_[i]->clear_actions();
            hide_mole(i);
        }
        mole_hit_[i] = true;

        // Add the points to the score.
        if (combo_count_ >= 10)
            points *= 2;
        score_ += points;

        // Show the pow animation.
        std::uniform_int_distribution<int> rand_int(0, 1);
        int pow_index = rand_int(opts.rand_engine_);
        std::string animation;
        float w_factor;
        if (pow_index == 0)
        {
            animation = "animations/pow.pf";
            w_factor = 1.417;
        }
        else
        {
            animation = "animations/hit.pf";
            w_factor = 1.109;
        }
        auto h = 0.568 * moles_[i]->rect_.GetHeight();
        auto w = w_factor * h;
        auto x = moles_[i]->rect_.Left + (moles_[i]->rect_.GetWidth()-w)/2;
        auto y = moles_[i]->rect_.Top + (moles_[i]->rect_.GetHeight())/2 - h;
        auto pow = std::make_shared<AnimatedWidget>(
                    animation,
                    x, y,
                    w, h,
                    6
        );
        add_widget(pow);
        event_manager.add_delayed_call(1.0, [&, pow](){
            remove_widget(pow);
        });

        // Add some movement to the pow animation.
        float move_top = 0.267 * h;
        float move_right = rand_int(opts.rand_engine_) == 0 ? -1 : 1;
        move_right *= move_top;
        auto moveact = std::make_shared<MoveByAction>(sf::Vector2f(move_right, -move_top), 1.0f);
        auto shrinkact = std::make_shared<ShrinkAction>(1.0f);
        pow->add_action(moveact);
        pow->add_action(shrinkact);
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
        int c = mole_count(opts.rand_engine_);

        //check if it is the golden mole wave.
        if(gmole_wave_== 0)
        {
            c--;
            show_gmole();
        }
        gmole_wave_--;

        // Randomly select the moles.
        for (int i = 0; i < c; ++i)
        {
            std::uniform_int_distribution<int> rand_int(0, 8);
            int mole_index;
            do
            {
                mole_index = rand_int(opts.rand_engine_);
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
        std::uniform_real_distribution<float> rand(moletime_min_, moletime_max_);
        float t = rand(opts.rand_engine_);
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
                if (!mole_hit_[i])
                    perfect_game_ = false;
            });
        }
    }

    void show_gmole()
    {
        gmole_->backwards(false);
        gmole_->restart();
        mole_out_[gmole_position_] = true;

        std::uniform_real_distribution<float> rand(moletime_min_, moletime_max_);
        float t = rand(opts.rand_engine_) / 2.0;
        auto hide_action = std::make_shared<FunctionAction>([&](Widget &w, float elapsed_time){
                hide_gmole();
                return true;
        });
        auto delay = std::make_shared<DelayedAction>(t-0.5, hide_action);
        gmole_->add_action(delay);
    }

    void hide_gmole()
    {
        bool already_hiding = gmole_->running() && gmole_->backwards();
        if (!already_hiding)
        {
            gmole_->backwards(true);
            gmole_->restart();
            event_manager.add_delayed_call(0.5,[&](){
                mole_out_[gmole_position_] = false;
                remove_widget(gmole_);
                if (!mole_hit_[gmole_position_])
                    perfect_game_ = false;
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
        if (gmole_->hovered())
            return gmole_position_;
        return -1;
    }

    /**
     * @brief Stop the game and show the score when the time is up.
     */
    void time_up()
    {
        std::cout << "TIME UP!" << std::endl;
        if (perfect_game_)
        {
            std::cout << "Perfect game" << std::endl;
            score_ *= 2;
        }
        else
        {
            std::cout << "Not perfect game" << std::endl;
        }
        std::cout << "Score: " << score_ << std::endl;

        running_ = false;
        auto width = rect_.GetWidth();
        auto height = rect_.GetHeight();
        auto h = 0.5 * height;
        auto w = 1.6 * h;
        auto timeup = std::make_shared<ImageWidget>(
                    "images/time_up.png",
                    (width-w-timefill_->rect_.GetWidth())/2, (height-h)/2,
                    w, h,
                    99
        );
        add_widget(timeup);
        event_manager.add_delayed_call(2.0, [&, timeup](){
            remove_widget(timeup);
        });
    }

    std::vector<MolePointer> moles_; // the mole widgets
    std::vector<bool> mole_out_; // the moles that are currently out
    std::vector<bool> mole_hit_; // whether a mole was hit in the current wave
    bool running_; // whether the game started
    float const total_time_; // the total time
    float remaining_time_; // the remaining time
    std::shared_ptr<ColorWidget> timefill_; // the time fill widget
    int timefill_original_height_; // the original height of the timefill widget
    int timefill_original_top_; // the original top position of the timefill widget
    size_t combo_count_;// the combo count of hitting moles, missing resets it to 0.
    int score_; //The current user score
    MolePointer gmole_;
    int gmole_position_; // the gmole position
    int gmole_wave_; // the wave the gmole will appear
    float moletime_min_; // min amount of time a mole is outside
    float moletime_max_; // max amount of time a mole is outside
    bool perfect_game_; // whether the user played a perfect game
};


} //namespace kin

#endif

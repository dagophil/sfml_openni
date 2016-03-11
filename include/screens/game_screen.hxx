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
          perfect_game_(true),
          combo_mult_(1)
    {
        // Hide the mouse.
        opts.mouse_->hide();


        // Set all moles to "in".
        mole_out_.resize(9, false);
        mole_hit_.resize(9, false);

        // Create the main grid (left: combo counter, center: the game, right: time).
        auto main_grid_ptr = std::make_shared<GridWidget>(3, 1);
        auto & main_grid = *main_grid_ptr;
        add_widget(main_grid_ptr);
        main_grid.set_x_sizes(0.14, 0.72, 0.14);

        // Create the moles.
        auto mole_grid_ptr = std::make_shared<GridWidget>(3, 3);
        auto & mole_grid = *mole_grid_ptr;
        main_grid(1) = mole_grid_ptr;
        mole_grid.set_y(0.3);
        mole_grid.set_width(0.8);
        mole_grid.set_height(0.66);
        mole_grid.align_x_ = CenterX;
        for (size_t i = 0; i < 9; ++i)
        {
            auto w = std::make_shared<AnimatedWidget>("animations/mole.pf");
            w->scale_ = ScaleInX;
            w->set_height(0.8);
            w->repeatable_ = false;
            w->freeze_finish_ = true;
            w->stop();
            mole_grid(i%3, i/3) = w;
            moles_.push_back(w);
        }

        // Create the 3-2-1 counter.
        auto timer = std::make_shared<AnimatedWidget>("animations/timer.pf", 50);
        timer->set_height(0.25);
        timer->scale_ = ScaleInX;
        timer->align_x_ = CenterX;
        timer->align_y_ = CenterY;
        timer->repeatable_ = false;
        timer->freeze_finish_ = true;
//        timer->hoverable_ = false;
        add_widget(timer);

        // Create the shrink actions for the timer.
        auto f0 = std::make_shared<ShrinkAction>(1);
        auto f1 = std::make_shared<ShrinkAction>(1);
        auto f2 = std::make_shared<ShrinkAction>(1);

        // Create the resize actions for the timer.
        auto func = [](Widget &wid, float elapsed_time){
            wid.set_height(0.25);
            return true;
        };
        auto resize_action1 = std::make_shared<FunctionAction>(func);
        auto resize_action2 = std::make_shared<FunctionAction>(func);

        // Chain the actions together.
        auto chain = std::make_shared<ChainedAction>(f0,resize_action1, f1,resize_action2,f2);
        timer->add_action(chain);

        // Create the "go" widget that appears after the timer.
        auto go = std::make_shared<ImageWidget>("images/timer0.png", 50);
        go->set_height(0.2857);
        go->scale_ = ScaleInX;
        go->align_x_ = CenterX;
        go->align_y_ = CenterY;

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
        auto back_button = std::make_shared<HoverclickWidget<ImageWidget> >("images/back_button.png");
        back_button->set_height(0.15);
        back_button->scale_ = ScaleInX;
        back_button->align_x_ = CenterX;
        back_button->set_y(0.05);
        attach_mouse_events(opts.mouse_, back_button);
        back_button->handle_click_ = [&](DiffType x, DiffType y){
            event_manager.post(Event(Event::MainMenuScreen));
        };
        add_widget(back_button);

        // Show the time bar.
        auto timebar = std::make_shared<ImageWidget>("images/timebar_frame.png");
        main_grid(2) = timebar;

        // Create the filler for the timer bar.
        timefill_original_height_ = 0.985;
        timefill_ = std::make_shared<ColorWidget>(sf::Color(255, 149, 14));
        timefill_->scale_ = None;
        timefill_->align_y_ = Bottom;
        timefill_->set_x(0.0472);
        timefill_->set_y(0.00729);
        timefill_->set_width(0.913);
        timefill_->set_height(timefill_original_height_);
        timebar->add_widget(timefill_);

// TODO: Add the scoreboard.
//        // Add the scoreboard.
//        auto s = std::make_shared<ImageWidget>("images/scoreboard.png", 1);
//        s->set_x((width-score_width)/2);
//        s->set_y(0);
//        s->set_width(score_width);
//        s->set_height(score_height);
//        add_widget(s);

        // Find the position and wave of the golden mole.
        std::uniform_int_distribution<int> gmole_index(0, 8);
        gmole_position_ = gmole_index(opts.rand_engine_);
        int max_wave = std::floor(total_time_ / moletime_max_);
        std::uniform_int_distribution<int> gmole_wave_index(0, max_wave);
        gmole_wave_ = gmole_wave_index(opts.rand_engine_);

        // Make golden mole widget.
        gmole_ = std::make_shared<AnimatedWidget>("animations/mole_golden.pf", 2);
        gmole_->repeatable_ = false;
        gmole_->freeze_finish_ = true;
        gmole_->stop();
        mole_grid(gmole_position_%3, gmole_position_/3)->add_widget(gmole_);

        // Create a grid for the combo counter.
        auto starbar = std::make_shared<ImageWidget>("images/combobar_frame.png");
        main_grid(0) = starbar;
        auto star_grid_ptr = std::make_shared<GridWidget>(1, 6);
        auto & star_grid = *star_grid_ptr;
        star_grid.set_height(0.9);
        star_grid.set_y(0.02);
        starbar->add_widget(star_grid_ptr);

        // Create the stars for the combo counter.
        for (size_t i = 0; i < 5; ++i)
        {
            auto star_gray = std::make_shared<ImageWidget>("images/star_gray.png");
            star_gray->scale_ = ScaleInX;
            star_gray->align_x_ = CenterX;
            star_grid(5-i) = star_gray;

            auto star_gold = std::make_shared<AnimatedWidget>("animations/star.pf");
            star_gold->freeze_finish_ = true;
            star_gold->repeatable_ = false;
            star_gold->stop();
            star_gold->hide();
            stars_.push_back(star_gold);
            star_gray->add_widget(star_gold);
        }

        // Create the combo counter.
        combo_counter_ = std::make_shared<AnimatedWidget>("animations/multi_sheet.pf");
        combo_counter_->stop();
        star_grid(0) = combo_counter_;


        std::vector<std::shared_ptr<ColorWidget> > targets;
        for (size_t i = 0; i < 9; ++i)
        {
            auto w = std::make_shared<ColorWidget>(sf::Color(255, 0, 0));
            targets.push_back(w);
            mole_grid(i%3, i/3)->add_widget(w);
            w->hide();
        }

        // Create the event listener.
        listener_ = std::make_shared<Listener>();
        listener_->handle_notify_ = [targets](Event const & ev){
            if (ev.type_ == Event::FieldHover)
            {
                for (auto w : targets)
                    w->hide();
                auto x = ev.field_hover_.x_;
                auto y = ev.field_hover_.y_;
                if (x != -1 && y != -1)
                {
                    targets[3*y + x]->show();
                }
            }
        };
        event_manager.register_listener(listener_);
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
            auto t = remaining_time_ / total_time_;
            timefill_->set_height(t * timefill_original_height_);
        }
    }

private:

    /**
     * @brief Reset the combo counter.
     */
    void miss()
    {
        combo_count_ = 0;
        for (auto star : stars_)
            star->hide();
        combo_mult_ = 1;
        combo_counter_->reset();
    }

    /**
     * @brief Hide the mole that was hit.
     */
    void hit(int i)
    {
        // Post the hit-event.
        event_manager.post(Event(Event::MoleHit));
        combo_count_ += 1;

        //While the combo guage is not full
        if (combo_count_ <= 10)
        {
            auto show_star = 0;

            //The star Position
            if(combo_count_ % stars_.size() == 0)
                show_star = 4;
            else
                show_star = combo_count_ % stars_.size() - 1;

            //Clear all Stars and animate the first one
            if (combo_count_ == 6)
            {
                for (auto star : stars_)
                    star->hide();
                show_star = 0;
            }

            stars_[show_star]->show();
            stars_[show_star]->restart();
        }
        //Combo multiplier after 5 succesful hits.
        if(combo_count_ == 5)
        {
            combo_mult_++;
            combo_counter_->next_frame();
        }
        else if(combo_count_ == 10)
        {
            combo_mult_++;
            combo_counter_->next_frame();
        }

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
        points *= combo_mult_;
        score_ += points;

        // Show the pow animation.
        std::uniform_int_distribution<int> rand_int(0, 1);
        int pow_index = rand_int(opts.rand_engine_);
        std::string animation;
        if (pow_index == 0)
            animation = "animations/pow.pf";
        else
            animation = "animations/hit.pf";
        auto pow = std::make_shared<AnimatedWidget>(animation, 10);
        pow->align_y_ = Top;
        pow->scale_ = ScaleInX;
        pow->set_x(0.2);
        pow->set_height(0.5);
        moles_[i]->add_widget(pow);
        event_manager.add_delayed_call(1.0, [&, pow](){
            remove_widget(pow);
        });
        pow_index = rand_int(opts.rand_engine_);
        if (pow_index == 0)
            pow->align_x_ = Right;
        else
            pow->align_x_ = Left;

        // Add some shrinking to the pow animation.
        auto shrinkact = std::make_shared<ShrinkAction>(1.0f);
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

        auto timeup = std::make_shared<ImageWidget>("images/time_up.png", 50);
        timeup->set_height(0.5);
        timeup->align_x_ = CenterX;
        timeup->align_y_ = CenterY;
        timeup->scale_ = ScaleInX;

        add_widget(timeup);
        event_manager.add_delayed_call(1.5, [&, timeup](){
            remove_widget(timeup);
        });
        check_highscore();
    }

    void check_highscore()
    {
        if(!dir_exist("highscore"))
        {
            mkdir("highscore", S_IRWXU|S_IRWXG);
            std::ofstream h("highscore/highscore.txt");

            for (size_t i = 0; i < 5; i++)
                h << 0 << "\n";
        }

        std::ifstream f("highscore/highscore.txt");

        if(!f.is_open())
        {
            f.close();

            std::ofstream h("highscore/highscore.txt");
            for (size_t i = 0; i < 5; i++)
                h << 0 << "\n";
        }

        std::vector<size_t> current_score;
        bool new_highscore = false;

        for(size_t i = 0; i < 5; ++i)
        {
            size_t t;
            f >> t;

            if (score_ > t && !new_highscore)
                new_highscore = true;

            current_score.push_back(t);
        }

        f.close();

        if(new_highscore)
        {
            auto high_ani = std::make_shared<AnimatedWidget>("animations/highscore.pf", 50);
            high_ani->set_width(0.5);
            high_ani->scale_ = ScaleInY;
            high_ani->align_x_ = CenterX;
            high_ani->align_y_ = CenterY;
            event_manager.add_delayed_call(1.6, [&, high_ani](){
                add_widget(high_ani);
            });

            event_manager.add_delayed_call(5.4, [&, high_ani](){
                remove_widget(high_ani);
            });

            current_score.push_back(score_);
            std::sort(current_score.begin(),current_score.end());
            std::reverse(current_score.begin(), current_score.end());
            std::ofstream h("highscore/highscore.txt", std::ofstream::trunc);

            for (size_t i = 0; i < 5; ++i)
                h << current_score[i] << "\n";

            h.close();
        }
    }

    std::vector<MolePointer> moles_; // the mole widgets
    std::vector<bool> mole_out_; // the moles that are currently out
    std::vector<bool> mole_hit_; // whether a mole was hit in the current wave
    bool running_; // whether the game started
    float const total_time_; // the total time
    float remaining_time_; // the remaining time
    std::shared_ptr<ColorWidget> timefill_; // the time fill widget
    float timefill_original_height_; // the original height of the timefill widget
    size_t combo_count_;// the combo count of hitting moles, missing resets it to 0
    int score_; //The current user score
    MolePointer gmole_;
    int gmole_position_; // the gmole position
    int gmole_wave_; // the wave the gmole will appear
    float moletime_min_; // min amount of time a mole is outside
    float moletime_max_; // max amount of time a mole is outside
    bool perfect_game_; // whether the user played a perfect game
    std::vector<MolePointer> stars_; //The golden stars
    size_t combo_mult_; // the current point multiplier
    std::shared_ptr<AnimatedWidget> combo_counter_; // the combo counter
    std::shared_ptr<Listener> listener_; // the event listener

};


} //namespace kin

#endif

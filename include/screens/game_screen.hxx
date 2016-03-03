#ifndef GAME_SCREEN_HXX
#define GAME_SCREEN_HXX

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
          Widget(args...)
    {
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
        add_widget(s);

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
        });
    }

protected:

    void update_impl(float elapsed_time)
    {
        if (opts.mouse_clicked_)
        {
            auto m = hovered_mole();
            if (m == nullptr)
            {
                std::cout << "daneben" << std::endl;
            }
            else
            {
                std::cout << "treffer" << std::endl;
            }
        }
    }

private:

    MolePointer hovered_mole()
    {
        for (auto m : moles_)
            if (m->hovered())
                return m;

        return nullptr;
    }

    std::vector<MolePointer> moles_;

};


} //namespace kin

#endif

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

        for (int i = 0; i < 9; i++)
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
        }

        auto s = std::make_shared<ImageWidget>(
                    "images/scoreboard.png",
                    (width-score_width)/2, 0,
                    score_width, score_height,
                    1);
        add_widget(s);
    }

private:

    std::vector<std::shared_ptr<AnimatedWidget> > moles_;

};


} //namespace kin

#endif

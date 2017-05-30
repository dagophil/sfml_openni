#ifndef HIGHSCORE_SCREEN_HXX
#define HIGHSCORE_SCREEN_HXX

#include <string>

#include <SFML/Graphics.hpp>

#include "../events.hxx"
#include "../widgets.hxx"
#include "../options.hxx"
#include "../utility.hxx"

namespace kin
{

class HighscoreScreen : public Widget
{
public:

    template <typename... Args>
    HighscoreScreen(
            Args... args
    )
        :
          Widget(args...)
    {
        auto grid_ptr = std::make_shared<GridWidget>(3,5);
        auto & grid = *grid_ptr;
        add_widget(grid_ptr);

        grid.set_x_sizes(0.1f, 0.2f, 0.2f);
        grid.set_height(0.9f);
        grid.set_width(0.6f);
        grid.align_x_ = CenterX;

        auto back_button = std::make_shared<HoverclickWidget<ImageWidget> >("images/back_button.png");
        back_button->set_height(0.1f);
        back_button->scale_ = ScaleInX;
        back_button->align_x_ = CenterX;
        back_button->set_y(0.9f);
        attach_mouse_events(opts.mouse_, back_button);
        back_button->handle_click_ = [](DiffType x, DiffType y){
            EventManager::instance().post(Event(Event::MainMenuScreen));
        };
        add_widget(back_button);

        auto add_mole = [&](std::string const & filename, int position)
        {
            auto m = std::make_shared<ImageWidget>(filename);
            m->scale_ = ScaleInX;
            m->align_x_ = CenterX;
            grid(0, position) = m;
        };
        add_mole("images/mole_crown_scepter.png", 0);
        add_mole("images/mole_scepter.png", 1);
        add_mole("images/mole_gold.png", 2);
        add_mole("images/mole.png", 3);
        add_mole("images/mole_no_shovel.png", 4);

        auto const dirname = "highscore";
        auto const filename = "highscore/highscore.txt";

        if(!dir_exist(dirname))
        {
            make_dir(dirname);
            std::ofstream h(filename);

            for (size_t i = 0; i < 5; i++)
                h << 0 << "\n";
        }
        if (!file_exist(filename))
        {
            std::ofstream f(filename);
            for (size_t i = 0; i < 5; ++i)
                f << 0 << "\n";
        }




//        if(!f.is_open())
//        {
//            f.close();

//            std::ofstream h(filename);
//            for (size_t i = 0; i < 5; i++)
//                h << 0 << "\n";
//        }

        auto help_font_size = opts.screen_height_ / 20;
        std::ifstream f(filename);
        if (!f.is_open())
            throw std::runtime_error(std::string("Could not open file: ") + filename);

        for (size_t i = 0; i < 5; i++)
        {
            size_t score;
            f >> score;
            auto v  = std::make_shared<TextWidget>(std::to_string(i + 1) + ".Platz",5);
            v->hoverable_ = false;
            v->text_align_y_ = CenterY;
            v->text_align_x_ = CenterX;
            v->bg_color_ = sf::Color(0, 0, 0, 128);
            v->style_ = sf::Text::Bold;
            v->font_size_ = help_font_size;
            grid(1,i) = v;

            auto w = std::make_shared<TextWidget>(std::to_string(score), 1);
            w->hoverable_ = false;
            w->text_align_x_ = CenterX;
            w->text_align_y_ = CenterY;
            w->bg_color_ = sf::Color(0, 0, 0, 128);
            w->style_ = sf::Text::Bold;
            w->font_size_ = help_font_size;
            grid(2,i) = w;
        }
    }

};



} //namespace kin

#endif

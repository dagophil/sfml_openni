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

class HighscoreScreen :public Widget
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
        auto &grid = *grid_ptr;
        add_widget(grid_ptr);

        grid.set_x_sizes(0.1, 0.2, 0.2);
        grid.set_height(0.9);
        grid.set_width(0.6);
        grid.align_x_ = CenterX;



        auto back_button = std::make_shared<HoverclickWidget<ImageWidget> >("images/back_button.png");
        back_button->set_height(0.1);
        back_button->scale_ = ScaleInX;
        back_button->align_x_ = CenterX;
        back_button->set_y(0.9);
        attach_mouse_events(opts.mouse_, back_button);
        back_button->handle_click_ = [&](DiffType x, DiffType y){
            event_manager.post(Event(Event::MainMenuScreen));
        };
        add_widget(back_button);

        auto mole_1 = std::make_shared<ImageWidget>("images/mole_crown_scepter.png", 4);
//        mole_1->set_height(h);
        mole_1->scale_ = ScaleInX;
        mole_1->align_x_ = CenterX;
//        mole_1->set_y(0);
//        mole_1->set_x(t);
        grid(0,0) = mole_1;

        auto mole_2 = std::make_shared<ImageWidget>("images/mole_scepter.png", 4);
//        mole_2->set_height(h);
        mole_2->scale_ = ScaleInX;
        mole_2->align_x_ = CenterX;
//        mole_2->set_y(h - gap);
//        mole_2->set_x(t);
        grid(0,1) = mole_2;

        auto mole_3 = std::make_shared<ImageWidget>("images/mole_gold.png", 4);
//        mole_3->set_height(h);
        mole_3->scale_ = ScaleInX;
        mole_3->align_x_ = CenterX;
//        mole_3->set_y(2 * h - gap);
//        mole_3->set_x(t);
        grid(0,2) = mole_3;

        auto mole_4 = std::make_shared<ImageWidget>("images/mole.png", 4);
//        mole_4->set_height(h);
        mole_4->scale_ = ScaleInX;
        mole_4->align_x_ = CenterX;
//        mole_4->set_y(3 * h - gap);
//        mole_4->set_x(t);
        grid(0,3) = mole_4;

        auto mole_5 = std::make_shared<ImageWidget>("images/mole_no_shovel.png", 4);
//        mole_4->set_height(h);
        mole_5->scale_ = ScaleInX;
        mole_5->align_x_ = CenterX;
//        mole_4->set_y(3 * h - gap);
//        mole_4->set_x(t);
        grid(0,4) = mole_5;

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

        auto help_font_size = opts.screen_height_ / 20;


        for (size_t i = 0; i < 5; i++)
        {
            size_t score;
            f >> score;


            auto v  = std::make_shared<TextWidget>(std::to_string(i + 1) + ".Platz",5);
//            v->scale_ = None;
            v->hoverable_ = false;
//            v->text_align_x_ = CenterX;
//            v->set_height(h-0.4);
            v->text_align_y_ = CenterY;
            v->text_align_x_ = CenterX;
            v->bg_color_ = sf::Color(0, 0, 0, 128);
//            v->set_x(t + 0.2);
//            v->set_y(s + i * h);
            v->style_ = sf::String::Bold;
            v->font_size_ = help_font_size;
            grid(1,i) = v;

            auto w = std::make_shared<TextWidget>(std::to_string(score), 1);
            w->hoverable_ = false;
//            w->scale_ = None;
            w->text_align_x_ = CenterX;
            w->text_align_y_ = CenterY;
//            w->set_width(0.7);
//            w->set_height(h-0.4);
//            w->set_y(s + i * h);
//            w->set_x(t + 0.08);
            w->bg_color_ = sf::Color(0, 0, 0, 128);
            w->style_ = sf::String::Bold;
            w->font_size_ = help_font_size;
            grid(2,i) = w;


//            scores_.push_back(w);
        }

//        for(int i = 0; i < 5; i++)
//            scores_[i]->show();

        f.close();


    }
private:
    std::vector<std::shared_ptr<TextWidget>> scores_; // The Highscores



};



} //namespace kin

#endif

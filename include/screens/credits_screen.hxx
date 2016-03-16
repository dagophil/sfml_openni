#ifndef CREDITS_SCREEN_HXX
#define CREDITS_SCREEN_HXX

#include <string>

#include "../events.hxx"
#include "../widgets.hxx"
#include "../options.hxx"

namespace kin
{

class CreditsScreen : public Widget
{
public:
    template <typename... Args>
    CreditsScreen(
            Args... args
    )
        :
          Widget(args...)
    {
        auto grid_ptr = std::make_shared<GridWidget>(2,1);
        auto & grid = *grid_ptr;
        add_widget(grid_ptr);
        grid.set_x_sizes(0.4, 0.4);
        grid.set_height(0.5);
        grid.set_width(0.8);
        grid.set_y(-0.15);
        grid.align_x_ = CenterX;
        grid.align_y_ = CenterY;

//        auto help_font_size = opts.screen_height_ /20;

        auto mole_phil = std::make_shared<ImageWidget>("images/mole_credits_phil.png");
        mole_phil->align_x_ = CenterX;
        mole_phil->align_y_ = Bottom;
        mole_phil->hoverable_ = false;
        mole_phil->scale_ = ScaleInX;
        grid(0,0) = mole_phil;

//        auto text_phil = std::make_shared<TextWidget>(" Created by: Philip Schill and ");
//        text_phil->text_align_x_ = CenterX;
//        text_phil->text_align_y_ = CenterY;
//        text_phil->bg_color_ = sf::Color(0, 0, 0, 82);
//        text_phil->style_ = sf::String::Bold;
//        text_phil->font_size_ = help_font_size;
//        grid(0,1) = text_phil;

        auto mole_flo = std::make_shared<ImageWidget>("images/mole_credits_flo.png");
        mole_flo->align_x_ = CenterX;
        mole_flo->align_y_ = Bottom;
        mole_flo->hoverable_ = false;
        mole_flo->scale_ = ScaleInX;
        grid(1,0) = mole_flo;

        auto credit_text = std::make_shared<ImageWidget>("images/credits_text.png");
        credit_text->hoverable_ = false;
        credit_text->set_height(0.2);
        credit_text->align_x_ = CenterX;
        credit_text->set_y(0.65);
        credit_text->scale_ = ScaleInX;
        add_widget(credit_text);

//        auto text_flo = std::make_shared<TextWidget>(" Florian Haegele");
//        text_flo->text_align_x_ = Left;
//        text_flo->text_align_y_ = CenterY;
//        text_flo->bg_color_ = sf::Color(0, 0, 0, 82);
//        text_flo->style_ = sf::String::Bold;
//        text_flo->font_size_ = help_font_size;
//        grid(1,1) = text_flo;

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
    }
};

} //namespace kin

#endif

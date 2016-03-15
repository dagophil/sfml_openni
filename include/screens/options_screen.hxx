#ifndef OPTIONS_SCREEN_HXX
#define OPTIONS_SCREEN_HXX

#include "../events.hxx"
#include "../widgets.hxx"
#include "../options.hxx"
#include "../utility.hxx"

namespace kin
{

class OptionsScreen : public Widget
{
public:

    template <typename... Args>
    OptionsScreen(
            Args... args
    )
        :
            Widget(args...)
    {
        // Create the grid background.
        auto gridbg = std::make_shared<ColorWidget>(sf::Color(0, 0, 0, 160));
        gridbg->set_width(0.8);
        gridbg->set_height(0.4);
        gridbg->align_x_ = CenterX;
        gridbg->set_y(0.3);
        add_widget(gridbg);

        // Create the grid.
        auto grid_ptr = std::make_shared<GridWidget>(2, 3);
        auto & grid = *grid_ptr;
        gridbg->add_widget(grid_ptr);

        // Get the font size.
        auto font_size = opts.screen_height_ / 25;

        // Add the reset highscore text.
        auto reset_text = std::make_shared<TextWidget>("RESET HIGHSCORE");
        reset_text->text_align_x_ = Right;
        reset_text->text_align_y_ = CenterY;
        reset_text->font_size_ = font_size;
        reset_text->style_ = sf::String::Bold;
        reset_text->set_x(-0.05);
        grid(0, 0) = reset_text;

        // Add the sound option text.
        auto sound_text = std::make_shared<TextWidget>(*reset_text);
        sound_text->text_ = "SOUND";
        grid(0, 1) = sound_text;

        // Add the y-z option text.
        auto use_depth = std::make_shared<TextWidget>(*reset_text);
        use_depth->text_ = "USE KINECT DEPTH";
        grid(0, 2) = use_depth;

        // Add the success text.
        auto success = std::make_shared<TextWidget>("Highscore was reset!");
        success->set_width(0.4);
        success->set_height(0.15);
        success->bg_color_ = sf::Color(0, 0, 0, 160);
        success->font_size_ = font_size;
        success->style_ = sf::String::Bold;
        success->align_x_ = Center;
        success->set_y(0.05);
        success->text_align_x_ = Center;
        success->text_align_y_ = Center;
        success->hide();
        add_widget(success);

        // Add the reset checkbox.
        auto reset_img = std::make_shared<HoverclickWidget<ColorWidget> >(sf::Color(255, 0, 0));
        reset_img->align_y_ = CenterY;
        reset_img->handle_click_ = [success](DiffType x, DiffType y){
            reset_highscore();
            success->show();
            event_manager.add_delayed_call(3.0, [success](){
                success->hide();
            });
        };
        attach_mouse_events(opts.mouse_, reset_img);
        grid(1, 0) = reset_img;

        // Create the back button.
        auto back_button = std::make_shared<HoverclickWidget<ImageWidget> >("images/back_button.png");
        back_button->set_height(0.15);
        back_button->scale_ = ScaleInX;
        back_button->align_x_ = CenterX;
        back_button->align_y_ = Bottom;
        back_button->set_y(0.05);
        attach_mouse_events(opts.mouse_, back_button);
        back_button->handle_click_ = [&](DiffType x, DiffType y){
            event_manager.post(Event(Event::MainMenuScreen));
        };
        add_widget(back_button);
    }

private:
};

} //namespace kin

#endif

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
        if (opts.use_kinect_)
        {
            auto use_depth = std::make_shared<TextWidget>(*reset_text);
            use_depth->text_ = "USE KINECT DEPTH";
            grid(0, 2) = use_depth;
        }

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

        // Create the checkboxes.
        auto create_checkbox = [&](int position){
            auto checkbox = std::make_shared<HoverclickWidget<AnimatedWidget> >("animations/checkbox.pf");
            checkbox->scale_ = ScaleInX;
            checkbox->set_height(0.8);
            checkbox->align_y_ = Center;
            checkbox->stop();
            attach_mouse_events(opts.mouse_, checkbox);
            grid(1, position) = checkbox;
            return checkbox;
        };

        // Program the reset checkbox.
        auto reset_box = create_checkbox(0);
        reset_box->handle_click_ = [success, reset_box](DiffType x, DiffType y){
            reset_highscore();
            success->show();
            event_manager.add_delayed_call(3.0, [success](){
                success->hide();
            });
            reset_box->next_frame();
            reset_box->handle_mouse_enter_ = detail::do_nothing2<DiffType, DiffType>;
            reset_box->handle_mouse_leave_ = detail::do_nothing2<DiffType, DiffType>;
            reset_box->handle_click_ = detail::do_nothing2<DiffType, DiffType>;
        };

        // Program the sound checkbox.
        auto sound_box = create_checkbox(1);
        if (opts.sound_)
            sound_box->next_frame();
        sound_box->handle_click_ = [sound_box](DiffType x, DiffType y){
            opts.sound_ = !opts.sound_;
            sound_box->next_frame();
        };

        // Program the depth checkbox.
        if (opts.use_kinect_)
        {
            auto depth_box = create_checkbox(2);
            if (opts.kinect_game_depth_)
                depth_box->next_frame();
            depth_box->handle_click_ = [depth_box](DiffType x, DiffType y){
                opts.kinect_game_depth_ = !opts.kinect_game_depth_;
                depth_box->next_frame();
            };
        }

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

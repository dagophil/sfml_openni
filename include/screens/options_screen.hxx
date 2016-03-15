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
        auto gridbg = std::make_shared<ColorWidget>(sf::Color(0, 0, 0, 128));
        gridbg->scale_ = None;
        gridbg->set_width(0.8);
        gridbg->set_height(0.7);
        gridbg->align_x_ = CenterX;
        gridbg->set_y(0.1);
        add_widget(gridbg);

        // Create the grid.
        auto grid_ptr = std::make_shared<GridWidget>(2, 3);
        auto & grid = *grid_ptr;
        gridbg->add_widget(grid_ptr);

        // Create the back button.
        auto back_button = std::make_shared<HoverclickWidget<ImageWidget> >("images/back_button.png");
        back_button->set_height(0.1);
        back_button->scale_ = ScaleInX;
        back_button->align_x_ = CenterX;
        back_button->align_y_ = Bottom;
        back_button->set_y(0.05);
        attach_mouse_events(opts.mouse_, back_button);
        back_button->handle_click_ = [&](DiffType x, DiffType y){
            event_manager.post(Event(Event::MainMenuScreen));
        };
        add_widget(back_button);

        auto checkbox_checked = std::make_shared<HoverclickWidget<ImageWidget> >("images/checkbox_checked.png");
        checkbox_checked->align_x_ = Left;
        checkbox_checked->align_y_ = CenterY;
        checkbox_checked->scale_ = ScaleInX;
        checkbox_checked->set_height(0.5);
        grid(1,1) = checkbox_checked;
        grid(1,2) = checkbox_checked;
    }

private:
};

} //namespace kin

#endif

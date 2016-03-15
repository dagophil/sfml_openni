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
        auto grid_ptr = std::make_shared<GridWidget>(2,2);
        auto & grid = *grid_ptr;
        add_widget(grid_ptr);

        grid.set_x_sizes(0.2, 0.5);
        grid.set_height(0.5);
        grid.set_width(0.8);
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
    }

private:
};

} //namespace kin

#endif

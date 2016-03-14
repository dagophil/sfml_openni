#ifndef MANUAL_SCREEN_HXX
#define MANUAL_SCREEN_HXX

#include <memory>

#include "../events.hxx"
#include "../utility.hxx"
#include "../options.hxx"

namespace kin
{

class ManualScreen : public Widget
{
public:

    template <typename... Args>
    ManualScreen(
            Args... args
    )
        :
          Widget(args...)
    {
        auto w = 100;
        auto h = 77;
        auto offset = 30;
        auto x = render_rect_.GetWidth() - w - offset;
        auto y = render_rect_.GetHeight() - h - offset;

        auto arrow_right = std::make_shared<HoverclickWidget<ImageWidget> >("images/arrow_right.png", 0);
        arrow_right->set_height(0.15);
        arrow_right->scale_ = ScaleInX;
        arrow_right->align_x_ = CenterX;
        arrow_right->set_y(0.8);
        arrow_right->set_x(0.3);
        attach_mouse_events(opts.mouse_, arrow_right);
        add_widget(arrow_right);

        auto arrow_left = std::make_shared<HoverclickWidget<ImageWidget> >("images/arrow_left.png", 0);
        arrow_left->set_height(0.15);
        arrow_left->scale_ = ScaleInX;
        arrow_left->align_x_ = CenterX;
        arrow_left->set_y(0.8);
        arrow_left->set_x(-0.3);
        attach_mouse_events(opts.mouse_, arrow_left);
        add_widget(arrow_left);

        auto back_button = std::make_shared<HoverclickWidget<ImageWidget> >("images/back_button.png");
        back_button->set_height(0.15);
        back_button->scale_ = ScaleInX;
        back_button->align_x_ = CenterX;
        back_button->set_y(0.8);
        attach_mouse_events(opts.mouse_, back_button);
        back_button->handle_click_ = [&](DiffType x, DiffType y){
            event_manager.post(Event(Event::MainMenuScreen));
        };
        add_widget(back_button);
    }
};

} // namespace kin

#endif

#ifndef MANUAL_SCREEN_HXX
#define MANUAL_SCREEN_HXX

#include <memory>

#include "../events.hxx"
#include "../utility.hxx"

namespace kin
{

template <typename T>
class ManualScreen : public Widget
{
public:

    template <typename... Args>
    ManualScreen(EventManager & event_manager, T mouse, Args... args)
        :
          Widget(args...),
          event_manager_(event_manager),
          mouse_(mouse)
    {
        auto w = 100;
        auto h = 77;
        auto offset = 30;
        auto x = rect_.GetWidth() - w - offset;
        auto y = rect_.GetHeight() - h - offset;

        auto arrow_right = std::make_shared<HoverclickWidget<ImageWidget> >(
                    "images/arrow_right.png",
                    x, y,
                    w, h,
                    0
        );
        attach_mouse_events(mouse_, arrow_right);
        add_widget(arrow_right);

        auto arrow_left = std::make_shared<HoverclickWidget<ImageWidget> >(
                    "images/arrow_left.png",
                    offset, y,
                    w, h,
                    0
        );
        attach_mouse_events(mouse_, arrow_left);
        add_widget(arrow_left);

        w = 240;
        h = 80;
        x = (rect_.GetWidth() - w) / 2;
        y = rect_.GetHeight() - h - offset;
        auto back_button = std::make_shared<HoverclickWidget<ImageWidget> >(
                    "images/back_button.png",
                    x, y,
                    w, h,
                    0
        );
        attach_mouse_events(mouse_, back_button);
        back_button->handle_click_ = [&](DiffType x, DiffType y){
            Event ev(Event::ChangeScreen);
            ev.change_screen_.screen_id = Event::MainMenuScreen;
            event_manager_.post(ev);
        };
        add_widget(back_button);
    }

private:

    EventManager & event_manager_;
    T mouse_;

};

} // namespace kin

#endif

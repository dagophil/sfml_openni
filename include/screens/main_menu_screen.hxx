#ifndef MAIN_MENU_SCREEN_HXX
#define MAIN_MENU_SCREEN_HXX

#include "../events.hxx"

namespace kin
{

template <typename T>
class MainMenuScreen : public Widget
{
public:

    template <typename... Args>
    MainMenuScreen(EventManager & event_manager, T mouse, Args... args)
        :
          Widget(args...),
          event_manager_(event_manager),
          mouse_(mouse)
    {
        auto w = 240;
        auto h = 80;
        auto x = (rect_.GetWidth()-w)/2;
        auto gap = 20;
        auto n = 5;
        auto y = (rect_.GetHeight() - n*h - (n-1)*gap) / 2;

        // Create the buttons.
        auto btn0 = std::make_shared<HoverclickWidget<ImageWidget> >(
                    "images/start_button.png",
                    x, y,
                    w, h,
                    1
        );
        attach_mouse_events(mouse_, btn0);
        add_widget(btn0);
        btn0->handle_click_ = [&](DiffType x, DiffType y){
            Event ev(Event::ChangeScreen);
            ev.change_screen_.screen_id = Event::GameScreen;
            event_manager_.post(ev);
        };

        auto btn1 = std::make_shared<HoverclickWidget<ImageWidget> >(
                    "images/highscore_button.png",
                    x, y + h+gap,
                    w, h,
                    1
        );
        attach_mouse_events(mouse_, btn1);
        add_widget(btn1);
        btn1->handle_click_ = [&](DiffType x, DiffType y){
            Event ev(Event::ChangeScreen);
            ev.change_screen_.screen_id = Event::HighscoreScreen;
            event_manager_.post(ev);
        };

        auto btn2 = std::make_shared<HoverclickWidget<ImageWidget> >(
                    "images/manual_button.png",
                    x, y + 2*(h+gap),
                    w, h,
                    1
        );
        attach_mouse_events(mouse_, btn2);
        add_widget(btn2);
        btn2->handle_click_ = [&](DiffType x, DiffType y){
            Event ev(Event::ChangeScreen);
            ev.change_screen_.screen_id = Event::ManualScreen;
            event_manager_.post(ev);
        };

        auto btn3 = std::make_shared<HoverclickWidget<ImageWidget> >(
                    "images/credits_button.png",
                    x, y + 3*(h+gap),
                    w, h,
                    1
        );
        attach_mouse_events(mouse_, btn3);
        add_widget(btn3);
        btn3->handle_click_ = [&](DiffType x, DiffType y){
            Event ev(Event::ChangeScreen);
            ev.change_screen_.screen_id = Event::CreditsScreen;
            event_manager_.post(ev);
        };

        auto btn4 = std::make_shared<HoverclickWidget<ImageWidget> >(
                    "images/exit_button.png",
                    x, y + 4*(h+gap),
                    w, h,
                    1
        );
        attach_mouse_events(mouse_, btn4);
        add_widget(btn4);
        btn4->handle_click_ = [&](DiffType x, DiffType y){
            event_manager_.post(Event(Event::Close));
        };
    }

private:

    EventManager & event_manager_;
    T mouse_;

};

} // namespace kin

#endif

#ifndef MAIN_MENU_SCREEN_HXX
#define MAIN_MENU_SCREEN_HXX

#include "../events.hxx"
#include "../widgets.hxx"

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

        std::vector<std::pair<std::string, Event> > v {
            {"images/start_button.png", Event::ChangeScreenEvent(Event::GameScreen)},
            {"images/highscore_button.png", Event::ChangeScreenEvent(Event::HighscoreScreen)},
            {"images/manual_button.png", Event::ChangeScreenEvent(Event::ManualScreen)},
            {"images/credits_button.png", Event::ChangeScreenEvent(Event::CreditsScreen)},
            {"images/exit_button.png", Event(Event::Close)}
        };

        for (size_t i = 0; i < v.size(); ++i)
        {
            auto const & p = v[i];
            auto btn = std::make_shared<HoverclickWidget<ImageWidget> >(
                        p.first,
                        x, y + i*(h+gap),
                        w, h,
                        1
            );
            attach_mouse_events(mouse_, btn);
            add_widget(btn);
            btn->handle_click_ = [&, p](DiffType x, DiffType y){
                event_manager.post(p.second);
            };
        }
    }

private:

    EventManager & event_manager_;
    T mouse_;

};

} // namespace kin

#endif

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
    MainMenuScreen(
            EventManager & event_manager,
            T mouse,
            Args... args
    )
        :
          Widget(args...),
          event_manager_(event_manager),
          mouse_(mouse)
    {
        // Fill the vector with the images and the according events.
        std::vector<std::pair<std::string, Event> > v {
            {"images/start_button.png", Event::ChangeScreenEvent(Event::GameScreen)},
            {"images/highscore_button.png", Event::ChangeScreenEvent(Event::HighscoreScreen)},
            {"images/manual_button.png", Event::ChangeScreenEvent(Event::ManualScreen)},
            {"images/credits_button.png", Event::ChangeScreenEvent(Event::CreditsScreen)},
            {"images/exit_button.png", Event(Event::Close)}
        };

        // Some variables for the button positions.
        auto h_gap = rect_.GetHeight() / (v.size()+1.0);
        auto h = 0.8*h_gap;
        auto gap = 0.2*h_gap;
        auto w = 2.76*h;
        auto x = (rect_.GetWidth()-w)/2;
        auto y0 = (rect_.GetHeight() - v.size()*(h+gap) + gap) / 2;

        // Draw the buttons.
        for (size_t i = 0; i < v.size(); ++i)
        {
            auto const & img = v[i].first;
            auto const & ev = v[i].second;
            auto btn = std::make_shared<HoverclickWidget<ImageWidget> >(
                        img,
                        x, y0 + i*h_gap,
                        w, h,
                        1
            );
            attach_mouse_events(mouse_, btn);
            add_widget(btn);
            btn->handle_click_ = [&, ev](DiffType x, DiffType y){
                event_manager.post(ev);
            };
        }
    }

private:

    EventManager & event_manager_;
    T mouse_;

};

} // namespace kin

#endif

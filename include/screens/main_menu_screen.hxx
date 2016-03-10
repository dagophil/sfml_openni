#ifndef MAIN_MENU_SCREEN_HXX
#define MAIN_MENU_SCREEN_HXX

#include "../events.hxx"
#include "../widgets.hxx"
#include "../options.hxx"

namespace kin
{

class MainMenuScreen : public Widget
{
public:

    template <typename... Args>
    MainMenuScreen(
            Args... args
    )
        :
          Widget(args...)
    {
        // Fill the vector with the images and the according events.
        std::vector<std::pair<std::string, Event> > v {
            {"images/start_button.png", Event::ChangeScreenEvent(Event::GameScreen)},
            {"images/highscore_button.png", Event::ChangeScreenEvent(Event::HighscoreScreen)},
            {"images/manual_button.png", Event::ChangeScreenEvent(Event::ManualScreen)},
            {"images/credits_button.png", Event::ChangeScreenEvent(Event::CreditsScreen)},
            {"images/exit_button.png", Event(Event::Close)}
        };

        // Create the grid for the menu buttons.
        auto grid_ptr = std::make_shared<GridWidget>(1, 2*v.size()-1);
        add_widget(grid_ptr);
        auto & grid = *grid_ptr;
        auto gap_height = 0.3333;
        grid.set_y_sizes(1.0, gap_height, 1.0, gap_height, 1.0, gap_height, 1.0, gap_height, 1.0);
        grid.scale_ = None;
        grid.align_x_ = CenterX;
        grid.align_y_ = CenterY;
        grid.set_width(0.333);
        grid.set_height(0.8);

        // Draw the buttons.
        for (size_t i = 0; i < v.size(); ++i)
        {
            auto const & img = v[i].first;
            auto const & ev = v[i].second;
            auto btn = std::make_shared<HoverclickWidget<ImageWidget> >(img);
            btn->scale_ = ScaleInX;
            btn->align_x_ = CenterX;
            attach_mouse_events(opts.mouse_, btn);
            btn->handle_click_ = [&, ev](DiffType x, DiffType y){
                event_manager.post(ev);
            };
            grid(2*i) = btn;
        }
    }
};

} // namespace kin

#endif

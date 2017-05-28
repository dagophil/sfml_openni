#ifndef SPLASH_SCREEN_HXX
#define SPLASH_SCREEN_HXX

#include "../events.hxx"
#include "../widgets.hxx"
#include "../options.hxx"

namespace kin
{

class SplashScreen : public Widget
{
public:

    template <typename... Args>
    SplashScreen(
            Args... args
    )
        :
          Widget(args...)
    {
        // Show the title.
        auto title = std::make_shared<ImageWidget>("images/title.png");
        title->scale_ = ScaleInX;
        title->set_height(0.333f);
        title->set_y(0.0833f);
        title->align_x_ = CenterX;
        add_widget(title);

        // Show the mole.
        auto mole = std::make_shared<HoverclickWidget<ImageWidget> >("images/mole.png");
        mole->scale_ = ScaleInX;
        mole->set_height(0.435f);
        mole->set_y(0.2825f);
        mole->align_x_ = CenterX;
        attach_mouse_events(opts.mouse_, mole);
        mole->handle_click_ = [&](DiffType x, DiffType y) {
            event_manager.post(Event(Event::MainMenuScreen));
        };
        add_widget(mole);

        // Show the splash text.
        auto splash_text = std::make_shared<AnimatedWidget >("animations/splash_text.pf");
        splash_text->scale_ = ScaleInX;
        splash_text->set_height(0.13f);
        splash_text->set_y(0.8f);
        splash_text->align_x_ = CenterX;
        add_widget(splash_text);
    }

};

}

#endif

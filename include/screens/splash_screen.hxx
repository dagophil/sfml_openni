#ifndef SPLASH_SCREEN_HXX
#define SPLASH_SCREEN_HXX

#include "../events.hxx"
#include "../widgets.hxx"

namespace kin
{

template <typename T>
class SplashScreen : public Widget
{
public:

    template <typename... Args>
    SplashScreen(
            EventManager & event_manager,
            T mouse,
            Args... args
    )
        :
          Widget(args...),
          event_manager_(event_manager),
          mouse_(mouse)
    {
        auto width = rect_.GetWidth();
        auto height = rect_.GetHeight();

        // Show the title.
        auto title = std::make_shared<ImageWidget>(
                    "images/title.png",
                    width/2,
                    height/12,
                    0,
                    height/3,
                    1
        );
        title->scale_style_ = ImageWidget::FitY;
        title->align_x_ = ImageWidget::CenterX;
        add_widget(title);

        // Show the mole.
        auto h = height/2.3;
        auto w = 0.73 * h;
        auto mole = std::make_shared<HoverclickWidget<ImageWidget> >(
                    "images/mole.png",
                    (width-w)/2,
                    (height-h)/2+0.02*height,
                    w,
                    h,
                    1
        );
        attach_mouse_events(mouse_, mole);
        mole->handle_click_ = [&](DiffType x, DiffType y) {
            event_manager_.post(Event(Event::MainMenuScreen));
        };
        add_widget(mole);

        // Show the splash text.
        w = 0.8*width;
        h = 0.08*w;

        auto splash_text = std::make_shared<AnimatedWidget >(
                    "animations/splash_text.pf",
                    0.1*width,
                    height - 2.0*h,
                    w,
                    h,
                    1
        );
        add_widget(splash_text);
    }

private:

    EventManager & event_manager_;
    T mouse_;

};

}

#endif

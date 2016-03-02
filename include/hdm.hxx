#ifndef HDM_HXX
#define HDM_HXX

#include <SFML/Graphics.hpp>

#include "widgets.hxx"
#include "events.hxx"
#include "options.hxx"
#include "screens/manual_screen.hxx"
#include "screens/main_menu_screen.hxx"

namespace kin
{

class HDMGame : public Widget
{
public:

    typedef std::shared_ptr<AnimatedWidget> MouseType;

    HDMGame(size_t width, size_t height);

    std::function<void()> handle_close_;

protected:

    /**
     * @brief Post the tick event.
     */
    void update_impl(float elapsed_time);

private:

    void notify(Event const & event); // callback for events
    void load_splash_screen(); // load the widgets for the splash screen
    void load_main_menu_screen(); // load the widgets for the main menu screen
    void load_manual_screen(); // load the widgets for the manual screen

    MouseType mouse_; // the mouse widget
    std::shared_ptr<Widget> container_; // the main container
    EventManager event_manager_; // the event manager
    std::shared_ptr<Listener> listener_; // the main listener

};

HDMGame::HDMGame(
        size_t width,
        size_t height
)   :
      Widget(0, 0, width, height, 0),
      handle_close_(detail::do_nothing0)
{
    // Load the mouse.
    mouse_ = std::make_shared<AnimatedWidget>(
                "animations/hand_load_2s.pf",
                width, height,
                75, 75,
                999
    );
    mouse_->hoverable_ = false;
    mouse_->stop();
    mouse_->repeatable_ = false;
    add_widget(mouse_);
    handle_hover_ = [&](DiffType x, DiffType y)
    {
        auto old_x = mouse_->rect_.Left;
        auto old_y = mouse_->rect_.Top;
        auto w = mouse_->rect_.GetWidth();
        auto h = mouse_->rect_.GetHeight();
        mouse_->rect_.Offset(x-old_x - 0.45*w, y-old_y - 0.17*h);
    };

    // Add the background image.
    add_widget(std::make_shared<ImageWidget>(
            "images/bg_grass.jpg",
            0, 0,
            width, height,
            0
    ));

    // Create the container.
    container_ = std::make_shared<Widget>(0, 0, width, height, 0);
    add_widget(container_);

    // Register the listener.
    listener_ = std::make_shared<Listener>();
    listener_->handle_notify_ = [&](Event const & event)
    {
        notify(event);
    };
    event_manager_.register_listener(listener_);

    // Load the splash screen.
    load_splash_screen();
//    load_main_menu_screen();
//    load_manual_screen();
}

void HDMGame::update_impl(float elapsed_time)
{
    event_manager_.post(Event(Event::Tick));
}

void HDMGame::notify(Event const & event)
{
    if (event.type_ == Event::ChangeScreen)
    {
        container_->clear_widgets();
        auto const id = event.change_screen_.screen_id;
        if (id == Event::SplashScreen)
            load_splash_screen();
        else if (id == Event::MainMenuScreen)
            load_main_menu_screen();
        else if (id == Event::ManualScreen)
            load_manual_screen();
        else
            throw std::runtime_error("Unknown screen.");
    }
    else if (event.type_ == Event::Close)
    {
        handle_close_();
    }
}

void HDMGame::load_splash_screen()
{
    auto w = container_->rect_.GetWidth();
    auto h = container_->rect_.GetHeight();

    // Show the title.
    auto title = std::make_shared<ImageWidget>(
                "images/title.png",
                w/2 - 300,
                h/2 - 260,
                600,
                200,
                1
    );
    container_->add_widget(title);

    // Show the mole.
    auto mole = std::make_shared<HoverclickWidget<ImageWidget> >(
                "images/mole.png",
                w/2 - 90,
                h/2 - 145,
                180,
                290,
                1
    );
    attach_mouse_events(mouse_, mole);
    mole->handle_click_ = [&](DiffType x, DiffType y) {
        Event ev(Event::EventType::ChangeScreen);
        ev.change_screen_.screen_id = Event::MainMenuScreen;
        event_manager_.post(ev);
    };
    container_->add_widget(mole);

    // Show the splash text.
    auto splash_text = std::make_shared<AnimatedWidget >(
                "animations/splash_text.pf",
                110,
                h - 120,
                w-220,
                60,
                1
    );
    container_->add_widget(splash_text);
}

void HDMGame::load_main_menu_screen()
{
    auto screen = std::make_shared<MainMenuScreen<MouseType> >(
                event_manager_,
                mouse_,
                0, 0,
                container_->rect_.GetWidth(),
                container_->rect_.GetHeight(),
                1
    );
    container_->add_widget(screen);
}

void HDMGame::load_manual_screen()
{
    auto screen = std::make_shared<ManualScreen<MouseType> >(
                event_manager_,
                mouse_,
                0, 0,
                container_->rect_.GetWidth(),
                container_->rect_.GetHeight(),
                1
    );
    container_->add_widget(screen);
}



}

#endif

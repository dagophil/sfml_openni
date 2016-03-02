#ifndef HDM_HXX
#define HDM_HXX

#include <SFML/Graphics.hpp>

#include "widgets.hxx"
#include "events.hxx"
#include "options.hxx"

namespace kin
{

class HDMGame : public Widget
{
public:

    HDMGame(size_t width, size_t height);

protected:

    void update_impl(float elapsed_time);

private:

    void notify(Event const & event);
    void load_splash_screen();
    void load_main_menu_screen();

    std::shared_ptr<AnimatedWidget> mouse_; // the mouse widget
    std::shared_ptr<Widget> container_; // the main container
    EventManager event_manager_; // the event manager
    std::shared_ptr<Listener> listener_; // the main listener

};

HDMGame::HDMGame(
        size_t width,
        size_t height
)   :
      Widget(0, 0, width, height, 0)
{
    // Load the mouse.
    mouse_ = std::make_shared<AnimatedWidget>(
                "animations/hand_load.pf",
                width, height,
                75, 75,
                999
    );
    mouse_->hoverable_ = false;
    mouse_->stop();
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
        else if (id == Event::MainMenu)
            load_main_menu_screen();
    }
}

void HDMGame::load_splash_screen()
{
    auto w = container_->rect_.GetWidth();
    auto h = container_->rect_.GetHeight();
    auto btn = std::make_shared<HoverclickWidget<ImageWidget> >(
                "images/start_button.png",
                w/2-120,
                h/2-40,
                240,
                80,
                1
    );
    btn->handle_click_ = [&](DiffType x, DiffType y) {
        Event ev(Event::EventType::ChangeScreen);
        ev.change_screen_.screen_id = Event::MainMenu;
        event_manager_.post(ev);
    };

    container_->add_widget(btn);
}

void HDMGame::load_main_menu_screen()
{

}



}

#endif
